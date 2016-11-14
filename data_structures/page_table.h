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

enum page_permissions_t {READ_ONLY, READ_WRITE};
enum page_privilege_t {SUPERVISOR, USER};
enum page_size_t {FOUR_KB, FOUR_MB};

void             map_kernel_into_page_directory(page_directory_t new_pd);
page_directory_t initialize_kernel_page_directory();
uint32_t         initialize_page_allocator(
                  struct kernel_memory_descriptor_t kernel_memory,
                  multiboot_info_t* mbinfo
                );
uint32_t         make_page_directory_entry(
                  void* page_table_physical_address,
                  enum page_size_t page_size,
                  bool cache_disabled,
                  bool write_through,
                  enum page_privilege_t privelage,
                  enum page_permissions_t permissions,
                  bool present
                );
uint32_t         make_page_table_entry(
                  void* page_frame_address,
                  bool global,
                  bool cache_disabled,
                  bool write_through,
                  enum page_privilege_t privelage,
                  enum page_permissions_t permissions,
                  bool present
                );
uint32_t         num_present_pages(page_directory_t pd);
uint32_t         page_directory_offset(const void* virtual_address);
void             page_in(const void* virtual_address);
uint32_t         page_table_offset(const void* virtual_address);
void             print_page_table(FILE file, const uint32_t*);
void*            virtual_to_physical(const void* addr);

#endif /* INCLUDE_PAGE_TABLE_H */