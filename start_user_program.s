section .text
extern main
  mov eax, 0xCAFEBABE
  ; push argv
  ; push argc
  call interrupt
  mov eax, 0xDEADBEEF
  ; main has returned, eax is return value
  jmp  $    ; loop forever

interrupt:
  push ebp ; make the caller show up in the stack trace
  mov ebp, esp
  int 0x80
  pop ebp
  ret