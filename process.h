#ifndef INCLUDE_PROCESS_H
#define INCLUDE_PROCESS_H

#include "multiboot.h"
#include "types.h"
#include "kernel_filesystem.h"

void create_process(struct file_t* file);

#endif /* INCLUDE_PROCESS_H */