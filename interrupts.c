#include "interrupts.h"

#include "assembly_interface.h"
#include "drivers/keyboard.h"
#include "drivers/pic.h"
#include "stdio.h"

#define INT_KEYBOARD 0x00000009
#define INT_GENERAL_PROTECTION_FAULT 0x0000000D
#define INT_PAGE_FAULT 0x0000000E

void interrupt_handler(struct cpu_state cpu, uint32_t interrupt_number, uint32_t error_code) {
  if (error_code) {
    log("interrupt_handler() error_code ");
    print_uint32(LOG, error_code);
    log("\n");
  }

  switch(interrupt_number) {
    case(INT_KEYBOARD):
      keyboard_interrupt_handler();
      break;
    case(INT_PAGE_FAULT):
      log("\nPage fault! Here's what I know:\n");
      log("- Tried to access linear address ");
      print_uint32(LOG, cpu.cr2);
      log("\n");
      if (error_code & 0b1) {
        log("- Couldn't complete because of page-protection violation\n");
      } else {
        log("- Couldn't complete because page was not present\n");
      }
      if (error_code & 0b10) {
        log("- This was an attempt to WRITE to this address.\n");
      } else {
        log("- This was an attempt to READ from this address.\n");
      }
      if (error_code & 0b100) {
        log("- Memory access came from user.\n");
      } else {
        log("- Memory access came from kernel.\n");
      }
      if (error_code & 0b1000) {
        log("- caused by reading a 1 in a reserved field.\n");
      }
      if (error_code & 0b10000) {
        log("- caused by an instruction fetch.\n");
      }
      while(1){};
      break;

    default:
      log("Unhandled Interrupt: ");
      print_uint32(LOG, interrupt_number);
      log("\n");
      break;
  }

  pic_acknowledge();
}
