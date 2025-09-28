section .text
    global _start

_start:
    ; Jump over the code below to label 'next_instruction'
    jmp next_instruction

    ; This code will be skipped
    mov rax, 1          ; syscall write
    mov rdi, 1          ; stdout
    mov rsi, message    ; message to write
    mov rdx, len        ; message length
    syscall

next_instruction:
    ; Exit syscall
    mov rax, 60         ; sys_exit
    xor rdi, rdi        ; exit code 0
    syscall

section .data
message db 'This should be skipped.', 0xA
len equ $ - message
