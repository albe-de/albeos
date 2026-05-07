#ifndef HARDWARE_H
#define HARDWARE_H
#include "string.h"
#include "vega.h"
#include "alsh.h"

typedef unsigned char  uint8_t;
typedef signed char    int8_t;
typedef unsigned short uint16_t;

#define true 1
#define false 0

void init_mouse();
void user_input();
void keyboard_manager(char input);

/* Inline Hardware I/O */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

#endif
