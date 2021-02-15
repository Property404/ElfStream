#include "Agent.h"
#include "inject.h"
#include "FileUtil.h"
#include "scrub.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <fcntl.h>

#include <cassert>
#include <iostream>
#include <stdexcept>

struct Agent::Impl
{
	pid_t pid;
	size_t block_size;

	// File name to send stdout to
	// Used for testing
	std::string stdout_redirect_file = "";

};

void Agent::redirectOutput(std::string file_name)
{
	pimpl->stdout_redirect_file = std::move(file_name);
}

std::string Agent::expandBlankElf() const
{
	std::vector<Range> ranges;
	const auto contents = merchant->getBlankElf(ranges);
	return expandScrubbedElf(contents, ranges);
};

void Agent::spawn()
{
	// First fetch blank elf file
	const std::string file_name = "/tmp/_blank."+std::to_string(rand())+".elf";
	FileUtil::setFileContents(file_name, expandBlankElf());
	// Make sure it's executable
	system((std::string("chmod +x ")+file_name).c_str());
	
	const pid_t pid = fork();
	if(!pid)
	{
		// Redirect output if necessary
		if(pimpl->stdout_redirect_file != "")
		{
			int output_file = open(pimpl->stdout_redirect_file.c_str(), O_RDWR|O_CREAT, 0600);
			dup2(output_file, fileno(stdout));
			close(output_file);
		}

		// Allow agent to attach - inferior will pause after exec
		if(ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0)
			throw("ptrace(PTRACE_TRACEME...) failed");

		if(execl(file_name.c_str(), "PROGRAM_NAME", nullptr))
		{
			perror("Huh");
			throw std::runtime_error("Exec unsuccessful");
		}
	}
	this->pimpl->pid = pid;
}

void* Agent::createInjectionSite()
{
	const void*const bootstrap_site = merchant->textStart();
	struct user_regs_struct regs;

	// Get injection site location
	if(0>ptrace(PTRACE_GETREGS, pimpl->pid, nullptr, &regs))
		throw std::runtime_error("Couldn't get registers");
	void*const injection_site = merchant->alignToBlockStart((void*)(regs.rsp-16));

	// Set injection site as executable
	if(inject_syscall_mprotect(pimpl->pid, bootstrap_site, injection_site, merchant->getBlockSize(),
				PROT_EXEC|PROT_WRITE|PROT_READ))
		throw std::runtime_error("Could not bootstrap injection region");

	return injection_site;
}

void Agent::run()
{
	const auto pid = this->pimpl->pid;
	int wait_status{};
	waitpid(pid, &wait_status, 0);


	// Protect region
	const auto region_base = merchant->memoryStart();
	const auto region_size = merchant->memorySize()-0x13000;
	const void*const injection_site = createInjectionSite();
	int status=0;
	if((status = inject_syscall_mprotect(pid, injection_site, region_base, region_size, PROT_NONE)))
	{
		std::cerr<<"Warning: inject_syscall_mprotect() returned error: "<<std::dec<<status<<std::endl;
		std::cerr<<"\t"<<std::hex<<region_base <<"("<<region_size<<")"<<std::endl;
	}

	while(WIFSTOPPED(wait_status))
	{
		// Continue on our way
		if(ptrace(PTRACE_CONT, pid, 0, 0) < 0)
			throw std::runtime_error("ptrace(PTRACE_CONT...) failed");

		// Wait for segfault or somethign
		waitpid(pid, &wait_status, 0);

		if(WIFEXITED(wait_status))
			break;

		// Get segfault info
		siginfo_t si;
		if(0>ptrace(PTRACE_GETSIGINFO, pid, nullptr, &si))
			throw std::runtime_error("ptrace(PTRACE_GETSIGINFO...) failed");
		if(si.si_signo != SIGSEGV)
			continue;

		// Fix region and unprotect
		void*const segfault_address = si.si_addr;
		void*const block_to_unlock = merchant->alignToBlockStart(segfault_address);

		// First unlock
		if(inject_syscall_mprotect(pid, injection_site, block_to_unlock, pimpl->block_size,
				PROT_READ|PROT_WRITE|PROT_EXEC))
		{
			std::cerr<<"Unprotect Failed"<<std::endl;
			std::cerr<<"\tInjection site: "<<injection_site<<std::endl;
			std::cerr<<"\tMprotect target: "<<block_to_unlock<<std::endl;
			throw std::runtime_error("Unprotect failed");
		}

		// Copy patches from Merchant to inferior
		using Word = uint64_t;
		constexpr auto word_size = sizeof(Word);

		AbstractElfAccessor::PatchList patches;
		merchant->fetchPatches(block_to_unlock, patches); 

		for(const auto& patch : patches)
		{
			const auto patch_end = patch.start + patch.size;
			assert(patch_end > patch.start);
			assert(patch_end <= pimpl->block_size);

			for(unsigned i=patch.start;i<patch_end;i+=word_size)
			{
				// Copy word from Merchant's copy
				// TODO: this is only necessary for the last word, and only if patch size is not word-aligned
				const Word original = ptrace(PTRACE_PEEKTEXT, pid, (uint8_t*)block_to_unlock + i, nullptr);
				Word word = 0;
				for(unsigned j=0;j<8;j++)
				{
					uint64_t new_byte;
					const uint64_t index = i+j-patch.start;
					if(index < patch.size)
					{
						new_byte = static_cast<unsigned char>(patch.content.at(index));
					}
					else
						new_byte = (uint64_t)(0xFFL)& (original >> (uint64_t)(8)*j);
					assert(new_byte < 0x100L);
					word += (uint64_t)(new_byte) << ((uint64_t)(8)*j);
				}
				/*
				if(original != word)
				{
					std::cout<<"@"<<segfault_address<<std::endl;
					std::cout<<std::hex<<"^"<<original<<" => "<<word<<std::endl;
				}
				*/
				if(ptrace(PTRACE_POKETEXT, pid, (uint8_t*)block_to_unlock + i, reinterpret_cast<void*>(word)))
				{
					throw std::runtime_error("Failed");
				}
			}
		}
	}
	std::cout<<"Child exited succesfully probably"<<std::endl;
}

Agent::Agent(std::shared_ptr<AbstractElfAccessor> m){
	merchant = m;
	pimpl = std::make_unique<Agent::Impl>();

	pimpl->block_size = merchant->getBlockSize();
};
Agent::~Agent() = default;
