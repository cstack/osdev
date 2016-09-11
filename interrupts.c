#include "interrupts.h"
#include "stdio.h"

void interrupt_handler() {
  log("interrupt_handler()\n");
  return;
}