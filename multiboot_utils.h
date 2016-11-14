#ifndef MULTIBOOT_UTILS_H
#define MULTIBOOT_UTILS_H

#include "multiboot.h"
#include "stdio.h"
#include "types.h"

struct module* first_module(multiboot_info_t* info);
struct elf_section_header_t * get_elf_section(multiboot_info_t* info, char * section_name);
uint32_t p_to_v(uint32_t physical_address);
void print_multiboot_info(FILE stream, multiboot_info_t* info);

#endif
