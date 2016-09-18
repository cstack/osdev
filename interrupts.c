#include "interrupts.h"

#include "assembly_interface.h"
#include "drivers/keyboard.h"
#include "drivers/pic.h"
#include "stdio.h"

#define INT_KEYBOARD 0x00000009

void interrupt_handler(struct cpu_state cpu, uint32_t interrupt_number, uint32_t error_code) {
  if (cpu.eax) {}; // Avoid unused parameter error
  if (error_code) {
    log("interrupt_handler() error_code ");
    print_uint32(LOG, error_code);
    log("\n");
  }

  switch(interrupt_number) {
    case(INT_KEYBOARD):
      keyboard_interrupt_handler();
      break;

    default:
      log("Unhandled Interrupt: ");
      print_uint32(LOG, interrupt_number);
      log("\n");
      break;
  }

  pic_acknowledge();
}
