#ifndef STDIO_H
#define STDIO_H

#include "types.h"

typedef enum output_t {SCREEN, LOG} FILE;

int fprintf (FILE stream, const char * format, ...);

#endif /* STDIO_H */