/*
We have to define our own linker script
so the kernal is loaded at 1MB. Addresses
below that point are used by BIOS and GRUB,
which load the kernal.
*/

ENTRY(loader)                /* the name of the entry label */
                             /* argument is a symbol name exported by and object
                             file that we're linking */

SECTIONS {
    . = 0x00100000;          /* the code should be loaded at 1 MB */

    .text ALIGN (0x1000) :   /* align at 4 KB, the size of a page */
    {
        *(.text)             /* all text sections from all files */
    }

    .rodata ALIGN (0x1000) : /* align at 4 KB, the size of a page */
    {
        *(.rodata*)          /* all read-only data sections from all files */
    }

    .data ALIGN (0x1000) :   /* align at 4 KB, the size of a page */
    {
        *(.data)             /* all data sections from all files */
    }

    .bss ALIGN (0x1000) :    /* align at 4 KB, the size of a page */
    {
        *(COMMON)            /* all COMMON sections from all files */
        *(.bss)              /* all bss sections from all files */
    }
}