section .text
extern main
  mov eax, 0xCAFEBABE
  ; push argv
  ; push argc
  call main
  ; main has returned, eax is return value
  jmp  $    ; loop forever