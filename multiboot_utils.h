#ifndef MULTIBOOT_UTILS_H
#define MULTIBOOT_UTILS_H

#include "multiboot.h"
#include "stdio.h"
#include "types.h"

void_function_t first_module_as_a_function(multiboot_info_t* info);
struct elf_section_header_t * get_elf_section(multiboot_info_t* info, char * section_name);
void print_multiboot_info(FILE stream, multiboot_info_t* info);

#endif
