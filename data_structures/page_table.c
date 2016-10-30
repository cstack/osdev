#include "page_table.h"

#include "../loader.h"
#include "../multiboot_utils.h"

uint32_t free_pages;

#define PAGE_DIRECTORY_OFFSET_BITS 10
#define PAGE_TABLE_OFFSET_BITS 10
#define PAGE_OFFSET_BITS 12

#define PAGE_SIZE_BYTES 4096
#define PAGE_SIZE_DWORDS 1024

// #define BITMAP_SIZE 32768
// Enough room for 512 MB of RAM
// TODO: Find a more efficient way to initialize page allocator
#define BITMAP_SIZE 4096
uint32_t free_page_bitmap[BITMAP_SIZE];

uint32_t page_number(uint32_t address);
enum page_size_t {FOUR_KB, FOUR_MB};
enum page_privilege_t {SUPERVISOR, USER};
enum page_permissions_t {READ_ONLY, READ_WRITE};

uint32_t make_page_directory_entry(
  void* page_table_physical_address,
  enum page_size_t page_size,
  bool cache_disabled,
  bool write_through,
  enum page_privilege_t privelage,
  enum page_permissions_t permissions,
  bool present
) {
  log("make_page_directory_entry()\n");
  uint32_t entry = (uint32_t) page_table_physical_address;
  entry |= page_size << 7;
  entry |= cache_disabled << 4;
  entry |= write_through << 3;
  entry |= privelage << 2;
  entry |= permissions << 1;
  entry |= present;

  return entry;
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

void mark_free(uint32_t page_number);
void mark_unavailable(uint32_t page_number);

// 1 page = 1024 * 4 bytes = 4 kB
// 1 page table = 1024 pages = 4 mB
// 1 page directory = 1024 page tables = 4 gB

void* allocate_physical_page() {
  // log("allocate_physical_page()\n");
  for (uint32_t index = 0; index < BITMAP_SIZE; index++) {
    // log("allocate_physical_page() index ");
    // print_uint32(LOG, index);
    // log("\n");
    if (free_page_bitmap[index] != 0) {
      for (uint8_t bit = 0; bit < 32; bit++) {
        // log("allocate_physical_page() bit ");
        // print_uint32(LOG, bit);
        // log("\n");
        if ((free_page_bitmap[index] & (1 << bit)) != 0) {
          uint32_t page_number = index * 32 + bit;
          mark_unavailable(page_number);
          uint32_t* page_start = (uint32_t*) (page_number << 12);
          // for (uint32_t i = 0; i < PAGE_SIZE_DWORDS; i++) {
          //   page_start[i] = 0;
          // }
          return  (void*) page_start;
        }
      }
    }
  }
  return 0;
}

void * page_table_virtual_address(uint32_t page_number) {
  uint32_t virtual_address = page_number << PAGE_OFFSET_BITS;
  virtual_address |= 0xFFC00000;
  return (void*) virtual_address;
}

page_directory_t initialize_page_directory() {
  page_directory_t pd = (page_directory_t) &BootPageDirectory;
  log("pd: ");
  print_uint32(LOG, (uint32_t) pd);
  log("\n");

  // Make the last entry in the pd a pointer to itself
  log("Setting last entry in page directory.\n");
  uint32_t pde = make_page_directory_entry(
    (void*) &BootPageDirectoryPhysicalAddress,
    FOUR_KB,
    false,
    false,
    SUPERVISOR,
    READ_WRITE,
    true
  );
  log("pde: ");
  print_uint32(LOG, pde);
  log("\n");
  pd[1023] = pde;

  // dedicate one page table for memory for the kernel
  void* page_table_physical_address = allocate_physical_page();
  log("allocated physical memory for page table: ");
  print_uint32(LOG, (uint32_t) page_table_physical_address);
  log("\n");
  pd[768] = make_page_directory_entry(
    page_table_physical_address,
    FOUR_KB,
    false,
    false,
    SUPERVISOR,
    READ_WRITE,
    true
  );

  // From here on out, we can access present page tables by setting the
  // upper bits of the address to 0xFFFFF
  page_table_t pt = (page_table_t) page_table_virtual_address(768);
  log("page_table virtual address: ");
  print_uint32(LOG, (uint32_t) pt);
  log("\n");
  for (uint32_t i = 0; i < 1024; i++) {
    void* page_base = allocate_physical_page();
    // log("allocated physical memory for a kernel page: ");
    // print_uint32(LOG, (uint32_t) page_base);
    // log("\n");
    pt[i] = make_page_table_entry(
      page_base,
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

void print_page_directory(FILE stream, page_directory_t pd) {
  for (int i = 0; i < 1024; i++) {
    uint32_t entry = pd[i];
    bool present = entry & 0x1;

    if (present) {
      fprintf(stream, "page directory entry ");
      print_uint32(stream, i);
      fprintf(stream, " is present.\n");
    }
  }
}

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
  memory_map_t * memory_map = (memory_map_t *) p_to_v(mbinfo->mmap_addr);
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

  // for(uint16_t i = 0; i < BITMAP_SIZE; i++) {
  //   print_uint16(LOG, i);
  //   print_uint32(LOG, free_page_bitmap[i]);
  //   log("\n");
  // }

  return free_pages;
}
