#include "interrupts.h"

#include "assembly_interface.h"
#include "data_structures/page_table.h"
#include "data_structures/symbol_table.h"
#include "drivers/keyboard.h"
#include "drivers/pic.h"
#include "stdio.h"

#define INT_KEYBOARD 0x00000009
#define INT_GENERAL_PROTECTION_FAULT 0x0000000D
#define INT_PAGE_FAULT 0x0000000E
#define INT_SOFTWARE 0x00000031

void log_stack_trace_line(uint32_t eip) {
  print_uint32(LOG, eip);
  log(" : ");
  char * symbol_name = address_to_symbol_name(eip);
  fprintf(LOG, symbol_name);
  log("\n");
}

void log_interrupt_details(char* int_name, uint32_t error_code, uint32_t eip, struct cpu_state* cpu) {
  if(cpu->ebp){}
  log("--------------------\n");

  log("Interrupt : ");
  log(int_name);
  log("\n");
  log("error_code: ");
  print_uint32(LOG, error_code);
  log("\n");

  log("\nStack trace:\n");
  eip -= 4; // eip actually points one past the instruction that triggered interrupt
  log_stack_trace_line(eip);
  uint32_t ebp = cpu->ebp;
  while (ebp & 0xC0100000) {
    eip = ((uint32_t*) ebp)[1];
    log_stack_trace_line(eip);

    ebp = *((uint32_t*)ebp);
  }

  log("--------------------\n");
}

void interrupt_handler(struct cpu_state cpu, uint32_t interrupt_number, uint32_t error_code, uint32_t eip) {
  switch(interrupt_number) {
    case(INT_KEYBOARD):
      keyboard_interrupt_handler();
      break;
    case(INT_PAGE_FAULT):
      if ((error_code & 0b1) == 0) {
        // Caused by page-not-present
        page_in((void*) cpu.cr2);
        break;
      }
      log("Interrupt was a page fault. Here's what I know:\n");
      log("- Tried to access virtual address ");
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
      break;

    case(INT_SOFTWARE):
      log_interrupt_details("INT_SOFTWARE", error_code, eip, &cpu);
      break;

    default:
      log("ERROR: Unabled to handle interrupt: ");
      print_uint32(LOG, interrupt_number);
      log("\n");
      break;
  }

  pic_acknowledge();
}
