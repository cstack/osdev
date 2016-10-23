#ifndef INCLUDE_ELF_H
#define INCLUDE_ELF_H

#include "types.h"

struct elf_section_header_t {
  uint32_t sh_name;
  uint32_t attr1;
  uint32_t attr2;
  uint32_t sh_addr;
  uint32_t attr4;
  uint32_t sh_size;
  uint32_t attr6;
  uint32_t attr7;
  uint32_t attr8;
  uint32_t attr9;
};

typedef struct {
  uint32_t st_name;
  uint32_t st_value;
  uint32_t st_size;
  uint8_t st_info;
  uint8_t st_other;
  uint16_t st_shndx;
} Elf32_Sym;

#endif /* INCLUDE_ELF_H */