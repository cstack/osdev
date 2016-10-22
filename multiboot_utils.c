#include "multiboot_utils.h"
#include "loader.h"
#include "elf.h"

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

void print_multiboot_info(FILE stream, multiboot_info_t* info) {
  fprintf(stream, "flags: ");
  print_uint32(stream, info->flags);
  fprintf(stream, "\n");

  fprintf(stream, "mem_lower (virtual): ");
  print_uint32(stream, p_to_v(info->mem_lower));
  fprintf(stream, "\n");
  fprintf(stream, "mem_upper (virtual): ");
  print_uint32(stream, p_to_v(info->mem_upper));
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
}

void_function_t first_module_as_a_function(multiboot_info_t* info) {
  if (info->mods_count == 0) {
    return 0;
  }

  struct module * module_info = (struct module *) info->mods_addr;
  return (void_function_t) module_info->mod_start;
}
