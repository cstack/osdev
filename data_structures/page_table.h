#ifndef INCLUDE_PAGE_TABLE_H
#define INCLUDE_PAGE_TABLE_H

#include "../multiboot.h"
#include "../stdio.h"
#include "../types.h"


struct kernel_memory_descriptor_t {
  uint32_t kernel_virtual_start;
  uint32_t kernel_virtual_end;
  uint32_t kernel_physical_start;
  uint32_t kernel_physical_end;
};

void* allocate_page();
uint32_t initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo);

typedef uint32_t * page_directory_t;
typedef uint32_t * page_table_t;

page_directory_t initialize_page_directory();
void print_page_directory(FILE stream, page_directory_t pd);

#endif /* INCLUDE_PAGE_TABLE_H */