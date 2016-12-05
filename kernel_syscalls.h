#ifndef INCLUDE_KERNEL_SYSCALLS_H
#define INCLUDE_KERNEL_SYSCALLS_H

#include "interrupts.h"

uint32_t handle_syscall(struct cpu_state* cpu);

#endif /* INCLUDE_KERNEL_SYSCALLS_H */