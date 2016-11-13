#include "assembly_interface.h"
#include "data_structures/global_descriptor_table.h"
#include "data_structures/interrupt_descriptor_table.h"
#include "data_structures/page_table.h"
#include "data_structures/symbol_table.h"
#include "drivers/frame_buffer.h"
#include "drivers/pic.h"
#include "drivers/serial_port.h"
#include "multiboot_utils.h"
#include "process.h"
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

uint32_t trigger_page_fault() {
  uint32_t* unmapped_address = (uint32_t *) 0x400000; // 4 MB
  return *unmapped_address;
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
  log("\n--------------------\ncstackOS is booting!\n--------------------\n\n");

  #ifdef DEBUG
  log("\nMultiboot info passed to kernel from GRUB:\n");
  print_multiboot_info(LOG, mbinfo);
  log("\n");
  #endif

  log("- Initializing symbol table...\n");
  if (load_symbol_table(get_elf_section(mbinfo, ".symtab"), get_elf_section(mbinfo, ".strtab"))) {
    log("  - done\n");
  } else {
    log("ERROR: Could not initialize symbol table.\n");
    while(1){}
  }

  log("- Initializing global descriptor table...\n");
  initialize_gdt();
  log("  - done\n");

  log("- Initializing interrupt descriptor table...\n");
  initialize_idt();
  log("  - done\n");

  log("- Issuing test interrupt...\n");
  interrupt();
  log("  - done\n");

  log("- Initializing programable interrupt controller...\n");
  pic_init();
  log("  - done\n");

  log("- Initializing page allocator...\n");
  uint32_t free_pages = initialize_page_allocator(kernel_memory, mbinfo);
  log("  - done\n");
  log("  - ");
  print_uint32(LOG, free_pages);
  log(" free pages (");
  print_uint32(LOG, free_pages / 256);
  log(" MB)\n");

  log("- Initializing page directory...\n");
  page_directory_t pd = initialize_kernel_page_directory();
  log("  - done\n");
  log("  - Address of page directory: ");
  print_uint32(LOG, (uint32_t) pd);
  log("\n");
  uint32_t num_pages = num_present_pages(pd);
  log("  - ");
  print_uint32(LOG, num_pages);
  log(" present pages\n");

  log("- Triggering a page fault...\n");
  uint32_t value = trigger_page_fault();
  log("  - done\n");
  log("  - value at unmapped address was ");
  print_uint32(LOG, value);
  log("\n");

  log("- Dynamically allocating a struct...\n");
  struct test_struct_t* test = (struct test_struct_t*) malloc(sizeof(struct test_struct_t));
  log("  - done\n");
  log("  - address is: ");
  print_uint32(LOG, (uint32_t) test);
  log("\n");
  
  uint32_t local = 0xDEADBEEF;

  log("  - local variable at: ");
  print_uint32(LOG, (uint32_t) &local);
  log("\n");
  log("  - local variable value: ");
  print_uint32(LOG, (uint32_t) local);
  log("\n");

  void* sp = current_stack_pointer();
  log("  - stack pointer is: ");
  print_uint32(LOG, (uint32_t) sp);
  log("\n");

  log("- Creating a user process...\n");
  create_process();
  log("  - done\n");

  // void_function_t start_program = first_module_as_a_function(mbinfo);
  // start_program();
  // log("Got past call to start_program()\n");

  // Loop forever
  // User input is accepted asynchronously via interrupts
  while(1){}
}