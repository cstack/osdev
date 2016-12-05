#ifndef INCLUDE_SYSCALLS_H
#define INCLUDE_SYSCALLS_H

#include "types.h"
#include "filesystem.h"

void write_to_screen(char* s);
uint32_t count_files();
void list_files(struct file_t buffer[]);

typedef void (*InputHandler)(char*);
void register_input_handler(InputHandler handler);

#endif /* INCLUDE_SYSCALLS_H */