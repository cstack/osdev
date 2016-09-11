#ifndef STDIO_H
#define STDIO_H

#include "types.h"

typedef enum output_t {SCREEN, LOG} FILE;

int fprintf (FILE stream, const char * format, ...);
int log(char * format, ...);
int printf (const char * format, ...);
void print_int8(FILE stream, uint8_t data);
void print_uint32(FILE stream, uint32_t data);

#endif /* STDIO_H */