#include "interrupts.h"
#include "stdio.h"

void interrupt_handler(struct cpu_state cpu, struct stack_state stack, uint32_t interrupt) {
  log("interrupt_handler()\n");
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

  log("stack.error_code: ");
  print_uint32(LOG, stack.error_code);
  log("\n");
  log("stack.eip: ");
  print_uint32(LOG, stack.eip);
  log("\n");
  log("stack.cs: ");
  print_uint32(LOG, stack.cs);
  log("\n");
  log("stack.eflags: ");
  print_uint32(LOG, stack.eflags);
  log("\n");

  log("interrupt: ");
  print_uint32(LOG, interrupt);
  log("\n");
  return;
}
