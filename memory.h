#ifndef INCLUDE_MEMORY_H
#define INCLUDE_MEMORY_H

#define UPPER_GB_START           (const void*) 0xC0000000
#define TMP_PAGE_TABLE_0         (const void*) 0xFFBFE000
#define TMP_PAGE_TABLE_1         (const void*) 0xFFBFF000
#define FIRST_PAGE_TABLE_ADDRESS (const void*) 0xFFC00000 // 4 MB from end
#define PAGE_DIRECTORY_ADDRESS   (const void*) 0xFFFFF000 // 4 KB from end

#endif /* INCLUDE_MEMORY_H */