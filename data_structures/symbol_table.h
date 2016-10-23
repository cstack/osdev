#ifndef INCLUDE_SYMBOL_TABLE_H
#define INCLUDE_SYMBOL_TABLE_H

#include "../types.h"
#include "../elf.h"

struct symbol_table_descriptor_t;

bool load_symbol_table(struct elf_section_header_t * symbol_table_section, struct  elf_section_header_t * string_table_section);
char * address_to_symbol_name(uint32_t address);

#endif /* INCLUDE_SYMBOL_TABLE_H */