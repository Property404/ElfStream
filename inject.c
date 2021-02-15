/*
 * Inject a syscall into a child process
 * Language: C11
 */
#include "inject.h"

#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>

#include <linux/ptrace.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Return number of arguments expected of a particular syscall
static int getNumberOfArguments(int syscall)
{
#define SYSCALL_ARG_INFO(n) {.number_of_arguments=n, .valid=true}
	struct SyscallArgInfo{
		int number_of_arguments;
		bool valid;
	};
	static const struct SyscallArgInfo syscall_arg_map[] = 
	{
		[SYS_exit] = SYSCALL_ARG_INFO(1),
		[SYS_fork] = SYSCALL_ARG_INFO(0),
		[SYS_mprotect] = SYSCALL_ARG_INFO(3),
		[SYS_write] = SYSCALL_ARG_INFO(3),
	};

	if(syscall < 0)
		return -1;
	if((size_t)syscall >= sizeof(syscall_arg_map)/sizeof(syscall_arg_map[0]))
		return -1;
	if(!(syscall_arg_map[syscall].valid))
		return -1;
	return syscall_arg_map[syscall].number_of_arguments;
#undef SYSCALL_ARG_INFO
}

static int inject_syscall(pid_t child, struct InjectionInfo* info, int syscall, ...)
{
	va_list args;
	struct user_regs_struct regs;
	struct user_regs_struct orig_regs;

	if(child <= 0 || syscall < 0 || info == NULL || info->ip==NULL) 
		return -1;

	// Copy current state
	if(0>ptrace(PTRACE_GETREGS, child, NULL, &orig_regs))
		return -1;
	long original_code = ptrace(PTRACE_PEEKTEXT, child, (void*)info->ip, NULL);
	if(NULL == memcpy(&regs, &orig_regs, sizeof(orig_regs)))
		return -1;

	// Inject "syscall" instruction
	regs.rip = (long)info->ip;
	if(ptrace(PTRACE_POKETEXT, child, (void*)regs.rip, (void*)0x050f))
	{
		perror("Poking of text failed");
		fprintf(stderr, "\tInjection site: %p\n", (void*)regs.rip);
		return -1;
	}

	// Set syscall arguments
	va_start(args, syscall);
	regs.rax = syscall;
	int num_args = getNumberOfArguments(syscall);
	if(num_args < 0)
		return -1;
	if(num_args >= 1)
		regs.rdi = va_arg(args, long);
	if(num_args >= 2)
		regs.rsi = va_arg(args, long);
	if(num_args >= 3)
		regs.rdx = va_arg(args, long);
	if(num_args >= 4)
		return -1;
	va_end(args);

	// Replace registers
	if(0>ptrace(PTRACE_SETREGS, child, NULL, &regs))
		return -1;

	// Resume 
	// Stop at entry
	if(0>ptrace(PTRACE_SYSCALL, child, NULL, NULL))
		return -1;
	if(0>waitpid(child, NULL, 0))
		return -1;
	// Stop at exit
	if(0>ptrace(PTRACE_SYSCALL, child, NULL, NULL))
		return -1;
	if(0>waitpid(child, NULL, 0))
		return -1;

	/* Get return value */
	if(0>ptrace(PTRACE_GETREGS, child, NULL, &regs))
		return -1;
	const long return_value = regs.rax;
	if(return_value)
	{
		printf("!!!Return value:%ld\n", return_value);
		if(return_value == syscall)
		{
			printf("!!!Syscall was apparently not executed @%p\n",
					(void*)regs.rip);
		}
	}


	// Restore code
	if(ptrace(PTRACE_POKETEXT, child, (void*)info->ip, (void*)original_code))
		return -1;
	if(original_code != ptrace(PTRACE_PEEKTEXT, child, (void*)info->ip, NULL))
	{
		fprintf(stderr, "Failed to write back");
		return -1;
	}
	// Restore regs
	if(0>ptrace(PTRACE_SETREGS, child, NULL, &orig_regs))
		return -1;

	return return_value;
}


int inject_syscall_mprotect(pid_t child, struct InjectionInfo* info, void* addr, size_t len, int prot)
{
	return inject_syscall(child, info, SYS_mprotect, (long) addr, (long) len, (long) prot);
}

/*
 * References used:
 * https://stackoverflow.com/questions/22444526/x86-64-linux-syscall-arguments
 * https://stackoverflow.com/questions/53024196/can-ptrace-cause-the-traced-process-to-perform-a-syscall-without-access-to-an-ex
 * https://stackoverflow.com/questions/23969524/injected-mprotect-system-call-into-traced-process-fails-with-efault
 */
