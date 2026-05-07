#ifndef TIME_H
#define TIME_H

#include "hardware.h"

typedef unsigned long long uint64_t;
extern uint64_t cycles_per_ms;
extern uint64_t start;

uint64_t rdtsc();
void pit_wait_ms(uint32_t ms);
void calibrate_tsc(int time_ms);
uint64_t get_runtime();
// void kernel_sleep(uint32_t ms);

#endif
