#include "process.h"

#include "data_structures/page_table.h"
#include "memory.h"
#include "stdio.h"
#include "stdlib.h"

uint32_t next_pid = 0;

struct process_t {
  uint32_t pid;
  page_directory_t pd;
};

void create_process(struct module* mod) {
  struct process_t* p = malloc(sizeof(struct process_t));
  p->pid = next_pid;
  next_pid += 1;

  log("Created process descriptor:\n");
  log("Virtual address: ");
  print_uint32(LOG, (uint32_t) p);
  log("\n");

  log("pid: ");
  print_uint32(LOG, (uint32_t) p->pid);
  log("\n");

  log("Allocating a physical page for the new page directory\n");
  page_in(TMP_PAGE_0);
  page_directory_t pd = TMP_PAGE_0;
  p->pd = pd;
  log("Paged into TMP_PAGE_0\n");
  log("Physical address: ");
  print_uint32(LOG, (uint32_t) virtual_to_physical(pd));
  log("\n");

  log("Allocating a page table for user code\n");
  page_in(TMP_PAGE_1);
  page_table_t code_pt = TMP_PAGE_1;
  uint32_t pde = make_page_directory_entry(
    virtual_to_physical(code_pt),
    FOUR_KB,
    false,
    false,
    USER,
    READ_ONLY,
    true
  );
  pd[0] = pde;
  log("Allocated physical page ");
  print_uint32(LOG, (uint32_t) virtual_to_physical(code_pt));
  log(" and set entry in page directory\n");

  log("Allocating a page for user code\n");
  page_in(TMP_PAGE_2);
  void* user_code = TMP_PAGE_2;
  uint32_t pte = make_page_table_entry(
      virtual_to_physical(user_code),
      false,
      false,
      false,
      USER,
      READ_ONLY,
      true
    );
  code_pt[0] = pte;
  log("Allocated physical page ");
  print_uint32(LOG, (uint32_t) virtual_to_physical(user_code));
  log(" and set entry in code page table\n");

  log("Allocating a page table for user stack\n");
  page_in(TMP_PAGE_3);
  page_table_t stack_pt = TMP_PAGE_3;
  pde = make_page_directory_entry(
    virtual_to_physical(stack_pt),
    FOUR_KB,
    false,
    false,
    USER,
    READ_ONLY,
    true
  );
  pd[page_directory_offset(UPPER_GB_START) - 1] = pde;
  log("Allocated physical page ");
  print_uint32(LOG, (uint32_t) virtual_to_physical(stack_pt));
  log(" and set entry in page directory\n");

  log("Allocating a page for user stack\n");
  page_in(TMP_PAGE_4);
  pte = make_page_table_entry(
      virtual_to_physical(TMP_PAGE_4),
      false,
      false,
      false,
      USER,
      READ_ONLY,
      true
    );
  stack_pt[page_table_offset(UPPER_GB_START) - 1] = pte;
  log("Allocated physical page ");
  print_uint32(LOG, (uint32_t) virtual_to_physical(TMP_PAGE_4));
  log(" and set entry in stack page table\n");

  log("Mapping in kernel. Copying entries from current page directory.\n");
  map_kernel_into_page_directory(pd);

  log("Page directory:\n");
  print_page_table(LOG, pd);

  log("Copying user program into code section:\n");
  uint32_t program_length_bytes = mod->mod_end - mod->mod_start;
  void* mod_virtual_start = (void*) (mod->mod_start + UPPER_GB_START);

  log("Copying ");
  print_uint32(LOG, program_length_bytes);
  log(" bytes from ");
  print_uint32(LOG, (uint32_t) mod_virtual_start);
  log("\n");
  for (uint32_t i = 0; i < program_length_bytes; i++) {
    ((uint8_t*)user_code)[i] = ((uint8_t*)mod_virtual_start)[i];
    print_uint8(LOG, ((uint8_t*)mod_virtual_start)[i]);
    log("\n");
  }
  log("Done copying code.\n");
}