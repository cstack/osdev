#include "stdio.h"
#include "drivers/frame_buffer.h"
#include "drivers/serial_port.h"

typedef void (*write_byte_t)(uint8_t);

write_byte_t write_byte_function(FILE stream) {
  switch (stream) {
    case (SCREEN):
      return fb_write_byte;
    case (LOG):
      return serial_write_byte;
  }
}

int fprintf (FILE stream, const char * format, ...) {
  write_byte_t write_byte = write_byte_function(stream);

  int i = 0;
  while (format[i]) {
    write_byte(format[i]);
    i++;
  }
  return i;
}

void print_int8(FILE stream, uint8_t data) {
  write_byte_t write_byte = write_byte_function(stream);

  uint8_t half_byte;
  write_byte('0');
  write_byte('x');
  for (int i = 1; i >=0; i--) {
    half_byte = (data >> (4*i)) & 0x0F;
    switch (half_byte) {
      case 0x0:
        write_byte('0');
        break;
      case 0x1:
        write_byte('1');
        break;
      case 0x2:
        write_byte('2');
        break;
      case 0x3:
        write_byte('3');
        break;
      case 0x4:
        write_byte('4');
        break;
      case 0x5:
        write_byte('5');
        break;
      case 0x6:
        write_byte('6');
        break;
      case 0x7:
        write_byte('7');
        break;
      case 0x8:
        write_byte('8');
        break;
      case 0x9:
        write_byte('9');
        break;
      case 0xA:
        write_byte('A');
        break;
      case 0xB:
        write_byte('B');
        break;
      case 0xC:
        write_byte('C');
        break;
      case 0xD:
        write_byte('D');
        break;
      case 0xE:
        write_byte('E');
        break;
      case 0xF:
        write_byte('F');
        break;
    }
  }
}

int printf (const char * format, ...) {
  return fprintf(SCREEN, format);
}

int log(char * format, ...) {
  return fprintf(LOG, format);
}