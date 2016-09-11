extern interrupt_handler ; the C interrupt handler

global interrupt_handler_49
interrupt_handler_49:
  push    dword 0                     ; push 0 as error code
  push    dword 49                    ; push the interrupt number
  jmp     common_interrupt_handler    ; jump to the common handler

common_interrupt_handler:               ; the common parts of the generic interrupt handler
  ; save the registers
  push    eax
  push    ebx
  push    ecx
  push    edx
  push    esi
  push    edi
  push    esp
  push    ebp

  ; call the C function
  call    interrupt_handler

  ; restore the registers
  pop    ebp
  pop    esp
  pop    edi
  pop    esi
  pop    edx
  pop    ecx
  pop    ebx
  pop    eax

  ; restore the esp
  add     esp, 8

  ; return to the code that got interrupted
  iret
