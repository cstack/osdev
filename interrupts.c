#include "interrupts.h"

#include "assembly_interface.h"
#include "data_structures/page_table.h"
#include "data_structures/symbol_table.h"
#include "drivers/keyboard.h"
#include "drivers/pic.h"
#include "loader.h"
#include "memory.h"
#include "stdio.h"

void log_stack_trace_line(uint32_t eip) {
  char * symbol_name = address_to_symbol_name(eip);
  fprintf(LOG, "%x : %s\n", eip, symbol_name);
}

void log_interrupt_details(char* int_name, uint32_t error_code, uint32_t eip, struct cpu_state* cpu) {
  if(cpu->ebp){}
  fprintf(LOG, "--------------------\n");

  fprintf(LOG, "Interrupt: %s\n", int_name);
  fprintf(LOG, "error_code: %x\n", error_code);

  fprintf(LOG, "\nStack trace:\n");
  eip -= 4; // eip actually points one past the instruction that triggered interrupt
  log_stack_trace_line(eip);
  uint32_t ebp = cpu->ebp;
  uint32_t kernel_stack_highest_address = ((uint32_t) &kernel_stack_lowest_address + KERNEL_STACK_SIZE - 4);
  while (ebp <= kernel_stack_highest_address && ebp >= ((uint32_t) &kernel_stack_lowest_address)) {
    eip = ((uint32_t*) ebp)[1];
    log_stack_trace_line(eip);

    ebp = *((uint32_t*)ebp);
  }

  fprintf(LOG, "--------------------\n");
}

void sys_write_to_screen(char* s) {
  fprintf(LOG, "sys_write_to_screen(%x)\n", (uint32_t) s);
  printf(s);
}

void handle_syscall(struct cpu_state* cpu) {
  uint32_t syscall_num = cpu->eax;

  switch (syscall_num) {
    case (1):
      sys_write_to_screen((char*) cpu->ebx);
      break;
    default:
      fprintf(LOG, "Unknown syscall: %x\n", syscall_num);
      while(1){}
  }
}

void interrupt_handler(struct cpu_state cpu, uint32_t interrupt_number, uint32_t error_code, uint32_t eip) {
  switch(interrupt_number) {
    case(INT_KEYBOARD):
      keyboard_interrupt_handler();
      break;
    case(INT_PAGE_FAULT):
      log_interrupt_details("INT_PAGE_FAULT", error_code, eip, &cpu);
      if ((error_code & 0b1) == 0) {
        // Caused by page-not-present
        page_in((void*) cpu.cr2);
        break;
      }
      fprintf(LOG, "Interrupt was a page fault. Here's what I know:\n");
      fprintf(LOG, "- Tried to access virtual address %x\n", cpu.cr2);
      if (error_code & 0b1) {
        fprintf(LOG, "- Couldn't complete because of page-protection violation\n");
      } else {
        fprintf(LOG, "- Couldn't complete because page was not present\n");
      }
      if (error_code & 0b10) {
        fprintf(LOG, "- This was an attempt to WRITE to this address.\n");
      } else {
        fprintf(LOG, "- This was an attempt to READ from this address.\n");
      }
      if (error_code & 0b100) {
        fprintf(LOG, "- Memory access came from user.\n");
      } else {
        fprintf(LOG, "- Memory access came from kernel.\n");
      }
      if (error_code & 0b1000) {
        fprintf(LOG, "- caused by reading a 1 in a reserved field.\n");
      }
      if (error_code & 0b10000) {
        fprintf(LOG, "- caused by an instruction fetch.\n");
      }
      break;

    case(INT_SOFTWARE):
      log_interrupt_details("INT_SOFTWARE", error_code, eip, &cpu);
      break;

    case(INT_SYSCALL):
      log_interrupt_details("INT_SYSCALL", error_code, eip, &cpu);
      handle_syscall(&cpu);
      break;

    case(INT_OUT_OF_MEMORY):
      log_interrupt_details("INT_OUT_OF_MEMORY", error_code, eip, &cpu);
      while(true){}
      break;

    default:
      fprintf(LOG, "ERROR: Unable to handle interrupt: %x\n", interrupt_number);
      log_interrupt_details("INT_UNKNOWN", error_code, eip, &cpu);
      while(1){}
      break;
  }

  pic_acknowledge();
}
