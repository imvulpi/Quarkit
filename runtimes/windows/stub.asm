bits 16
; This creates a minimal DOS header that's 64 bytes. It "removes" the unnecesary "This program cannot be run in DOS mode" string from the binary.
; Simply run: nasm -f bin stub.asm -o stub.bin
db 'MZ'          ; A required magic number.
times 58 db 0    ; 60 bytes of zeros.
dd 0x00000040    ; Offset of 0x3C: Pointer to the PE header (64 bytes in).