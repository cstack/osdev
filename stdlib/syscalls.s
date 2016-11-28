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

global count_files
; count_files
; stack: [esp    ] return address
count_files:
  mov eax, 2
  int 0x80
  ret                  ; return to the calling function

global list_files
; list_files
; stack: [esp + 4] address of buffer
;        [esp    ] return address
list_files:
  mov eax, 3
  mov ebx, [esp+4]
  int 0x80
  ret                  ; return to the calling function
