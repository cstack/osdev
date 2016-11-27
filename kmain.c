#include "assembly_interface.h"
#include "data_structures/global_descriptor_table.h"
#include "data_structures/interrupt_descriptor_table.h"
#include "data_structures/page_table.h"
#include "data_structures/symbol_table.h"
#include "drivers/frame_buffer.h"
#include "drivers/pic.h"
#include "drivers/serial_port.h"
#include "filesystem.h"
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
  fprintf(LOG, "\n--------------------\ncstackOS is booting!\n--------------------\n\n");

  #ifdef DEBUG
  fprintf(LOG, "\nMultiboot info passed to kernel from GRUB:\n");
  print_multiboot_info(LOG, mbinfo);
  fprintf(LOG, "\n");
  #endif

  fprintf(LOG, "- Initializing symbol table...\n");
  if (load_symbol_table(get_elf_section(mbinfo, ".symtab"), get_elf_section(mbinfo, ".strtab"))) {
    fprintf(LOG, "  - done\n");
  } else {
    fprintf(LOG, "ERROR: Could not initialize symbol table.\n");
    while(1){}
  }

  fprintf(LOG, "- Initializing global descriptor table...\n");
  initialize_gdt();
  fprintf(LOG, "  - done\n");

  fprintf(LOG, "- Initializing interrupt descriptor table...\n");
  initialize_idt();
  fprintf(LOG, "  - done\n");

  fprintf(LOG, "- Issuing test interrupt...\n");
  interrupt();
  fprintf(LOG, "  - done\n");

  fprintf(LOG, "- Initializing programable interrupt controller...\n");
  pic_init();
  fprintf(LOG, "  - done\n");

  fprintf(LOG, "- Initializing page allocator...\n");
  uint32_t free_pages = initialize_page_allocator(kernel_memory, mbinfo);
  fprintf(LOG, "  - done\n");
  fprintf(LOG, "  - %i free pages (%i MB)\n", free_pages, free_pages/256);

  fprintf(LOG, "- Initializing page directory...\n");
  page_directory_t pd = initialize_kernel_page_directory();
  fprintf(LOG, "  - done\n");
  fprintf(LOG, "  - Address of page directory: %x\n", (uint32_t) pd);
  uint32_t num_pages = num_present_pages(pd);
  fprintf(LOG, "  - %i present pages\n", num_pages);

  fprintf(LOG, "- Triggering a page fault...\n");
  uint32_t value = trigger_page_fault();
  fprintf(LOG, "  - done\n");
  fprintf(LOG, "  - value at unmapped address was %x\n", value);

  fprintf(LOG, "- Initializing task state segment...\n");
  initialize_tss();
  fprintf(LOG, "  - done\n");

  fprintf(LOG, "- Dynamically allocating a struct...\n");
  struct test_struct_t* test = (struct test_struct_t*) malloc(sizeof(struct test_struct_t));
  fprintf(LOG, "  - done\n");
  fprintf(LOG, "  - address (on heap) is: %x\n", (uint32_t) test);
  
  uint32_t local = 0xDEADBEEF;

  fprintf(LOG, "  - local variable (on stack) at: %x\n", (uint32_t) &local);

  void* sp = current_stack_pointer();
  fprintf(LOG, "  - stack pointer is: %x\n", (uint32_t) sp);

  fprintf(LOG, "- Testing format strings...\n");
  fprintf(
    LOG,
    "  - %%s -> %s, %%c -> %c, %%x -> %x, %%X -> %X, %%u -> %u, %%i -> %i, %%o -> %o\n",
    "test", 'A', 0xDEADBEEF, 0xDEADBEEF, 12345, -12345, 12345
  );

  fprintf(LOG, "- Loading file system from GRUB module...\n");
  initialize_filesystem(first_module(mbinfo));
  fprintf(LOG, "  - done\n");

  struct file_t* file = get_file();
  fprintf(LOG, "Name of file: %s\n", file->name);
  fprintf(LOG, "Size of file: %i\n", file->size);
  fprintf(LOG, "Contents of file:\n----\n%s\n----\n", file->bytes);

  // fprintf(LOG, "- Creating a user process...\n");
  // create_process(first_module(mbinfo));
  // fprintf(LOG, "  - done\n");

  // Loop forever
  // User input is accepted asynchronously via interrupts
  while(1){}
}