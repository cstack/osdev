section .text
global write_to_screen
; write_to_screen
; stack: [esp + 4] string
;        [esp    ] return address
write_to_screen:
  mov eax, 1
  mov ebx, [esp+4]
  int 0x80
  ret                  ; return to the calling function
