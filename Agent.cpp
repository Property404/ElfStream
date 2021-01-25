#include "Agent.h"
#include "inject.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/mman.h>

#include <cassert>
#include <iostream>
#include <stdexcept>

struct Agent::Impl
{
	pid_t pid;
	size_t block_size;
};

void Agent::spawn(const std::string& path)
{
	const pid_t pid = fork();
	if(!pid)
	{
		if(ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0)
			throw("ptrace(PTRACE_TRACEME...) failed");

		execl(path.c_str(), "PROGRAM_NAME", nullptr);
	}
	this->pimpl->pid = pid;
}

void Agent::run()
{
	const auto pid = this->pimpl->pid;
	int wait_status{};
	waitpid(pid, &wait_status, 0);

	// Protect region
	const auto region_base = merchant->memoryStart();
	const auto region_size = merchant->memorySize();
	std::cout<<"Injecting mprotect("<<region_base<<", "<<region_size<<")"<<std::endl;
	InjectionInfo injection_info{};
	if(0>inject_syscall_mprotect(pid, &injection_info, region_base, region_size, PROT_NONE))
		std::cerr<<"Warning: inject_syscall_mprotect() returned error"<<std::endl;
	std::cout<<"Mprotect Injected"<<std::endl;

	while(WIFSTOPPED(wait_status))
	{
		// Continue on our way
		if(ptrace(PTRACE_CONT, pid, 0, 0) < 0)
			throw std::runtime_error("ptrace(PTRACE_CONT...) failed");

		// Wait for sigfault or somethign
		waitpid(pid, &wait_status, 0);

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
		std::cout<<"Unlocking memory:"<<block_to_unlock<<std::endl;
		if(inject_syscall_mprotect(pid, &injection_info, block_to_unlock, pimpl->block_size,
				PROT_READ|PROT_WRITE|PROT_EXEC) < 0)
			throw std::runtime_error("Unprotect failed");

		// Temporary gate - TODO: remove
		if(uintptr_t(block_to_unlock) >= 0x404000)
			continue;

		// Copy from Merchant to inferior
		using Word = uint64_t;
		constexpr auto word_size = sizeof(Word);
		const auto correct_code = merchant->fetchBlockOf(block_to_unlock); 
		for(unsigned i=0;i<pimpl->block_size/word_size;i+=word_size)
		{
			const void*const vmem_location = (uint8_t*) block_to_unlock + i;

			// Copy word from Merchant's copy
			Word word = 0;
			for(unsigned j=0;j<8;j++)
			{
				const uint64_t index = i+j;
				const uint64_t new_byte = static_cast<unsigned char>(correct_code[index]);
				assert(new_byte < 0x100L);
				word += (uint64_t)(new_byte) << ((uint64_t)(8)*index);
			}

			if(ptrace(PTRACE_POKETEXT, pid, (uint8_t*)block_to_unlock + i, reinterpret_cast<void*>(word)))
			{
				throw std::runtime_error("Failed");
			}
		}
	}
	std::cout<<"Child exited succesfully probably"<<std::endl;
}

Agent::Agent(std::shared_ptr<Merchant> m){
	merchant = m;
	pimpl = std::make_unique<Agent::Impl>();

	pimpl->block_size = merchant->getBlockSize();
};
Agent::~Agent() = default;
