#include "interrupts.h"

#include "assembly_interface.h"
#include "drivers/keyboard.h"
#include "drivers/pic.h"
#include "stdio.h"

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
    case(0x00000020):
    case(0x00000021):
      consume_scan_code();
      log("returned from consume_scan_code()\n");
      pic_acknowledge();
      log("returned from pic_acknowledge()\n");
      break;

    default:
      log("Unhandled Interrupt: ");
      print_uint32(LOG, interrupt_number);
      log("\n");
      pic_acknowledge();
      break;
  }
}

void test_interrupt_handler(struct cpu_state cpu, uint32_t interrupt_number, uint32_t error_code) {
  if (cpu.eax) {}; // Avoid unused parameter error

  log("interrupt_handler()\n");

  log("error_code: ");
  print_uint32(LOG, error_code);
  log("\n");

  log("interrupt_number: ");
  print_uint32(LOG, interrupt_number);
  log("\n");

  uint8_t scan_code = inb(0x60);
  outb(0x3F8, scan_code);
  outb(0x20, 0x20);
}
