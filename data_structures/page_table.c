#include "page_table.h"

#include "../loader.h"

enum page_size_t {FOUR_KB, FOUR_MB};
enum page_privilege_t {SUPERVISOR, USER};
enum page_permissions_t {READ_ONLY, READ_WRITE};

struct page_directory_entry_t {
  uint32_t page_table_address;
} __attribute__((packed));

uint32_t make_page_directory_entry(
  uint32_t page_table_address,
  enum page_size_t page_size,
  bool cache_disabled,
  bool write_through,
  enum page_privilege_t privelage,
  enum page_permissions_t permissions,
  bool present
) {
  uint32_t entry = page_table_address;
  entry |= page_size << 7;
  entry |= cache_disabled << 4;
  entry |= write_through << 3;
  entry |= privelage << 2;
  entry |= permissions << 1;
  entry |= present;

  return entry;
}

uint32_t make_page_table_entry(
  uint32_t page_frame_address,
  bool global,
  bool cache_disabled,
  bool write_through,
  enum page_privilege_t privelage,
  enum page_permissions_t permissions,
  bool present
) {
  uint32_t entry = page_frame_address;
  entry |= global << 8;
  entry |= cache_disabled << 6;
  entry |= write_through << 3;
  entry |= privelage << 2;
  entry |= permissions << 1;
  entry |= present;

  return entry;
}

// 1 page = 1024 * 4 bytes = 4 kB
// 1 page table = 1024 pages = 4 mB
// 1 page directory = 1024 page tables = 4 gB

page_directory_t initialize_page_directory() {
  page_directory_t pd = (page_directory_t) &BootPageDirectory;
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
