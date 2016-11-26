#include "stdio.h"

#include "assembly_functions.h"
#include "stdarg.h"

#define BUFFER_SIZE 128

struct buffer_t {
  char bytes[BUFFER_SIZE+1]; // 1 bytes for null character
  uint8_t bytes_in_buffer;
};

void flush_buffer(struct buffer_t* buffer) {
  write_to_screen(buffer->bytes);
  buffer->bytes_in_buffer = 0;
}

void write_byte(char byte, struct buffer_t* buffer) {
  buffer->bytes[buffer->bytes_in_buffer] = byte;

  buffer->bytes_in_buffer++;

  if (buffer->bytes_in_buffer >= BUFFER_SIZE) {
    flush_buffer(buffer);
  }
}

void write_half_byte(uint8_t half_byte, bool upcase, struct buffer_t* buffer) {
  switch (half_byte) {
    case 0x0:
      write_byte('0', buffer);
      break;
    case 0x1:
      write_byte('1', buffer);
      break;
    case 0x2:
      write_byte('2', buffer);
      break;
    case 0x3:
      write_byte('3', buffer);
      break;
    case 0x4:
      write_byte('4', buffer);
      break;
    case 0x5:
      write_byte('5', buffer);
      break;
    case 0x6:
      write_byte('6', buffer);
      break;
    case 0x7:
      write_byte('7', buffer);
      break;
    case 0x8:
      write_byte('8', buffer);
      break;
    case 0x9:
      write_byte('9', buffer);
      break;
    case 0xA:
      write_byte(upcase ? 'A' : 'a', buffer);
      break;
    case 0xB:
      write_byte(upcase ? 'B' : 'b', buffer);
      break;
    case 0xC:
      write_byte(upcase ? 'C' : 'c', buffer);
      break;
    case 0xD:
      write_byte(upcase ? 'D' : 'd', buffer);
      break;
    case 0xE:
      write_byte(upcase ? 'E' : 'e', buffer);
      break;
    case 0xF:
      write_byte(upcase ? 'F' : 'f', buffer);
      break;
  }
}

void print_uint8(uint8_t data, struct buffer_t* buffer) {
  uint8_t half_byte;
  write_byte('0', buffer);
  write_byte('x', buffer);
  for (int i = 1; i >=0; i--) {
    half_byte = (data >> (4*i)) & 0x0F;
    write_half_byte(half_byte, true, buffer);
  }
}

void write_string(char* s, struct buffer_t* buffer) {
  while(*s) {
    write_byte(*s, buffer);
    s++;
  }
}

void write_uint(uint32_t value, struct buffer_t* buffer) {
  char output[10];
  for (int place = 0; place < 10; place++) {
    output[place] = value % 10;
    value = value / 10;
  }

  bool past_leading_zeroes = false;
  for (int place = 9; place >= 0; place--) {
    if (output[place] > 0) {
      past_leading_zeroes = true;
    }

    if (past_leading_zeroes) {
      write_byte(output[place] + '0', buffer);
    }
  }
}

void write_int(int value, struct buffer_t* buffer) {
  if (value < 0) {
    write_byte('-', buffer);
    value *= -1;
  }
  write_uint(value, buffer);
}

void write_octal(uint32_t value, struct buffer_t* buffer) {
  write_byte('0', buffer);
  write_byte('o', buffer);
  bool past_leading_zeroes = false;
  uint8_t part = (value >> 30) & 0b11;
  if (part > 0) {
    write_half_byte(part, true, buffer);
    past_leading_zeroes = true;
  }
  for (int i = 10; i >=0; i--) {
    part = (value >> (3*i)) & 0b111;
    if (part > 0) {
      past_leading_zeroes = true;
    }

    if (past_leading_zeroes) {
      write_half_byte(part, true, buffer);
    }
  }
}

enum format_string_mode {NORMAL, COMMAND};
int printf (const char * format, ...) {
  va_list vl;
  va_start(vl,format);

  struct buffer_t buffer;
  for (int i = 0; i <=BUFFER_SIZE; i++) {
    buffer.bytes[i] = 0;
    buffer.bytes_in_buffer = 0;
  }

  int i = 0;
  enum format_string_mode mode = NORMAL;
  uint32_t vararg;
  while (format[i]) {
    switch (mode) {
      case (NORMAL):
        if (format[i] == '%') {
          mode = COMMAND;
        } else {
          write_byte(format[i], &buffer);
        }
        break;
      case (COMMAND):
        if (format[i] == '%') {
          write_byte('%', &buffer);
        } else if (format[i] == 'c') {
          write_byte((uint32_t) va_arg(vl,uint32_t), &buffer);
        } else if (format[i] == 'i' || format[i] == 'd') {
          write_int(va_arg(vl, int), &buffer);
        } else if (format[i] == 'o') {
          write_octal(va_arg(vl, int), &buffer);
        } else if (format[i] == 's') {
          write_string((char*) va_arg(vl,char*), &buffer);
        } else if (format[i] == 'u') {
          write_uint(va_arg(vl, uint32_t), &buffer);
        } else if (format[i] == 'x') {
          write_byte('0', &buffer);
          write_byte('x', &buffer);
          vararg = va_arg(vl,uint32_t);
          for (int i = 7; i >=0; i--) {
            write_half_byte((vararg >> (4*i)) & 0x0F, false, &buffer);
          }
        } else if (format[i] == 'X') {
          write_byte('0', &buffer);
          write_byte('x', &buffer);
          vararg = va_arg(vl,uint32_t);
          for (int i = 7; i >=0; i--) {
            write_half_byte((vararg >> (4*i)) & 0x0F, true, &buffer);
          }
        } else {
          write_byte('?', &buffer);
          mode = NORMAL;
        }
        mode = NORMAL;
        break;
    }
    i++;
  }
  flush_buffer(&buffer);
  return i;
}