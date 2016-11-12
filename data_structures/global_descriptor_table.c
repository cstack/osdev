#include "global_descriptor_table.h"

#include "../assembly_interface.h"

// As described here: http://wiki.osdev.org/Global_Descriptor_Table#Structure
struct segment_descriptor_t {
  uint16_t limit_0_15; // bits 0-15 of limit
  uint16_t base_0_15;
  uint8_t base_16_23;
  uint8_t access_byte;
  uint8_t flags_and_limit_16_19;
  uint8_t base_24_31;
} __attribute__((packed));

const uint16_t NULL_SEGMENT_SELECTOR = 0x0;
const uint16_t KERNAL_CODE_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * 1;
const uint16_t KERNAL_DATA_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * 2;
const uint16_t USER_CODE_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * 3;
const uint16_t USER_DATA_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * 4;

struct segment_descriptor_t gdt[5];

enum segment_selector_t {
  NULL_DESCRIPTOR, // Not used but has to be here
  KERNAL_CODE_SEGMENT_INDEX, // Offset 0x8
  KERNAL_DATA_SEGMENT_INDEX, // Offset 0x10
  USER_CODE_SEGMENT_INDEX, // Offset 0x18
  USER_DATA_SEGMENT_INDEX // Offset 0x20
};

// a pointer to the global descriptor table
// passed by reference to the LGDT instruction
struct gdt_description_structure_t {
  uint16_t size; // in bytes
  uint32_t offset;
} __attribute__((packed)) gdt_description_structure;

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