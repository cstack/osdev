#include "stdlib/stdio.h"
#include "stdlib/syscalls.h"
#include "stdlib/filesystem.h"

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

  struct file_t files[10];

  uint32_t num_files = count_files();
  printf("\nFiles: %d\n", num_files);
  list_files(files);

  for (uint32_t i = 0; i < num_files; i++) {
    printf("%s\n", files[i].name);
  }

  return result;
}
