#include "stdlib.h"

#include "assembly_interface.h"
#include "data_structures/page_table.h"
#include "loader.h"

// naive strategy for now: keep track of highest allocated
// address, don't worry about freeing memory.

void* lowest_free_memory = &kernel_stack_lowest_address;

void* malloc(uint32_t bytes) {
  void* chunk_start = lowest_free_memory;
  lowest_free_memory += bytes;

  if (lowest_free_memory >= current_stack_pointer()) {
    interrupt_out_of_memory();
  }

  return chunk_start;
}