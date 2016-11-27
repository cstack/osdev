#ifndef INCLUDE_FILESYSTEM_H
#define INCLUDE_FILESYSTEM_H

#include "types.h"
#include "multiboot.h"

#define FILE_NAME_MAX_LENGTH 63
struct file_t {
  char name[FILE_NAME_MAX_LENGTH+1];
  uint32_t size;
  char* bytes;
  struct file_t* next_sibling;
};

void initialize_filesystem(struct module* mbinfo);
struct file_t* get_file();

#endif /* INCLUDE_FILESYSTEM_H */