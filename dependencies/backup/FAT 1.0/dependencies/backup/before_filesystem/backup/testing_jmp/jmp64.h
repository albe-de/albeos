#ifndef JMP64_H
#define JMP64_H

typedef struct {
    unsigned long rbx;
    unsigned long rbp;
    unsigned long r12;
    unsigned long r13;
    unsigned long r14;
    unsigned long r15;
    unsigned long rsp;
    unsigned long rip;
} jmp_buf;

int my_setjmp(jmp_buf *env);
void my_longjmp(jmp_buf *env, int val);

#endif
