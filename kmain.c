#include "assembly_interface.h"
#include "data_structures/global_descriptor_table.h"
#include "data_structures/interrupt_descriptor_table.h"
#include "data_structures/page_table.h"
#include "data_structures/symbol_table.h"
#include "drivers/frame_buffer.h"
#include "drivers/pic.h"
#include "drivers/serial_port.h"
#include "multiboot_utils.h"
#include "stdio.h"
#include "stdlib.h"
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

void trigger_page_fault() {
  uint32_t * unmapped_address = (uint32_t *) 0x400000; // 4 MB
  log("Trying to access address ");
  print_uint32(LOG, (uint32_t) unmapped_address);
  log(":\n");
  print_uint8(LOG, *unmapped_address);
  log("\n. Successfully accessed.\n");
}

struct test_struct_t {
  uint32_t integer;
  char character;
  char* string;
};

void kmain(struct kernel_memory_descriptor_t kernel_memory, uint32_t ebx) {
  // GRUB passes info to the kernel through the ebx register
  multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;

  clear_screen();

  printf(welcome_string);

  serial_init();
  log("\n--------------------\ncstackOS is booting!\n--------------------\n\nInitialized serial port.\n");

  log("\nMultiboot info passed to kernel from GRUB:\n");
  print_multiboot_info(LOG, mbinfo);
  log("\n");

  if (load_symbol_table(get_elf_section(mbinfo, ".symtab"), get_elf_section(mbinfo, ".strtab"))) {
    log("Initialized symbol table.\n");
  } else {
    log("ERROR: Could not initialize symbol table.\n");
  }

  initialize_gdt();
  log("Loaded global descriptor table.\n");

  initialize_idt();
  log("Loaded interrupt descriptor table.\n");

  log("Issuing test interrupt.\n");
  interrupt(49);
  log("Returned from test interrupt()\n");

  pic_init();
  log("Initialized PIC\n");

  uint32_t free_pages = initialize_page_allocator(kernel_memory, mbinfo);
  log("Initialized page allocator.\n");
  print_uint32(LOG, free_pages);
  log(" free pages (");
  print_uint32(LOG, free_pages / 256);
  log(" MB)\n");

  page_directory_t pd = initialize_page_directory();
  log("Initialized page directory.\n");
  log("Address of page directory: ");
  print_uint32(LOG, (uint32_t) pd);
  log("\n");

  print_page_directory(LOG, pd);

  trigger_page_fault();

  struct test_struct_t* test = (struct test_struct_t*) malloc(sizeof(struct test_struct_t));

  log("Dynamically allocated a struct\nAddress is: ");
  print_uint32(LOG, (uint32_t) test);
  log("\n");

  log("test->integer started as: ");
  print_uint32(LOG, (uint32_t) test->integer);
  log("\n");

  test->integer = 10;

  log("now test->integer is: ");
  print_uint32(LOG, (uint32_t) test->integer);
  log("\n");

  test->character = 'A';
  test->string = "Hello World";

  // void_function_t start_program = first_module_as_a_function(mbinfo);
  // start_program();
  // log("Got past call to start_program()\n");

  // Loop forever
  // User input is accepted asynchronously via interrupts
  while(1){}
}