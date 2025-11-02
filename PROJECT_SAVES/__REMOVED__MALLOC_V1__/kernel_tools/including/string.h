#ifndef STRING_H
#define STRING_H

#define MAX_STRING_SIZE 512
#include "malloc.h"

typedef struct {
    char* self;   // dynamically allocated buffer
    int length;   // number of characters, not including '\0'
} string;

int get_len(const char* str);
char* get_string(string* obj);

void create(string* obj, const char* str);
void add_char(string* obj, char c);
void combine_strings(string* obj, const char* str2);
void add_integer_to(string* obj, int integer);
void remove_index(string* obj, int index);
void remove_char(string* obj, char c);
void clear_string(string* obj);
void free_string(string* obj);

char* truncate_after_char(string* obj, char c);
void truncate_after_index(string* obj, int index);
char* substring_after_char(string* obj, char c);
int compare_strings(string* a, string* b);

#endif
