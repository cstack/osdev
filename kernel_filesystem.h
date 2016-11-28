#ifndef INCLUDE_KERNEL_FILESYSTEM_H
#define INCLUDE_KERNEL_FILESYSTEM_H

#include "types.h"
#include "multiboot.h"
#include "stdlib/filesystem.h"

void initialize_filesystem(struct module* mbinfo);
struct file_t* get_file(char* name);
struct file_t* get_first_file();

#endif /* INCLUDE_KERNEL_FILESYSTEM_H */