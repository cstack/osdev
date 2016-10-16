#ifndef INCLUDE_PAGE_TABLE_H
#define INCLUDE_PAGE_TABLE_H

#include "../types.h"
#include "../stdio.h"

typedef uint32_t * page_directory_t;
typedef uint32_t * page_table_t;

page_directory_t initialize_page_directory();
void print_page_directory(FILE stream, page_directory_t pd);

#endif /* INCLUDE_PAGE_TABLE_H */