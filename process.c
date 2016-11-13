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
  page_in(TMP_PAGE_TABLE_0);
  log("Paged into TMP_PAGE_TABLE_0\n");
  log("Physical address: ");
  print_uint32(LOG, (uint32_t) virtual_to_physical(TMP_PAGE_TABLE_0));
  log("\n");
}