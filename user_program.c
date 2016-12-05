#include "stdlib/filesystem.h"
#include "stdlib/stdio.h"
#include "stdlib/string.h"
#include "stdlib/syscalls.h"

void print_prompt() {
  printf("> ");
}

void handle_input(char* input) {
  if (strcmp(input, "ls") == 0) {
    struct file_t files[10];
    uint32_t num_files = count_files();
    list_files(files);
    for (uint32_t i = 0; i < num_files; i++) {
      printf("%s\n", files[i].name);
    }
  } else {
    printf("Unknown command `%s`\n", input);
  }
  printf("> ");
}

int main() {
  print_prompt();
  register_input_handler(handle_input);
  while(1){}

  return 0;
}
