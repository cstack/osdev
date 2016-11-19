section .text
extern main
  mov eax, 0xCAFEBABE
  ; push argv
  ; push argc
  call main
  jmp $
  ; main has returned, eax is return value
  mov eax, 0xDEADBEEF
  jmp  $    ; loop forever