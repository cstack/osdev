#include "drivers/frame_buffer.h"

void kmain() {
  clear_screen();
  print("Hello, World!");

  // Loop forever
  // TODO: accept user input
  while(1) {};
}