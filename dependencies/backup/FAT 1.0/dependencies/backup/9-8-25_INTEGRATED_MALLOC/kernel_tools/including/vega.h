// vega.h
#ifndef VEGA_H
#define VEGA_H

#include "string.h"
extern string current_line;

void vga_put_char(int x, int y, char c, char color);
void clear_screen();
void enter_line();
void write(const char* message);
void write_char(char c);
string get_buffer();
void write_int(int num);

#endif // VEGA_H
