global loader                   ; the entry symbol for ELF
                                ; 'global' is a directive that includes this
                                ; symbol in the symbol table. Directives appear
                                ; at the start of the file. 'global' exports and
                                ; 'extern' imports.

extern kmain                    ; the starting point in our C code


MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant
                                ; 'equ' is a pseduo-instruction which does not
                                ; correspond to an x86 instruction. This one
                                ; defines a literal value for a symbol, so
                                ; whereever 'MAGIC_NUMBER' shows up, in the code,
                                ; it will be replaced with 0x1BADB002

ALIGN_MODULES   equ 0x00000001  ; tell GRUB to align modules on page boundaries.
                                ; We use modules to load a 'user' program into
                                ; memory

FLAGS        equ ALIGN_MODULES  ; multiboot flags passed to GRUB

; calculate the checksum
; (magic number + checksum + flags should equal 0)
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

KERNEL_STACK_SIZE equ 4096      ; size of stack in bytes (1 kilobyte)

section .text                   ; start of the text (code) section
                                ; .data is for initialized variables
                                ; .bss is for uninitialized variables

align 4                         ; the code must be 4 byte aligned
                                ; the 'align' directive inserts NOOPS or NULLs
                                ; until we are aligned on the operand (4).
                                ; operand must be a power of 2

    dd MAGIC_NUMBER             ; write the magic number to the machine code
                                ; 'dd' declares initialized data

    dd FLAGS                    ; the flags,
    dd CHECKSUM                 ; and the checksum

loader:                         ; the loader label (defined as entry point in linker script)
                                ; that means the address of this instruction will
                                ; appear in the symbol table

    mov esp, kernel_stack + KERNEL_STACK_SIZE   ; point esp to the start of the
                                                ; stack (end of memory area)

    push ebx ; GRUB stores a pointer to a struct in the register ebx that,
             ; among other things, describes at which addresses the modules are loaded.
             ; Push ebx on the stack before calling kmain to make it an argument for kmain.

    call kmain ; use call rather than jmp so C function finds paramters in the correct place
    hlt ; should never get here. kmain should not return.

section .bss                        ; Use the 'bss' section for the stack
    align 4                         ; align at 4 bytes for performance reasons
    kernel_stack:                   ; label points to beginning of memory
        resb KERNEL_STACK_SIZE      ; reserve stack for the kernel