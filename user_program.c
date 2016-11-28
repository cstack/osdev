#include "stdlib/stdio.h"
#include "stdlib/syscalls.h"

int fib(unsigned int n) {
  if (n == 0) {
    return 0;
  }

  if (n == 1) {
    return 1;
  }

  return fib(n-1) + fib(n-2);
}

int main() {
  int result = fib(20); // Should be 0x1a6d

  printf("\n----\nHello World!\n----\n");
  printf("fib(20) -> %i\n", result);

  uint32_t num_files = count_files();
  // struct file_t* files = malloc(sizeof(file_t));
  // list_files(files);

  printf("\nFiles: %d\n", num_files);
  // for (uint32_t i = 0; i < num_files; i++) {
  //   file_t* file = files[i];
  //   printf("%s\n", file->name);
  // }

  return result;
}
