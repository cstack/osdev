#include "filesystem.h"

#include "memory.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct file_t* first_file;
struct file_t* last_file;

struct file_t* parse_file(char** cursor) {
  if (strncmp(*cursor, "FILE ", 5) != 0) {
    fprintf(LOG, "ERROR: First bytes of metadata should be `FILE ` but were `");
    for (int i = 0; i < 5; i++) {
      fprintf(LOG, "%c", (*cursor)[i]);
    }
    fprintf(LOG, "`\n");
    fprintf(LOG, "strncmp returned %i\n", strncmp(*cursor, "FILE ", 5));
    fprintf(LOG, "cursor: %s", cursor);
    while(1){}
  }
  (*cursor) += 5;

  struct file_t* file = malloc(sizeof(struct file_t));

  uint32_t file_name_length = 0;
  while (*(*cursor) != ' ' && file_name_length < FILE_NAME_MAX_LENGTH) {
    file->name[file_name_length] = *(*cursor);
    (*cursor)++;
    file_name_length++;
  }
  (*cursor)++;

  uint32_t file_size = atoi(*cursor);
  file->size = file_size;
  while (*(*cursor) != '\n') {
    (*cursor)++;
  }
  (*cursor)++;
  file->bytes = (*cursor);

  (*cursor) += file->size + 1;

  return file;
}

void initialize_filesystem(struct module* mod) {
  void* mod_virtual_start = (void*) (mod->mod_start + UPPER_GB_START);
  char* mod_virtual_end = (mod->mod_end + UPPER_GB_START);
  char* cursor = (char*) mod_virtual_start;

  while (cursor < mod_virtual_end) {
    struct file_t* new_file = parse_file(&cursor);
    if (!first_file) {
      first_file = new_file;
    }
    if (last_file) {
      last_file->next_sibling = new_file;
    } else {
      last_file = new_file;
    }
  }
}

struct file_t* get_file() {
  return first_file;
}
