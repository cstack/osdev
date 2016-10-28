#ifndef PAGE_ALLOCATOR_H
#define PAGE_ALLOCATOR_H

#include "types.h"
#include "multiboot.h"

struct kernel_memory_descriptor_t {
  uint32_t kernel_virtual_start;
  uint32_t kernel_virtual_end;
  uint32_t kernel_physical_start;
  uint32_t kernel_physical_end;
};

bool allocate_page(uint32_t virtual_address);
uint32_t initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo);

#endif /* PAGE_ALLOCATOR_H */