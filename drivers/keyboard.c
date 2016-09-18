#include "keyboard.h"
#include "../assembly_interface.h"
#include "../types.h"

#define KBD_DATA_PORT   0x60

uint8_t read_scan_code()
{
  return inb(KBD_DATA_PORT);
}