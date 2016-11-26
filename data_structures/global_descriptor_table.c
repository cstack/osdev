#include "global_descriptor_table.h"

#include "../assembly_interface.h"
#include "../loader.h"
#include "../stdio.h"
#include "../memory.h"

// A struct describing a Task State Segment.
struct tss_entry_struct
{
  uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
  uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
  uint32_t ss0;        // The stack segment to load when we change to kernel mode.
  uint32_t esp1;       // everything below here is unusued now..
  uint32_t ss1;
  uint32_t esp2;
  uint32_t ss2;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint32_t es;
  uint32_t cs;
  uint32_t ss;
  uint32_t ds;
  uint32_t fs;
  uint32_t gs;
  uint32_t ldt;
  uint16_t trap;
  uint16_t iomap_base;
} __packed;
typedef struct tss_entry_struct tss_entry_t;
tss_entry_t tss;

// As described here: http://wiki.osdev.org/Global_Descriptor_Table#Structure
struct segment_descriptor_t {
  uint16_t limit_0_15; // bits 0-15 of limit
  uint16_t base_0_15;
  uint8_t base_16_23;
  uint8_t access_byte;
  uint8_t flags_and_limit_16_19;
  uint8_t base_24_31;
} __attribute__((packed));

enum segment_selector_t {
  NULL_DESCRIPTOR, // Not used but has to be here
  KERNAL_CODE_SEGMENT_INDEX, // Offset 0x8
  KERNAL_DATA_SEGMENT_INDEX, // Offset 0x10
  USER_CODE_SEGMENT_INDEX, // Offset 0x18
  USER_DATA_SEGMENT_INDEX, // Offset 0x20
  TASK_STATE_SEGMENT_INDEX // Offset 0x28
};

const uint16_t NULL_SEGMENT_SELECTOR = 0x0;
const uint16_t KERNAL_CODE_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * KERNAL_CODE_SEGMENT_INDEX;
const uint16_t KERNAL_DATA_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * KERNAL_DATA_SEGMENT_INDEX;
const uint16_t USER_CODE_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * USER_CODE_SEGMENT_INDEX;
const uint16_t USER_DATA_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * USER_DATA_SEGMENT_INDEX;
const uint16_t TASK_STATE_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * TASK_STATE_SEGMENT_INDEX;

struct segment_descriptor_t gdt[6];

// a pointer to the global descriptor table
// passed by reference to the LGDT instruction
struct gdt_description_structure_t {
  uint16_t size; // in bytes
  uint32_t offset;
} __attribute__((packed)) gdt_description_structure;

void initialize_tss() {
  uint32_t tss_base = (uint32_t) &tss;
  uint32_t tss_limit = sizeof(tss);
  fprintf(LOG, "tss_base: %x\n", tss_base);
  fprintf(LOG, "tss_limit: %x\n", tss_limit);
  gdt[TASK_STATE_SEGMENT_INDEX].limit_0_15 = tss_limit & 0xFFFF;
  gdt[TASK_STATE_SEGMENT_INDEX].base_0_15 = tss_base & 0xFFFF;
  gdt[TASK_STATE_SEGMENT_INDEX].base_16_23 = (tss_base & 0xFF0000) >> 16;
  gdt[TASK_STATE_SEGMENT_INDEX].access_byte = 0b11101001;
  gdt[TASK_STATE_SEGMENT_INDEX].flags_and_limit_16_19 = tss_limit & 0xF0000;
  gdt[TASK_STATE_SEGMENT_INDEX].base_24_31 = (tss_base & 0xFF000000) >> 24;

  tss.ss0 = KERNAL_DATA_SEGMENT_SELECTOR;
  tss.esp0 = ((uint32_t) &kernel_stack_lowest_address + KERNEL_STACK_SIZE) - 4;

  log("KERNEL_STACK_SIZE: %x\n", (uint32_t) KERNEL_STACK_SIZE);

  log("kernel_stack_lowest_address: %x\n", (uint32_t) kernel_stack_lowest_address);

  log("&kernel_stack_lowest_address: %x\n", (uint32_t) (&kernel_stack_lowest_address));

  log("tss.ss0: %x\n", tss.ss0);

  log("tss.esp0: %x\n", tss.esp0);

  tss_flush();
}

void initialize_gdt() {
  gdt_description_structure.size = sizeof(gdt) - 1;
  gdt_description_structure.offset = (uint32_t) gdt;

  gdt[KERNAL_CODE_SEGMENT_INDEX].limit_0_15 = 0xFFFF;
  gdt[KERNAL_CODE_SEGMENT_INDEX].base_0_15 = 0x0000;
  gdt[KERNAL_CODE_SEGMENT_INDEX].base_16_23 = 0x00;
  gdt[KERNAL_CODE_SEGMENT_INDEX].access_byte = 0b10011010;
  gdt[KERNAL_CODE_SEGMENT_INDEX].flags_and_limit_16_19 = 0xCF;
  gdt[KERNAL_CODE_SEGMENT_INDEX].base_24_31 = 0x00;

  gdt[KERNAL_DATA_SEGMENT_INDEX].limit_0_15 = 0xFFFF;
  gdt[KERNAL_DATA_SEGMENT_INDEX].base_0_15 = 0x0000;
  gdt[KERNAL_DATA_SEGMENT_INDEX].base_16_23 = 0x00;
  gdt[KERNAL_DATA_SEGMENT_INDEX].access_byte = 0b10010010;
  gdt[KERNAL_DATA_SEGMENT_INDEX].flags_and_limit_16_19 = 0xCF;
  gdt[KERNAL_DATA_SEGMENT_INDEX].base_24_31 = 0x00;

  gdt[USER_CODE_SEGMENT_INDEX].limit_0_15 = 0xFFFF;
  gdt[USER_CODE_SEGMENT_INDEX].base_0_15 = 0x0000;
  gdt[USER_CODE_SEGMENT_INDEX].base_16_23 = 0x00;
  gdt[USER_CODE_SEGMENT_INDEX].access_byte = 0b11111010;
  gdt[USER_CODE_SEGMENT_INDEX].flags_and_limit_16_19 = 0xCF;
  gdt[USER_CODE_SEGMENT_INDEX].base_24_31 = 0x00;

  gdt[USER_DATA_SEGMENT_INDEX].limit_0_15 = 0xFFFF;
  gdt[USER_DATA_SEGMENT_INDEX].base_0_15 = 0x0000;
  gdt[USER_DATA_SEGMENT_INDEX].base_16_23 = 0x00;
  gdt[USER_DATA_SEGMENT_INDEX].access_byte = 0b11110010;
  gdt[USER_DATA_SEGMENT_INDEX].flags_and_limit_16_19 = 0xCF;
  gdt[USER_DATA_SEGMENT_INDEX].base_24_31 = 0x00;

  asm_lgdt(&gdt_description_structure);

  // Grub has already loaded the segment registers
  // with the correct values (0x8 for cs, 0x10 for the others)
}