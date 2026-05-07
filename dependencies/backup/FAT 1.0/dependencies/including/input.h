#ifndef INPUT_H
#define INPUT_H

#include "multiboot.h"
#define CURSOR_SIZE 32
static int prev_x;
static int prev_y;

void handle_mouse(int x, int y, int left, int right);
void handle_key(char c);

#endif