.global _start
.extern kernel_main

.section .bss
.align 16
stack:
    .skip 16384   # 16 KB stack

.section .text
_start:
    cli

    # set stack pointer
    mov $stack + 16384, %esp

    # pass multiboot info pointer
    push %ebx
    call kernel_main

hang:
    hlt
    jmp hang