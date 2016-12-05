#include "symbol_table.h"

#include "../loader.h"
#include "../multiboot_utils.h"
#include "../kernel_stdio.h"

struct symbol_table_descriptor_t {
  bool present;
  uint32_t num_symbols;
  Elf32_Sym * symbols;
  char * string_table_addr;
};

struct symbol_table_descriptor_t symbol_table_descriptor;

bool load_symbol_table(struct elf_section_header_t * symbol_table_section, struct  elf_section_header_t * string_table_section) {
  if (symbol_table_section == 0) {
    symbol_table_descriptor.present = false;
    return false;
  } else {
    symbol_table_descriptor.present = true;
    symbol_table_descriptor.num_symbols = symbol_table_section->sh_size / sizeof(Elf32_Sym);
    symbol_table_descriptor.symbols = (Elf32_Sym *) p_to_v(symbol_table_section->sh_addr);
    symbol_table_descriptor.string_table_addr = (char*) p_to_v(string_table_section->sh_addr);
    return true;
  }
}

char * address_to_symbol_name(uint32_t address) {
  // Algorithm : Find symbol with greatest value <= address

  Elf32_Sym * symbol = 0;
  uint32_t symbol_value = 0;

  if (symbol_table_descriptor.present) {
    for (uint32_t i = 0; i < symbol_table_descriptor.num_symbols; i++) {
      Elf32_Sym * candidate = symbol_table_descriptor.symbols + i;
      if (candidate->st_value > symbol_value && candidate->st_value <= address) {
        symbol = candidate;
        symbol_value = candidate->st_value;
      }
    }

    uint32_t string_index = symbol->st_name;
    char * name = symbol_table_descriptor.string_table_addr + string_index;

    return name;
  }

  return 0;
}