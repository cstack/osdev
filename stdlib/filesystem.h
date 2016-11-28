#ifndef INCLUDE_FILESYSTEM_H
#define INCLUDE_FILESYSTEM_H

#define FILE_NAME_MAX_LENGTH 63
struct file_t {
  char name[FILE_NAME_MAX_LENGTH+1];
  uint32_t size;
  char* bytes;
  struct file_t* next_sibling;
};

#endif /* INCLUDE_FILESYSTEM_H */