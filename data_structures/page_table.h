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

typedef uint32_t * page_directory_t;
typedef uint32_t * page_table_t;

uint32_t initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo);
page_directory_t initialize_kernel_page_directory();
uint32_t num_present_pages(page_directory_t pd);
void page_in(const void* virtual_address);
void* virtual_to_physical(const void* addr);

#endif /* INCLUDE_PAGE_TABLE_H */