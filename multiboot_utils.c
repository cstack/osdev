#include "multiboot_utils.h"
#include "elf.h"
#include "loader.h"
#include "string.h"

uint32_t p_to_v(uint32_t physical_address) {
  return physical_address + (uint32_t) &KERNEL_VIRTUAL_BASE;
}

void print_module_info(FILE stream, struct module * m) {
  fprintf(stream, (char*) (p_to_v(m->string)));
  fprintf(stream, ":\n");
  fprintf(stream, "mod_start (virtual): ");
  print_uint32(stream, p_to_v(m->mod_start));
  fprintf(stream, "\n");
  fprintf(stream, "mod_end (virtual): ");
  print_uint32(stream, p_to_v(m->mod_end));
  fprintf(stream, "\n");
}

struct elf_section_header_t * get_elf_section(multiboot_info_t* info, char * section_name) {
  elf_section_header_table_t section_table = info->u.elf_sec;

  uint32_t addr = p_to_v(section_table.addr);
  uint32_t num_sections = section_table.num;
  uint32_t shndx = section_table.shndx;

  struct elf_section_header_t * section_header_table = (struct elf_section_header_t *) addr;
  uint32_t string_table_start = section_header_table[shndx].sh_addr;

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
  uint32_t string_table_start = section_header_table[shndx].sh_addr;

  for (uint32_t i = 0; i < num; i++) {
    uint32_t sh_addr = section_header_table[i].sh_addr;
    print_uint32(stream, sh_addr);

    fprintf(stream, " : ");
    print_uint32(stream, section_header_table[i].sh_size);

    fprintf(stream, " : ");
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
  memory_map_t * memory_map = (memory_map_t *) info->mmap_addr;
  uint32_t num_entries = info->mmap_length / sizeof(memory_map_t);

  for (uint32_t i = 0; i < num_entries; i++) {
    fprintf(stream, "base_addr_low: ");
    print_uint32(stream, memory_map[i].base_addr_low);
    fprintf(stream, "\n");

    fprintf(stream, "base_addr_high: ");
    print_uint32(stream, memory_map[i].base_addr_high);
    fprintf(stream, "\n");

    fprintf(stream, "length_low: ");
    print_uint32(stream, memory_map[i].length_low);
    fprintf(stream, "\n");

    fprintf(stream, "length_high: ");
    print_uint32(stream, memory_map[i].length_high);
    fprintf(stream, "\n");

    fprintf(stream, "type: ");
    print_uint32(stream, memory_map[i].type);
    fprintf(stream, "\n");

    fprintf(stream, "\n");
  }
}

void print_multiboot_info(FILE stream, multiboot_info_t* info) {
  fprintf(stream, "flags: ");
  print_uint32(stream, info->flags);
  fprintf(stream, "\n");

  fprintf(stream, "mem_lower (kilobytes): ");
  print_uint32(stream, info->mem_lower);
  fprintf(stream, "\n");
  fprintf(stream, "mem_upper (kilobytes): ");
  print_uint32(stream, info->mem_upper);
  fprintf(stream, "\n");

  fprintf(stream, "boot_device: ");
  print_uint32(stream, info->boot_device);
  fprintf(stream, "\n");
  fprintf(stream, "cmdline: ");
  print_uint32(stream, info->cmdline);
  fprintf(stream, "\n");

  fprintf(stream, "mods_count: ");
  print_uint32(stream, info->mods_count);
  fprintf(stream, "\n");
  fprintf(stream, "mods_addr (virtual): ");
  print_uint32(stream, p_to_v(info->mods_addr));
  fprintf(stream, "\n");

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

void_function_t first_module_as_a_function(multiboot_info_t* info) {
  if (info->mods_count == 0) {
    return 0;
  }

  struct module * module_info = (struct module *) info->mods_addr;
  return (void_function_t) module_info->mod_start;
}
