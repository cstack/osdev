#include "process.h"

#include "assembly_interface.h"
#include "data_structures/page_table.h"
#include "memory.h"
#include "stdio.h"
#include "stdlib.h"

uint32_t next_pid = 0;

struct process_t {
  uint32_t pid;
  page_directory_t pd;
};

void create_process(struct file_t* file) {
  struct process_t* p = malloc(sizeof(struct process_t));
  p->pid = next_pid;
  next_pid += 1;

  page_in(TMP_PAGE_0);
  page_directory_t pd = TMP_PAGE_0;
  p->pd = pd;
  void* pd_physical_address = virtual_to_physical(pd);

  uint32_t pde = make_page_directory_entry(
    pd_physical_address,
    FOUR_KB,
    false,
    false,
    SUPERVISOR,
    READ_WRITE,
    true
  );
  pd[1023] = pde;

  page_in(TMP_PAGE_1);
  page_table_t code_pt = TMP_PAGE_1;
  pde = make_page_directory_entry(
    virtual_to_physical(code_pt),
    FOUR_KB,
    false,
    false,
    USER,
    READ_ONLY,
    true
  );
  pd[0] = pde;

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

  page_in(TMP_PAGE_3);
  page_table_t stack_pt = TMP_PAGE_3;
  pde = make_page_directory_entry(
    virtual_to_physical(stack_pt),
    FOUR_KB,
    false,
    false,
    USER,
    READ_WRITE,
    true
  );
  pd[page_directory_offset(UPPER_GB_START) - 1] = pde;

  page_in(TMP_PAGE_4);
  pte = make_page_table_entry(
      virtual_to_physical(TMP_PAGE_4),
      false,
      false,
      false,
      USER,
      READ_WRITE,
      true
    );
  stack_pt[1023] = pte;

  map_kernel_into_page_directory(pd);

  for (uint32_t i = 0; i < file->size; i++) {
    ((char*)user_code)[i] = (file->bytes)[i];
  }

  disable_hardware_interrupts();

  set_page_directory(virtual_to_physical(pd));

  enter_user_mode();
}