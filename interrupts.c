#include "interrupts.h"
#include "stdio.h"

void interrupt_handler(struct cpu_state cpu, uint32_t interrupt_number, uint32_t error_code) {
  log("interrupt_handler()\n");

  log("error_code: ");
  print_uint32(LOG, error_code);
  log("\n");

  log("interrupt_number: ");
  print_uint32(LOG, interrupt_number);
  log("\n");

  log("cpu.eax: ");
  print_uint32(LOG, cpu.eax);
  log("\n");
  log("cpu.ebx: ");
  print_uint32(LOG, cpu.ebx);
  log("\n");
  log("cpu.ecx: ");
  print_uint32(LOG, cpu.ecx);
  log("\n");
  log("cpu.edx: ");
  print_uint32(LOG, cpu.edx);
  log("\n");
  log("cpu.esi: ");
  print_uint32(LOG, cpu.esi);
  log("\n");
  log("cpu.edi: ");
  print_uint32(LOG, cpu.edi);
  log("\n");
  log("cpu.esp: ");
  print_uint32(LOG, cpu.esp);
  log("\n");
  log("cpu.ebp: ");
  print_uint32(LOG, cpu.ebp);
  log("\n");
  return;
}
