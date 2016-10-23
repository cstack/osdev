#include "page_allocator.h"

struct memory_stats_t memory_stats;

uint32_t initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo) {
  if(kernel_memory.kernel_virtual_start){}
  if(mbinfo){}

  memory_stats.unallocated_bytes = 0;
  memory_stats.allocated_bytes = 0;
  memory_stats.unavailable_bytes = 0;

  memory_map_t * memory_map = (memory_map_t *) mbinfo->mmap_addr;
  uint32_t num_entries = mbinfo->mmap_length / sizeof(memory_map_t);

  for (uint32_t i = 0; i < num_entries; i++) {
    if (memory_map[i].type == 1) {
      // Available
      memory_stats.unallocated_bytes += memory_map[i].length_low;
    } else {
      // Unavailable
      memory_stats.unavailable_bytes += memory_map[i].length_low;
    }
  }

  return memory_stats.unallocated_bytes;
}