#include "stdio.h"
#include "drivers/frame_buffer.h"
#include "drivers/serial_port.h"

#include "stdarg.h"

typedef void (*write_byte_t)(uint8_t);

write_byte_t write_byte_function(FILE stream) {
  switch (stream) {
    case (SCREEN):
      return fb_write_byte;
    case (LOG):
      return serial_write_byte;
  }
}

void print_char(FILE stream, char c) {
  write_byte_t write_byte = write_byte_function(stream);
  write_byte(c);
}

void print_bytes(FILE stream, uint8_t * bytes, uint32_t num_bytes) {
  write_byte_t write_byte = write_byte_function(stream);
  if (num_bytes == 0) { return; }
  print_uint8(stream, bytes[0]);
  for (uint32_t i=1; i < num_bytes; i++) {
    write_byte(' ');
    print_uint8(stream, bytes[i]);
  }
}

void write_string(write_byte_t write_byte, char* s) {
  while(*s) {
    write_byte(*s);
    s++;
  }
}

enum format_string_mode {NORMAL, COMMAND};
int fprintf (FILE stream, const char * format, ...) {
  write_byte_t write_byte = write_byte_function(stream);

  va_list vl;
  va_start(vl,format);

  int i = 0;
  enum format_string_mode mode = NORMAL;
  while (format[i]) {
    switch (mode) {
      case (NORMAL):
        if (format[i] == '%') {
          mode = COMMAND;
        } else {
          write_byte(format[i]);
        }
        break;
      case (COMMAND):
        if (format[i] == '%') {
          write_byte('%');
          mode = NORMAL;
        } else if (format[i] == 's') {
          write_string(write_byte, (char*) va_arg(vl,char*));
          mode = NORMAL;
        } else {
          write_byte('?');
          mode = NORMAL;
        }
        break;
    }
    i++;
  }
  return i;
}

void print_half_byte(write_byte_t write_byte, uint8_t half_byte) {
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

void print_uint8(FILE stream, uint8_t data) {
  write_byte_t write_byte = write_byte_function(stream);

  uint8_t half_byte;
  write_byte('0');
  write_byte('x');
  for (int i = 1; i >=0; i--) {
    half_byte = (data >> (4*i)) & 0x0F;
    print_half_byte(write_byte, half_byte);
  }
}

void print_uint16(FILE stream, uint16_t data) {
  write_byte_t write_byte = write_byte_function(stream);

  uint8_t half_byte;
  write_byte('0');
  write_byte('x');
  for (int i = 3; i >=0; i--) {
    half_byte = (data >> (4*i)) & 0x0F;
    print_half_byte(write_byte, half_byte);
  }
}

void print_uint32(FILE stream, uint32_t data) {
  write_byte_t write_byte = write_byte_function(stream);

  uint8_t half_byte;
  write_byte('0');
  write_byte('x');
  for (int i = 7; i >=0; i--) {
    half_byte = (data >> (4*i)) & 0x0F;
    print_half_byte(write_byte, half_byte);
  }
}

int printf (const char * format, ...) {
  return fprintf(SCREEN, format);
}

int log(char * format, ...) {
  return fprintf(LOG, format);
}