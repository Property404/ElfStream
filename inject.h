#pragma once
#include <unistd.h>

#ifdef __cplusplus
extern "C"{
#endif

struct InjectionInfo
{
	// instruction pointer
	void* ip;
};

/* Inject syscalls into a child process */
int inject_syscall_mprotect
(
	pid_t child,
	struct InjectionInfo* info,
	void* addr,
	size_t len,
	int prot
);

#ifdef __cplusplus
}
#endif
