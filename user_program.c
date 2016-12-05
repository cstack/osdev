#include "stdlib/filesystem.h"
#include "stdlib/stdio.h"
#include "stdlib/string.h"
#include "stdlib/syscalls.h"

void print_prompt() {
  printf("> ");
}

void handle_ls(char* input) {
  bool flag_long = false;
  char* flag = input + 2;
  while (*flag == ' ') {
    flag++;
  }
  if (*flag != 0) {
    if (strncmp(flag, "-l", 2) == 0) {
      flag_long = true;
    } else {
      printf("Unrecognized flag `%s`\n", flag);
      return;
    }
  }
  struct file_t files[10];
  uint32_t num_files = count_files();
  list_files(files);
  for (uint32_t i = 0; i < num_files; i++) {
    if (flag_long) {
      printf("%s (%d bytes)\n", files[i].name, files[i].size);
    } else {
      printf("%s\n", files[i].name);
    }
  }
}

void handle_input(char* input) {
  if (strncmp(input, "ls", 2) == 0) {
    handle_ls(input);
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
