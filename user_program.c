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
  if (1) {
    return fib(20); // Should return 0x1a6d
  }
}
