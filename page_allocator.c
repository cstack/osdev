#include "page_allocator.h"

#include "stdio.h"

uint32_t free_pages;

// 2^32 address space
// 2^12 page size
// 2^20 pages
// 2^17 bytes in bitmap
// 2^15 uint32_t's = 32768

// #define BITMAP_SIZE 32768
// Enough room for 515 MB of RAM
// TODO: Find a more efficient way to initialize page allocator
#define BITMAP_SIZE 4096
uint32_t free_page_bitmap[BITMAP_SIZE];

uint32_t round_up_to_nearest_page_start(uint32_t address) {
  if ((address & 0xFFF) != 0) {
    address &= 0xFFFFF000;
    address += 0x00001000;
  }
  return address;
}

uint32_t round_down_to_nearest_page_start(uint32_t address) {
  if ((address & 0xFFF) != 0) {
    address &= 0xFFFFF000;
    address -= 0x00001000;
  }
  return address;
}

uint32_t page_number(uint32_t address) {
  return address >> 12;
}

void mark_free(uint32_t page_number) {
  uint32_t index = page_number >> 5;
  uint32_t bit = page_number & 0b11111;
  uint32_t value = free_page_bitmap[index];
  value |= (1 << bit);
  free_page_bitmap[index] = value;
  free_pages++;
}

void mark_unavailable(uint32_t page_number) {
  uint32_t index = page_number >> 5;
  uint32_t bit = page_number & 0b11111;
  uint32_t value = free_page_bitmap[index];
  value &= ~(1 << bit);
  free_page_bitmap[index] = value;
  free_pages--;
}

uint32_t initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo) {
  if(kernel_memory.kernel_virtual_start){}
  if(mbinfo){}

  memory_map_t * memory_map = (memory_map_t *) mbinfo->mmap_addr;
  uint32_t num_entries = mbinfo->mmap_length / sizeof(memory_map_t);

  for (uint32_t i = 0; i < num_entries; i++) {
    if (memory_map[i].type == 1) {
      // Available
      uint32_t first_addr = memory_map[i].base_addr_low;
      uint32_t one_past_last_addr = first_addr + memory_map[i].length_low;
      uint32_t first_full_page = page_number(round_up_to_nearest_page_start(first_addr));
      uint32_t one_past_last_full_page = page_number(round_down_to_nearest_page_start(one_past_last_addr));

      for(uint32_t i = first_full_page; i < one_past_last_full_page; i++) {
        mark_free(i);
      }
    } else {
      // Unavailable
    }
  }

  uint32_t first_partial_page = page_number(round_down_to_nearest_page_start(kernel_memory.kernel_physical_start));
  uint32_t one_past_last_partial_page = page_number(round_up_to_nearest_page_start(kernel_memory.kernel_physical_end));

  for(uint32_t i = first_partial_page; i < one_past_last_partial_page; i++) {
    mark_unavailable(i);
  }

  return free_pages;
}