#include <stdio.h>
#include "jmp64.h"

jmp_buf env;

int main() {
    int val = my_setjmp(&env);
    if (val == 0) {
        printf("my_setjmp returned 0, jumping back...\n");
        my_longjmp(&env, 123);
        printf("This should never print\n");
    } else {
        printf("my_longjmp returned with value %d\n", val);
    }
    return 0;
}
