#include "including/time.h"

uint64_t cycles_per_ms = 0;
uint64_t start = 0;

uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

void pit_wait_ms(uint32_t ms) {
    // 1. Set PIT to Mode 2 (Rate Generator)
    // Channel 0, LSB/MSB, Mode 2, Binary
    outb(0x43, 0x34);

    // 2. Set divisor to maximum (65535)
    // This gives us the longest window to measure (about 55ms)
    outb(0x40, 0xFF);
    outb(0x40, 0xFF);

    for (uint32_t i = 0; i < ms; i++) {
        // Read the current count
        // Send latch command for Channel 0
        outb(0x43, 0x00);
        uint8_t low = inb(0x40);
        uint8_t high = inb(0x40);
        uint16_t start_count = (high << 8) | low;

        uint16_t current_count = start_count;
        while ((start_count - current_count) < 1193.18166) {
            outb(0x43, 0x00); // Latch
            low = inb(0x40);
            high = inb(0x40);
            current_count = (high << 8) | low;

            // Handle wrap-around if the timer resets during our loop
            if (current_count > start_count) break;
        }
    }
}

void calibrate_tsc(int time_ms) {
    uint64_t c_start = rdtsc();
    pit_wait_ms(time_ms);
    uint64_t c_end = rdtsc();

    cycles_per_ms = (c_end - c_start) / time_ms; // Divide by the actual time passed
}

uint64_t get_runtime(){ return ((rdtsc() - start)/cycles_per_ms); }

// NOT IN MS...
// its in centiseconds
int cs_since_recalibration = 0;
void kernel_sleep(uint32_t cs) {
    if (cycles_per_ms == 0) return;

    uint64_t target = rdtsc() + ((uint64_t)cs * cycles_per_ms);
    while (rdtsc() < target) {
        __asm__ volatile ("pause");
    }
}
