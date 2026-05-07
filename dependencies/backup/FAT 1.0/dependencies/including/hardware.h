#ifndef HARDWARE_H
#define HARDWARE_H

#include "multiboot.h"

#define true 1
#define false 0

void init_mouse();
void read_hardware();
void set_mouse_bounds(int width, int height);

/* Inline Hardware I/O */
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

#endif
