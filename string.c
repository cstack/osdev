#include "string.h"

/*
Returns an integral value indicating the relationship between the strings:
return value  indicates
<0  the first character that does not match has a lower value in ptr1 than in ptr2
0 the contents of both strings are equal
>0  the first character that does not match has a greater value in ptr1 than in ptr2
*/
int strcmp(const char * str1, const char * str2) {
  while (*str1 != 0 && *str2 != 0) {
    int diff = *str1 - *str2;
    if (diff != 0) {
      return diff;
    }

    str1++;
    str2++;
  }

  // One of these points to 0
  return *str1 - *str2;
}

int strncmp (const char * str1, const char * str2, uint32_t num) {
  while (*str1 != 0 && *str2 != 0 && num > 1) {
    int diff = *str1 - *str2;
    if (diff != 0) {
      return diff;
    }

    str1++;
    str2++;
    num--;
  }

  return *str1 - *str2;
}