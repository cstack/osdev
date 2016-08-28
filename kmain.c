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

struct segment_selector {
  int16 address;
  int16 size;
} __attribute__((packed));

int16 segment_selectors[] = {
  0x0, // null descriptor
  (0x08 << 4), // kernel code segment
  (0x10 << 4), // kernel data segment
};

void initialize_gdt() {
  gdt.address = (int16) segment_selectors;
  gdt.size = sizeof(segment_selectors);
  lgdt(&gdt);
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