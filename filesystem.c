#include "filesystem.h"

#include "memory.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct file_t* f;

void initialize_filesystem(struct module* mod) {
  void* mod_virtual_start = (void*) (mod->mod_start + UPPER_GB_START);

  char* cursor = (char*) mod_virtual_start;

  if (strncmp(cursor, "FILE ", 5) != 0) {
    fprintf(LOG, "ERROR: First bytes of metadata should be `FILE ` but were `");
    for (int i = 0; i < 5; i++) {
      fprintf(LOG, "%c", cursor[i]);
    }
    fprintf(LOG, "`\n");
    fprintf(LOG, "strncmp returned %i\n", strncmp(cursor, "FILE ", 5));
    fprintf(LOG, "cursor: %s", cursor);
  }
  cursor += 5;

  struct file_t* file = malloc(sizeof(struct file_t));

  uint32_t file_name_length = 0;
  while (*cursor != ' ' && file_name_length < FILE_NAME_MAX_LENGTH) {
    file->name[file_name_length] = *cursor;
    cursor++;
    file_name_length++;
  }
  cursor++;

  uint32_t file_size = atoi(cursor);
  file->size = file_size;
  while (*cursor != '\n') {
    cursor++;
  }
  cursor++;
  file->bytes = cursor;

  f = file;
}

struct file_t* get_file() {
  return f;
}
