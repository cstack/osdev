#ifndef INCLUDE_SYSCALLS_H
#define INCLUDE_SYSCALLS_H

#include "types.h"
#include "filesystem.h"

void write_to_screen(char* s);
uint32_t count_files();
void list_files(struct file_t buffer[]);

#endif /* INCLUDE_SYSCALLS_H */