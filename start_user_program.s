section .text
extern main
  mov eax, 0xCAFEBABE
  ; push argv
  ; push argc
  ; mov eax, [esp] ; accessing the stack currently causes a crash
  jmp $
  call main2
  jmp $
  ; main has returned, eax is return value
  mov eax, 0xDEADBEEF
  jmp  $    ; loop forever

  main2:
  jmp $
  mov eax, 0x11111111
  jmp $