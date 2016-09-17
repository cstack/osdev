#include "interrupts.h"

#include "assembly_interface.h"
#include "drivers/keyboard.h"
#include "drivers/pic.h"
#include "stdio.h"

void enable_keyboard_interrupts() {
  outb(0x21,0xfd);
  outb(0xa1,0xff);
  enable_hardware_interrupts();
}

void interrupt_handler(struct cpu_state cpu, uint32_t interrupt_number, uint32_t error_code) {
  log("interrupt_handler()\n");

  log("error_code: ");
  print_uint32(LOG, error_code);
  log("\n");

  log("interrupt_number: ");
  print_uint32(LOG, interrupt_number);
  log("\n");

  if (cpu.eax) {}; // Avoid unused parameter error

  switch(interrupt_number) {
    case(0x00000009):
      consume_scan_code();
      pic_acknowledge(interrupt_number);
      break;

    default:
      log("Unhandled Interrupt: ");
      print_uint32(LOG, interrupt_number);
      log("\n");
      break;
  }
}
