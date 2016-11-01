global loader                   ; the entry symbol for ELF
                                ; 'global' is a directive that includes this
                                ; symbol in the symbol table. Directives appear
                                ; at the start of the file. 'global' exports and
                                ; 'extern' imports.

extern kmain                    ; the starting point in our C code

extern kernel_virtual_start
extern kernel_virtual_end
extern kernel_physical_start
extern kernel_physical_end


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

; This is the virtual base address of kernel space. It must be used to convert virtual
; addresses into physical addresses until paging is enabled. Note that this is not
; the virtual address where the kernel image itself is loaded -- just the amount that must
; be subtracted from a virtual address to get a physical address.
global KERNEL_VIRTUAL_BASE
KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; Page directory index of kernel's 4MB PTE.

section .data
align 0x1000
global PageDirectoryVirtualAddress
PageDirectoryVirtualAddress:
    ; This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
    ; All bits are clear except the following:
    ; bit 7: PS The kernel page is 4MB.
    ; bit 1: RW The kernel page is read/write.
    ; bit 0: P  The kernel page is present.
    ; This entry must be here -- otherwise the kernel will crash immediately after paging is
    ; enabled because it can't fetch the next instruction! It's ok to unmap this page later.
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0                 ; Pages before kernel space.
    ; This page directory entry defines a 4MB page containing the kernel.
    dd 0x00000083
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0  ; Pages after the kernel image.

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

_loader:
loader equ (_loader - KERNEL_VIRTUAL_BASE)
    ; NOTE: Until paging is set up, the code must be position-independent and use physical
    ; addresses, not virtual ones!
    global PageDirectoryPhysicalAddress
    PageDirectoryPhysicalAddress equ (PageDirectoryVirtualAddress - KERNEL_VIRTUAL_BASE) ; 0x104000
    mov ecx, PageDirectoryPhysicalAddress
    mov cr3, ecx                                        ; Load Page Directory Base Register.
 
    mov ecx, cr4
    or ecx, 0x00000010                          ; Set PSE bit in CR4 to enable 4MB pages.
    mov cr4, ecx
 
    mov ecx, cr0
    or ecx, 0x80000000                          ; Set PG bit in CR0 to enable paging.
    mov cr0, ecx
 
    ; Start fetching instructions in kernel space.
    ; Since eip at this point holds the physical address of this command (approximately 0x00100000)
    ; we need to do a long jump to the correct virtual address of higher_half_loader which is
    ; approximately 0xC0100000.
    lea ecx, [higher_half_loader]
    jmp ecx

higher_half_loader:
    ; Unmap the identity-mapped first 4MB of physical address space. It should not be needed
    ; anymore.
    mov dword [PageDirectoryVirtualAddress], 0
    invlpg [0]

    mov esp, kernel_stack + KERNEL_STACK_SIZE   ; point esp to the start of the
                                                ; stack (end of memory area)

    add ebx, KERNEL_VIRTUAL_BASE ; make the address virtual
    push ebx ; GRUB stores a pointer to a struct in the register ebx that,
             ; among other things, describes at which addresses the modules are loaded.
             ; Push ebx on the stack before calling kmain to make it an argument for kmain.

    push kernel_physical_end
    push kernel_physical_start
    push kernel_virtual_end
    push kernel_virtual_start

    call kmain ; use call rather than jmp so C function finds paramters in the correct place
    hlt ; should never get here. kmain should not return.

section .bss                        ; Use the 'bss' section for the stack
    align 4                         ; align at 4 bytes for performance reasons
    kernel_stack:                   ; label points to beginning of memory
        resb KERNEL_STACK_SIZE      ; reserve stack for the kernel