BITS 32 ; In a flat binary program, nasm assumes the code will run in 16-bit mode.
        ; More advanced executable formats like ELF specify this automatically.

mov eax, 0xDEADBEEF ; Write a recognizable value to a register just so we know
                    ; that this program ran.

jmp $ ; Enter infinite loop, nothing more to do.
      ; `$` means "beginning of line", ie. the same instruction.