#ifndef INCLUDE_LOADER_H
#define INCLUDE_LOADER_H

#include "types.h"

uint32_t KERNEL_VIRTUAL_BASE;

void * kernel_stack_lowest_address;
void * PageDirectoryVirtualAddress;
void * PageDirectoryPhysicalAddress;

#endif /* INCLUDE_LOADER_H */