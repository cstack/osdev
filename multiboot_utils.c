#include "multiboot_utils.h"

void print_module_info(FILE stream, struct module * m) {
  fprintf(stream, "mod_start: ");
  print_uint32(stream, m->mod_start);
  fprintf(stream, "\n");
  fprintf(stream, "mod_end: ");
  print_uint32(stream, m->mod_end);
  fprintf(stream, "\n");
  fprintf(stream, "string: ");
  fprintf(stream, (char*) m->string);
  fprintf(stream, "\n");
}

void print_multiboot_info(FILE stream, multiboot_info_t* info) {
  fprintf(stream, "flags: ");
  print_uint32(stream, info->flags);
  fprintf(stream, "\n");

  fprintf(stream, "mem_lower: ");
  print_uint32(stream, info->mem_lower);
  fprintf(stream, "\n");
  fprintf(stream, "mem_upper: ");
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
  fprintf(stream, "mods_addr: ");
  print_uint32(stream, info->mods_addr);
  fprintf(stream, "\n");

  struct module * modules;
  if (info->mods_count > 0) {
    fprintf(stream, "\nmodules:\n");
    modules = (struct module *) info->mods_addr;
    for (uint8_t i = 0; i < info->mods_count; i++) {
      fprintf(stream, "module #");
      print_uint8(stream, i);
      fprintf(stream, ":\n");
      print_module_info(stream, &(modules[i]));
    }
  }
}

void_function_t first_module_as_a_function(multiboot_info_t* info) {
  if (info->mods_count == 0) {
    return 0;
  }

  struct module * module_info = (struct module *) info->mods_addr;
  return (void_function_t) module_info->mod_start;
}
