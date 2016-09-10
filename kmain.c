#include "drivers/frame_buffer.h"
#include "drivers/serial_port.h"
#include "assembly_interface.h"
#include "types.h"

enum output_t {SCREEN, LOG};

// The global descriptor table
// describes memory segements and their
// privelage levels
struct gdt_t {
  int32 address;
  int16 size;
} __attribute__((packed)) gdt;

// See http://wiki.osdev.org/Global_Descriptor_Table#Structure
struct segment_descriptor_t {
  int16 base_0_15; // bits 0-15 of base
  int16 limit_0_15;
  int8 base_24_31;
  int8 flags_and_limit_16_19;
  int8 access_byte;
  int8 base_16_23;
} __attribute__((packed));

enum segment_selector_t {
  NULL_DESCRIPTOR, // Not but has to be here
  KERNAL_CODE_SEGMENT, // Offset 0x8
  KERNAL_DATA_SEGMENT // Offset 0x10
};
struct segment_descriptor_t segment_descriptors[3];

void initialize_gdt() {
  gdt.address = (int16) segment_descriptors;
  gdt.size = sizeof(segment_descriptors);

  segment_descriptors[KERNAL_CODE_SEGMENT].base_0_15 = 0x0;
  segment_descriptors[KERNAL_CODE_SEGMENT].base_24_31 = 0x0;
  segment_descriptors[KERNAL_CODE_SEGMENT].limit_0_15 = 0xFFFF;
  segment_descriptors[KERNAL_CODE_SEGMENT].flags_and_limit_16_19 = 0b1000 << 4;
  segment_descriptors[KERNAL_CODE_SEGMENT].flags_and_limit_16_19 &= 0xF;
  segment_descriptors[KERNAL_CODE_SEGMENT].access_byte &= 0b10010110;

  segment_descriptors[KERNAL_DATA_SEGMENT].base_0_15 = 0x0;
  segment_descriptors[KERNAL_DATA_SEGMENT].base_24_31 = 0x0;
  segment_descriptors[KERNAL_DATA_SEGMENT].limit_0_15 = 0xFFFF;
  segment_descriptors[KERNAL_DATA_SEGMENT].flags_and_limit_16_19 = 0b1000 << 4;
  segment_descriptors[KERNAL_DATA_SEGMENT].flags_and_limit_16_19 &= 0xF;
  segment_descriptors[KERNAL_DATA_SEGMENT].access_byte &= 0b10010010;
  lgdt(&gdt);

  // Grub has already loaded the segment registers
  // with the correct values (0x8 for cs, 0x10 for the others)
}

void write(enum output_t output_device, char * s) {
  switch (output_device) {
    case (SCREEN):
      fb_write(s);
      break;
    case (LOG):
      serial_write(SERIAL_COM1_BASE, s);
      break;
  }
}

void printf(char * s) {
  write(SCREEN, s);
}

void log(char * s) {
  write(LOG, s);
}

static char *welcome_string = ""
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                              | |                  | |                          "
"                   ___   ___  | |_    __ _    ___  | | __                       "
"                  / __| / __| | __|  / _` |  / __| | |/ /                       "
"                 | (__  \\__ \\ | |_  | (_| | | (__  |   <                        "
"                  \\___| |___/  \\__|  \\__,_|  \\___| |_|\\_\\                       "
"                                                                                "
"                                                                                "
"                             ____     _____                                     "
"                            / __ \\   / ____|                                    "
"                           | |  | | | (___                                      "
"                           | |  | |  \\___ \\                                     "
"                           | |__| |  ____) |                                    "
"                            \\____/  |_____/                                     "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"";

void kmain() {
  clear_screen();

  printf(welcome_string);

  serial_init(SERIAL_COM1_BASE);
  log("Initialized serial port.\n");

  initialize_gdt();
  log("Loaded global descriptor table.\n");

  move_cursor(17, 0);

  // Loop forever
  // TODO: accept user input
  while(1) {};
}