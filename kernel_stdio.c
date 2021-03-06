#include "kernel_stdio.h"

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

void kernel_write_half_byte(write_byte_t write_byte, uint8_t half_byte, bool upcase) {
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
      write_byte(upcase ? 'A' : 'a');
      break;
    case 0xB:
      write_byte(upcase ? 'B' : 'b');
      break;
    case 0xC:
      write_byte(upcase ? 'C' : 'c');
      break;
    case 0xD:
      write_byte(upcase ? 'D' : 'd');
      break;
    case 0xE:
      write_byte(upcase ? 'E' : 'e');
      break;
    case 0xF:
      write_byte(upcase ? 'F' : 'f');
      break;
  }
}

void kernel_write_string(write_byte_t write_byte, char* s) {
  while(*s) {
    write_byte(*s);
    s++;
  }
}

void kernel_write_uint(write_byte_t write_byte, uint32_t value) {
  if (value == 0) {
    write_byte('0');
    return;
  }

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
      write_byte(output[place] + '0');
    }
  }
}

void kernel_write_int(write_byte_t write_byte, int value) {
  if (value < 0) {
    write_byte('-');
    value *= -1;
  }
  kernel_write_uint(write_byte, value);
}

void kernel_write_octal(write_byte_t write_byte, uint32_t value) {
  write_byte('0');
  write_byte('o');
  bool past_leading_zeroes = false;
  uint8_t part = (value >> 30) & 0b11;
  if (part > 0) {
    kernel_write_half_byte(write_byte, part, true);
    past_leading_zeroes = true;
  }
  for (int i = 10; i >=0; i--) {
    part = (value >> (3*i)) & 0b111;
    if (part > 0) {
      past_leading_zeroes = true;
    }

    if (past_leading_zeroes) {
      kernel_write_half_byte(write_byte, part, true);
    }
  }
}

enum format_string_mode {NORMAL, COMMAND};
int fprintf (FILE stream, const char * format, ...) {
  write_byte_t write_byte = write_byte_function(stream);

  va_list vl;
  va_start(vl,format);

  int i = 0;
  enum format_string_mode mode = NORMAL;
  uint32_t vararg;
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
        } else if (format[i] == 'c') {
          write_byte((uint32_t) va_arg(vl,uint32_t));
        } else if (format[i] == 'i' || format[i] == 'd') {
          kernel_write_int(write_byte, va_arg(vl, int));
        } else if (format[i] == 'o') {
          kernel_write_octal(write_byte, va_arg(vl, int));
        } else if (format[i] == 's') {
          kernel_write_string(write_byte, (char*) va_arg(vl,char*));
        } else if (format[i] == 'u') {
          kernel_write_uint(write_byte, va_arg(vl, uint32_t));
        } else if (format[i] == 'x') {
          write_byte('0');
          write_byte('x');
          vararg = va_arg(vl,uint32_t);
          for (int i = 7; i >=0; i--) {
            kernel_write_half_byte(write_byte, (vararg >> (4*i)) & 0x0F, false);
          }
        } else if (format[i] == 'X') {
          write_byte('0');
          write_byte('x');
          vararg = va_arg(vl,uint32_t);
          for (int i = 7; i >=0; i--) {
            kernel_write_half_byte(write_byte, (vararg >> (4*i)) & 0x0F, true);
          }
        } else {
          write_byte('?');
          mode = NORMAL;
        }
        mode = NORMAL;
        break;
    }
    i++;
  }
  return i;
}