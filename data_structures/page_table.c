#include "page_table.h"

#include "../loader.h"
#include "../memory.h"
#include "../multiboot_utils.h"

#define PAGE_DIRECTORY_OFFSET_BITS 10
#define PAGE_TABLE_OFFSET_BITS 10
#define PAGE_OFFSET_BITS 12

#define PAGE_SIZE_BYTES 4096
#define PAGE_SIZE_DWORDS 1024

#define KERNEL_PAGE_TABLE_NUMBER 768

// #define BITMAP_SIZE 32768
// Enough room for 512 MB of RAM
// TODO: Find a more efficient way to initialize page allocator
#define BITMAP_SIZE 4096

uint32_t free_pages;
uint32_t free_page_bitmap[BITMAP_SIZE];

void mark_free(uint32_t page_number);
void mark_unavailable(uint32_t page_number);
uint32_t page_number(uint32_t address);

uint32_t make_page_directory_entry(
  void* page_table_physical_address,
  enum page_size_t page_size,
  bool cache_disabled,
  bool write_through,
  enum page_privilege_t privelage,
  enum page_permissions_t permissions,
  bool present
) {
  uint32_t entry = (uint32_t) page_table_physical_address;
  entry |= page_size << 7;
  entry |= cache_disabled << 4;
  entry |= write_through << 3;
  entry |= privelage << 2;
  entry |= permissions << 1;
  entry |= present;

  return entry;
}

bool get_present_from_pde(uint32_t pde) {
  return (pde & 0b1);
}

uint32_t make_page_table_entry(
  void* page_frame_address,
  bool global,
  bool cache_disabled,
  bool write_through,
  enum page_privilege_t privelage,
  enum page_permissions_t permissions,
  bool present
) {
  uint32_t entry = (uint32_t) page_frame_address;
  entry |= global << 8;
  entry |= cache_disabled << 6;
  entry |= write_through << 3;
  entry |= privelage << 2;
  entry |= permissions << 1;
  entry |= present;

  return entry;
}

bool get_present_from_pte(uint32_t pte) {
  return (pte & 0b1);
}

// 1 page = 1024 * 4 bytes = 4 kB
// 1 page table = 1024 pages = 4 mB
// 1 page directory = 1024 page tables = 4 gB

// Find a free physical page and return it's physical address
// This does NOT zero out the page
void* allocate_physical_page() {
  for (uint32_t index = 0; index < BITMAP_SIZE; index++) {
    if (free_page_bitmap[index] != 0) {
      // There is at least one free page in this chunk
      for (uint8_t bit = 0; bit < 32; bit++) {
        if ((free_page_bitmap[index] & (1 << bit)) != 0) {
          uint32_t page_number = index * 32 + bit;
          mark_unavailable(page_number);
          void* page_start = (void*) (page_number << PAGE_OFFSET_BITS);
          return  page_start;
        }
      }
    }
  }

  // Out of physical memory
  // TODO: Evict a page
  return 0;
}

// Assumes we are using recursive page tables.
// Last entry in page directory points to itself
void* page_table_virtual_address(uint16_t page_table_number) {
  // First 10 bits are set to 1
  uint32_t virtual_address = 0xFFC00000;

  // Next 10 bits index index into page directory
  virtual_address |= (page_table_number << PAGE_OFFSET_BITS);

  return (void*) virtual_address;
}

page_directory_t initialize_kernel_page_directory() {
  page_directory_t pd = (page_directory_t) &PageDirectoryVirtualAddress;

  // Make the last entry in the pd a pointer to itself
  uint32_t pde = make_page_directory_entry(
    (void*) &PageDirectoryPhysicalAddress,
    FOUR_KB,
    false,
    false,
    SUPERVISOR,
    READ_WRITE,
    true
  );
  pd[1023] = pde;

  // dedicate one page table for memory for the kernel
  void* page_table_physical_address = allocate_physical_page();
  pd[KERNEL_PAGE_TABLE_NUMBER] = make_page_directory_entry(
    page_table_physical_address,
    FOUR_KB,
    false,
    false,
    SUPERVISOR,
    READ_WRITE,
    true
  );

  // From here on out, we can access page tables with `page_table_virtual_address()`
  // Fill in the kernel page table entirely.
  // Map the kernel's 4MB to the first 4MB of physical memory
  page_table_t pt = (page_table_t) page_table_virtual_address(KERNEL_PAGE_TABLE_NUMBER);
  for (uint16_t i = 0; i < 1024; i++) {
    void* page_physical_address = (void*) (i * PAGE_SIZE_BYTES);
    pt[i] = make_page_table_entry(
      page_physical_address,
      false,
      false,
      false,
      SUPERVISOR,
      READ_WRITE,
      true
    );
  }

  return pd;
}

uint32_t num_present_pages(page_directory_t pd) {
  uint32_t num = 0;
  for (int i = 0; i < 1024; i++) {
    uint32_t entry = pd[i];
    bool present = entry & 0x1;

    if (present) {
      num += 1;
    }
  }
  return num;
}

uint32_t round_up_to_nearest_page_start(uint32_t address) {
  if ((address & 0xFFFFF000) != address) {
    address &= 0xFFFFF000;
    address += 0x00001000;
  }

  return address;
}

uint32_t round_down_to_nearest_page_start(uint32_t address) {
  return address & 0xFFFFF000;
}

