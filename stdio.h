#ifndef STDIO_H
#define STDIO_H

typedef enum output_t {SCREEN, LOG} FILE;

int fprintf (FILE stream, const char * format, ...);
int log(char * format, ...);

#endif /* STDIO_H */