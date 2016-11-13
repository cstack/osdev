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

void create_process() {
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
  uint32_t pte = make_page_table_entry(
      virtual_to_physical(TMP_PAGE_2),
      false,
      false,
      false,
      USER,
      READ_ONLY,
      true
    );
  code_pt[0] = pte;
  log("Allocated physical page ");
  print_uint32(LOG, (uint32_t) virtual_to_physical(TMP_PAGE_2));
  log(" and set entry in code page table\n");
}