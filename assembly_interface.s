global outb             ; make the label outb visible outside this file

; outb - send a byte to an I/O port
; stack: [esp + 8] the data byte
;        [esp + 4] the I/O port
;        [esp    ] return address
outb:
    mov al, [esp + 8]    ; move the data to be sent into the al register
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    out dx, al           ; send the data to the I/O port
    ret                  ; return to the calling function

global inb

; inb - returns a byte from the given I/O port
; stack: [esp + 4] The address of the I/O port
;        [esp    ] The return address
inb:
    mov dx, [esp + 4]       ; move the address of the I/O port to the dx register
    in  al, dx              ; read a byte from the I/O port and store it in the al register
    ret

global asm_lgdt

; asm_lgdt - load global descriptor table
; stack: [esp + 4] the address of the gdt description structure
;        [esp    ] return address
asm_lgdt:
  mov edx, [esp + 4]
  lgdt [edx]
  ret

global  load_idt

; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the idt description structure
;        [esp    ] the return address
load_idt:
  mov     eax, [esp+4]    ; load the address of the IDT into register eax
  lidt    [eax]             ; load the IDT
  ret                     ; return to the calling function

global interrupt
; interrupt - Generates a software interrupt
interrupt:
  push ebp ; make the caller show up in the stack trace
  mov ebp, esp
  int 49
  pop ebp
  ret

global interrupt_out_of_memory
interrupt_out_of_memory:
  push ebp ; make the caller show up in the stack trace
  mov ebp, esp
  int 50
  pop ebp
  ret

global enable_hardware_interrupts
; enable_hardware_interrupts
enable_hardware_interrupts:
  sti
  ret

global set_page_directory
; sets cr3 to the physical address of a page directory
; page_directory: [esp + 4] the physical address of the page directory
;                 [esp    ] the return address
set_page_directory:
  mov eax, [esp+4]
  mov cr3, eax
  ret

global enable_paging
; sets the paging bit on control register 0
enable_paging:
  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax
  ret

global current_stack_pointer
current_stack_pointer:
    mov eax, esp
    ret
