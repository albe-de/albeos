#ifndef STRING_H
#define STRING_H

#include "kernel_malloc.h"

typedef struct {
    char* self;  // dynamically allocated buffer
    int length;
    int capacity;
} string;

void draw_mouse(int mouse_x, int mouse_y);
int get_len(char* str);
char* get_string(string* obj);
void create(string* obj, char* str);
void add_char(string* obj, char c);
void combine_strings(string* obj, const char* str2);
void add_integer_to(string* obj, int integer);
void remove_index(string* obj, int index);
void remove_char(string* obj, char c);
void clear_string(string* obj);

char* truncate_after_char(string* obj, char c);
void truncate_after_index(string* obj, int index);
char* substring_after_char(string* obj, char c);
int compare_strings(string* a, string* b);
int is_empty_string(const char* str);

// Helper for copying memory
void memcpy_custom(char* dest, const char* src, int len);

#endif
