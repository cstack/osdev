#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

#include "../types.h"
#include "../stdlib/syscalls.h"

void keyboard_interrupt_handler();
void set_input_handler(InputHandler handler);
uint8_t read_scan_code();

#endif /* INCLUDE_KEYBOARD_H */