uint32_t page_number(uint32_t address) {
  return address >> PAGE_OFFSET_BITS;
}

void mark_free(uint32_t page_number) {
  uint32_t index = page_number >> 5;
  uint32_t bit = page_number & 0b11111;
  uint32_t value = free_page_bitmap[index];

  if ((value & (1 << bit)) == 0) {
    // Page was not free before. Update statistics.
    free_pages++;
  }

  value |= (1 << bit);
  free_page_bitmap[index] = value;
}

void mark_unavailable(uint32_t physical_page_number) {
  uint32_t index = physical_page_number >> 5;
  uint32_t bit = physical_page_number & 0b11111;

  uint32_t value = free_page_bitmap[index];

  if ((value & (1 << bit)) == 1) {
    // Page was free. Update statistics.
    free_pages--;
  }

  value &= ~(1 << bit);
  free_page_bitmap[index] = value;
}

uint32_t initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo) {
  memory_map_t * memory_map = (memory_map_t *) p_to_v(mbinfo->mmap_addr);
  uint32_t num_entries = mbinfo->mmap_length / sizeof(memory_map_t);

  // Mark physical pages as free according to memory map passed by GRUB
  for (uint32_t i = 0; i < num_entries; i++) {
    if (memory_map[i].type == 1) {
      // Available
      uint32_t first_addr = memory_map[i].base_addr_low;
      uint32_t one_past_last_addr = first_addr + memory_map[i].length_low;
      uint32_t first_full_page = page_number(round_up_to_nearest_page_start(first_addr));
      uint32_t one_past_last_full_page = page_number(round_down_to_nearest_page_start(one_past_last_addr));

      for(uint32_t i = first_full_page; i < one_past_last_full_page; i++) {
        if (i > PAGE_SIZE_DWORDS) { // First 4MB are where the kernel lives
          mark_free(i);
        }
      }
    } else {
      // Unavailable
      // Not currently keeping track of these regions
    }
  }

  // Mark physical pages as unavailable where kernel code is loaded
  uint32_t first_partial_page = page_number(round_down_to_nearest_page_start(kernel_memory.kernel_physical_start));
  uint32_t one_past_last_partial_page = page_number(round_up_to_nearest_page_start(kernel_memory.kernel_physical_end));

  for(uint32_t i = first_partial_page; i < one_past_last_partial_page; i++) {
    mark_unavailable(i);
  }

  return free_pages;
}

void page_in(const void* virtual_address) {
  uint32_t pd_offset = page_directory_offset(virtual_address);
  page_directory_t pd = (page_directory_t) PAGE_DIRECTORY_ADDRESS;
  uint32_t pde = pd[pd_offset];

  if (!get_present_from_pde(pde)) {
    // Allocate a physical page to hold the page table
    void* page_table_physical_address = allocate_physical_page();

    pde = make_page_directory_entry(
      page_table_physical_address,
      FOUR_KB,
      false,
      false,
      SUPERVISOR,
      READ_WRITE,
      true
    );
    pd[pd_offset] = pde;
  }

  page_table_t pt = (page_table_t) page_table_virtual_address(pd_offset);
  uint32_t pt_offset = page_table_offset(virtual_address);
  uint32_t pte = pt[pt_offset];

  if (!get_present_from_pte(pte)) {
    // Allocate a physical page to hold the virtual page
    void* page_physical_address = allocate_physical_page();

    pte = make_page_table_entry(
      page_physical_address,
      false,
      false,
      false,
      SUPERVISOR,
      READ_WRITE,
      true
    );
    pt[pt_offset] = pte;
  }
}

uint32_t page_directory_offset(const void* virtual_address) {
  return ((uint32_t) virtual_address) >> PAGE_OFFSET_BITS >> PAGE_TABLE_OFFSET_BITS;
}

uint32_t page_table_offset(const void* virtual_address) {
  return (((uint32_t) virtual_address) >> PAGE_OFFSET_BITS) & 0x3FF;
}

uint32_t page_offset(const void* addr) {
  return ((uint32_t) addr & 0x00000FFF);
}

void* virtual_to_physical(const void* addr) {
  uint32_t pd_offset = page_directory_offset(addr);
  uint32_t pt_offset = page_table_offset(addr);
  uint32_t p_offset = page_offset(addr);

  uint32_t* page_table_addr = (uint32_t*) (FIRST_PAGE_TABLE_ADDRESS + (PAGE_SIZE_BYTES * pd_offset));
  uint32_t page_table_entry = page_table_addr[pt_offset];

  uint32_t physical_address = ((page_table_entry >> 12) << 12) | p_offset;

  return (void*) physical_address;
}

void print_page_table(FILE file, const uint32_t* pt) {
  fprintf(file, "---\n");
  for (uint32_t i = 0; i < PAGE_SIZE_DWORDS; i++) {
    if (get_present_from_pte(pt[i])) {
      print_uint32(file, i);
      fprintf(file, " -> ");
      print_uint32(file, pt[i] & (uint32_t) PAGE_DIRECTORY_ADDRESS);
      fprintf(file, "\n");
    }
  }
  fprintf(file, "---\n");
}

void map_kernel_into_page_directory(page_directory_t new_pd) {
  page_directory_t pd = (page_directory_t) PAGE_DIRECTORY_ADDRESS;
  new_pd[KERNEL_PAGE_TABLE_NUMBER] = pd[KERNEL_PAGE_TABLE_NUMBER];
}
