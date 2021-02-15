#pragma once
#include <unistd.h>

#ifdef __cplusplus
extern "C"{
#endif

/* Inject syscalls into a child process */
int inject_syscall_mprotect
(
	pid_t child,
	const void* site,
	void* addr,
	size_t len,
	int prot
);

#ifdef __cplusplus
}
#endif
