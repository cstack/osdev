#include "stdio.h"
#include "drivers/frame_buffer.h"
#include "drivers/serial_port.h"

int fprintf (FILE stream, const char * format, ...) {
  void (*write_byte)(uint8_t);
  switch (stream) {
    case (SCREEN):
      write_byte = fb_write_byte;
      break;
    case (LOG):
      write_byte = serial_write_byte;
      break;
  }

  int i = 0;
  while (format[i]) {
    write_byte(format[i]);
    i++;
  }
  return i;
}

int printf (const char * format, ...) {
  return fprintf(SCREEN, format);
}

int log(char * format, ...) {
  return fprintf(LOG, format);
}