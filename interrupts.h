#ifndef INCLUDE_INTERRUPTS_H
#define INCLUDE_INTERRUPTS_H

#include "types.h"

#define INT_KEYBOARD 0x00000009
#define INT_GENERAL_PROTECTION_FAULT 0x0000000D
#define INT_PAGE_FAULT 0x0000000E
#define INT_SOFTWARE 0x00000031
#define INT_OUT_OF_MEMORY 0x00000032
#define INT_SYSCALL 0x00000080

struct cpu_state {
  uint32_t cr2;
  uint32_t ebp;
  uint32_t edi;
  uint32_t esi;
  uint32_t edx;
  uint32_t ecx;
  uint32_t ebx;
  uint32_t eax;
} __attribute__((packed));

void enable_keyboard_interrupts();
uint32_t interrupt_handler(struct cpu_state cpu, uint32_t interrupt_number, uint32_t error_code, uint32_t eip);

#endif /* INCLUDE_INTERRUPTS_H */