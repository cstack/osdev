#ifndef INCLUDE_MEMORY_H
#define INCLUDE_MEMORY_H

#define UPPER_GB_START           (void*) 0xC0000000
#define TMP_PAGE_0               (void*) 0xFFBFB000
#define TMP_PAGE_1               (void*) 0xFFBFC000
#define TMP_PAGE_2               (void*) 0xFFBFD000
#define TMP_PAGE_3               (void*) 0xFFBFE000
#define TMP_PAGE_4               (void*) 0xFFBFF000
#define FIRST_PAGE_TABLE_ADDRESS (void*) 0xFFC00000 // 4 MB from end
#define PAGE_DIRECTORY_ADDRESS   (void*) 0xFFFFF000 // 4 KB from end

#define KERNEL_STACK_SIZE 4096

#endif /* INCLUDE_MEMORY_H */