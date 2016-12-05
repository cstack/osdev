#include "multiboot_utils.h"
#include "elf.h"
#include "loader.h"
#include "stdlib/string.h"

uint32_t p_to_v(uint32_t physical_address) {
  return physical_address + (uint32_t) &KERNEL_VIRTUAL_BASE;
}

void print_module_info(FILE stream, struct module * m) {
  fprintf(stream, (char*) (p_to_v(m->string)));
  fprintf(stream, ":\n");
  fprintf(stream, "mod_start (virtual): %x\n", p_to_v(m->mod_start));
  fprintf(stream, "mod_end (virtual): %x\n", p_to_v(m->mod_end));
}

struct elf_section_header_t * get_elf_section(multiboot_info_t* info, char * section_name) {
  elf_section_header_table_t section_table = info->u.elf_sec;

  uint32_t addr = p_to_v(section_table.addr);
  uint32_t num_sections = section_table.num;
  uint32_t shndx = section_table.shndx;

  struct elf_section_header_t * section_header_table = (struct elf_section_header_t *) addr;
  uint32_t string_table_start = p_to_v(section_header_table[shndx].sh_addr);

  for (uint32_t i = 0; i < num_sections; i++) {
    uint32_t sh_name = section_header_table[i].sh_name;
    if (sh_name != 0) {
      char * name = (char*) (string_table_start + sh_name);
      int diff = strcmp(name, section_name);
      if (diff == 0) {
        return section_header_table + i;
      }
    }
  }

  return 0;
}


void print_elf_section_header_table(FILE stream, elf_section_header_table_t table) {
  fprintf(stream, "\nELF sections:\n");

  uint32_t addr = p_to_v(table.addr);
  uint32_t num = table.num;
  uint32_t shndx = table.shndx;

  struct elf_section_header_t * section_header_table = (struct elf_section_header_t *) addr;
  uint32_t string_table_start = p_to_v(section_header_table[shndx].sh_addr);

  for (uint32_t i = 0; i < num; i++) {
    uint32_t sh_addr = section_header_table[i].sh_addr;
    fprintf(stream, "%x : %x : ", sh_addr, section_header_table[i].sh_size);

    uint32_t sh_name = section_header_table[i].sh_name;
    if (sh_name != 0) {
      char * name = (char*) (string_table_start + sh_name);
      fprintf(stream, name);
    } else {
      fprintf(stream, "(NULL SECTION)");
    }
    fprintf(stream, "\n");
  }
}

void print_memory_map(FILE stream, multiboot_info_t* info) {
  fprintf(stream, "\nmemory map:\n");
  memory_map_t * memory_map = (memory_map_t *) p_to_v(info->mmap_addr);
  uint32_t num_entries = info->mmap_length / sizeof(memory_map_t);

  for (uint32_t i = 0; i < num_entries; i++) {
    fprintf(stream, "base_addr_low: %x\n", memory_map[i].base_addr_low);
    fprintf(stream, "base_addr_high: %x\n", memory_map[i].base_addr_high);
    fprintf(stream, "length_low: %x\n", memory_map[i].length_low);
    fprintf(stream, "length_high: %x\n", memory_map[i].length_high);
    fprintf(stream, "type: %x\n", memory_map[i].type);
    fprintf(stream, "\n");
  }
}

void print_multiboot_info(FILE stream, multiboot_info_t* info) {
  fprintf(stream, "flags: %x\n", info->flags);
  fprintf(stream, "mem_lower (kilobytes): %i\n", info->mem_lower);
  fprintf(stream, "mem_upper (kilobytes): %i\n", info->mem_upper);
  fprintf(stream, "boot_device: %x\n", info->boot_device);
  fprintf(stream, "cmdline: %x\n", info->cmdline);
  fprintf(stream, "mods_count: %i\n", info->mods_count);
  fprintf(stream, "mods_addr (virtual): %x\n", info->mods_addr);

  fprintf(stream, "\nGRUB modules: \n");
  struct module * modules;
  if (info->mods_count > 0) {
    modules = (struct module *) (p_to_v(info->mods_addr));
    for (uint8_t i = 0; i < info->mods_count; i++) {
      print_module_info(stream, &(modules[i]));
    }
  }

  print_elf_section_header_table(stream, info->u.elf_sec);

  print_memory_map(stream, info);
}

struct module* first_module(multiboot_info_t* info) {
  if (info->mods_count == 0) {
    return 0;
  }

  struct module* module_info = (struct module*) (p_to_v(info->mods_addr));
  return module_info;
}
