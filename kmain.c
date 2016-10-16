#include "assembly_interface.h"
#include "data_structures/global_descriptor_table.h"
#include "data_structures/interrupt_descriptor_table.h"
#include "data_structures/page_table.h"
#include "drivers/frame_buffer.h"
#include "drivers/pic.h"
#include "drivers/serial_port.h"
#include "multiboot_utils.h"
#include "stdio.h"
#include "types.h"

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

void kmain(uint32_t ebx) {
  // // GRUB passes info to the kernel through the ebx register
  // multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
  if(ebx){};

  clear_screen();

  printf(welcome_string);

  serial_init();
  log("\n--------------------\ncstackOS is booting!\n--------------------\n\nInitialized serial port.\n");

  initialize_gdt();
  log("Loaded global descriptor table.\n");

  initialize_idt();
  log("Loaded interrupt descriptor table.\n");

  log("Issuing test interrupt.\n");
  interrupt(49);
  log("Returned from test interrupt()\n");

  pic_init();
  log("Initialized PIC\n");
  while(1){};

  // page_directory_t pd = initialize_page_directory();
  // log("Initialized page directory.\n");
  // log("Address of page directory: ");
  // print_uint32(LOG, (uint32_t) pd);
  // log("\n");

  // page_table_t pt = get_page_table(pd, 0);
  // log("Address of first page table: ");
  // print_uint32(LOG, (uint32_t) pt);
  // log("\n");

  // log("First 5 page table entries: ");
  // for (int i = 0; i < 5; i++) {
  //   print_uint32(LOG, pt[i]);
  //   log("\n");
  // }

  // log("Setting page directory...\n");
  // set_page_directory(pd);
  // log("Set page directory.\n");

  // log("Enabling paging...\n");
  // enable_paging();
  // log("Paging enabled.\n");

  // uint32_t * unmapped_address = (uint32_t *) 0x400000; // 4 MB
  // log("Trying to access address ");
  // print_uint32(LOG, (uint32_t) unmapped_address);
  // log(":\n");
  // print_uint8(LOG, *unmapped_address);
  // log("\n. Successfully accessed.");

  // log("\nMultiboot info passed to kernel from GRUB:\n");
  // print_multiboot_info(LOG, mbinfo);

  // void_function_t start_program = first_module_as_a_function(mbinfo);
  // start_program();
  // log("Got past call to start_program()\n");

  // // Loop forever
  // // User input is accepted asynchronously via interrupts
  // while(1){}
}