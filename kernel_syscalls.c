#include "kernel_syscalls.h"

#include "drivers/keyboard.h"
#include "kernel_filesystem.h"
#include "kernel_stdio.h"
#include "stdlib/syscalls.h"

uint32_t sys_write_to_screen(char* s) {
  fprintf(SCREEN, s);
  return 0;
}

uint32_t sys_count_files() {
  struct file_t* file = get_first_file();
  uint32_t count = 0;
  while (file) {
    count++;
    file = file->next_sibling;
  }

  return count;
}

uint32_t sys_list_files(struct file_t* buffer) {
  struct file_t* file = get_first_file();
  while (file) {
    *buffer = *file;
    buffer++;
    file = file->next_sibling;
  }

  return 0;
}

uint32_t sys_register_input_handler(InputHandler handler) {
  set_input_handler(handler);
  return 0;
}

char* syscall_name(Syscall syscall) {
  switch (syscall) {
    case (WRITE_TO_SCREEN):
      return "WRITE_TO_SCREEN";
    case (COUNT_FILES):
      return "COUNT_FILES";
    case (LIST_FILES):
      return "LIST_FILES";
    case (REGISTER_INPUT_HANDLER):
      return "REGISTER_INPUT_HANDLER";
  }
}

uint32_t handle_syscall(struct cpu_state* cpu) {
  Syscall syscall = (Syscall) cpu->eax;

  fprintf(LOG, "--------------------\nSYSCALL (%i - %s)\n", syscall, syscall_name(syscall));

  switch (syscall) {
    case (WRITE_TO_SCREEN):
      return sys_write_to_screen((char*) cpu->ebx);
    case (COUNT_FILES):
      return sys_count_files();
    case (LIST_FILES):
      return sys_list_files((struct file_t *) cpu->ebx);
    case (REGISTER_INPUT_HANDLER):
      return sys_register_input_handler((InputHandler) cpu->ebx);
    default:
      fprintf(LOG, "Unknown syscall: %x\n", syscall);
      while(1){}
  }

  fprintf(LOG, "--------------------\n");
}