#ifndef STRING_H
#define STRING_H

#include "types.h"

void* memcpy (void* dest, const void* src, uint32_t count);
int strcmp (const char * str1, const char * str2);
int strncmp (const char * str1, const char * str2, uint32_t num);

#endif /* STRING_H */