#ifndef INCLUDE_INTERRUPTS_H
#define INCLUDE_INTERRUPTS_H

#include "types.h"

struct cpu_state {
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t esi;
  uint32_t edi;
  uint32_t ebp;
} __attribute__((packed));

struct stack_state {
  uint32_t error_code;
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
} __attribute__((packed));

void enable_keyboard_interrupts();
void interrupt_handler(struct cpu_state cpu, uint32_t interrupt_number, uint32_t error_code);
void test_interrupt_handler(struct cpu_state cpu);

#endif /* INCLUDE_INTERRUPTS_H */