#include "stdlib/assembly_functions.h"

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

  write_to_screen("\n----\nHello World!\n----\n");

  return result;
}
