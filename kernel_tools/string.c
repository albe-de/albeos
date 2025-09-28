#include "including/string.h"
#include "including/kernel_malloc.h"

// Basic helpers
int get_len(char* message) {
    int i = 0;
    while (message[i] != '\0') i++;
    return i;
}

char* get_string(string* obj) {
    return obj->self;
}

// Create a new string
void create(string* obj, char* str) {
    int len = get_len(str);
    obj->self = (char*)kmalloc(len + 1);  // +1 for null terminator
    if (!obj->self) return;

    for (int i = 0; i < len; i++) obj->self[i] = str[i];
    obj->self[len] = '\0';
    obj->length = len;
}

// Clear string
void clear_string(string* obj) {
    if (!obj->self) return;
    obj->self[0] = '\0';
    obj->length = 0;
}

// Add a char
void add_char(string* obj, char c) {
    int len = obj->length;
    char* new_buffer = (char*)kmalloc(len + 2); // +1 for new char + null
    if (!new_buffer) return;

    for (int i = 0; i < len; i++) new_buffer[i] = obj->self[i];
    new_buffer[len] = c;
    new_buffer[len + 1] = '\0';

    obj->self = new_buffer;
    obj->length++;
}

// Combine strings
void combine_strings(string* obj, const char* str2) {
    int len1 = obj->length;
    int len2 = get_len((char*)str2);
    char* new_buffer = (char*)kmalloc(len1 + len2 + 1);
    if (!new_buffer) return;

    for (int i = 0; i < len1; i++) new_buffer[i] = obj->self[i];
    for (int i = 0; i < len2; i++) new_buffer[len1 + i] = str2[i];
    new_buffer[len1 + len2] = '\0';

    obj->self = new_buffer;
    obj->length += len2;
}

// Remove char by index
void remove_index(string* obj, int index) {
    if (!obj->self || index < 0 || index >= obj->length) return;
    for (int i = index; i < obj->length - 1; i++) obj->self[i] = obj->self[i + 1];
    obj->self[obj->length - 1] = '\0';
    obj->length--;
}

// Remove first occurrence of a char
void remove_char(string* obj, char c) {
    if (!obj->self) return;
    for (int i = 0; i < obj->length; i++) {
        if (obj->self[i] == c) {
            remove_index(obj, i);
            return;
        }
    }
}

// Truncate after a character
char* truncate_after_char(string* obj, char c) {
    if (!obj->self) return 0;
    for (int i = 0; i < obj->length; i++) {
        if (obj->self[i] == c) {
            obj->self[i] = '\0';
            obj->length = i;
            break;
        }
    }
    return obj->self;
}

// Truncate after index
void truncate_after_index(string* obj, int index) {
    if (!obj->self || index < 0 || index >= obj->length) return;
    obj->self[index] = '\0';
    obj->length = index;
}

// Return substring after a character
char* substring_after_char(string* obj, char c) {
    if (!obj->self) return 0;
    int idx = -1;
    for (int i = 0; i < obj->length; i++) {
        if (obj->self[i] == c) {
            idx = i;
            break;
        }
    }
    if (idx == -1 || idx + 1 >= obj->length) return "";
    static char buffer[512];
    int j = 0;
    for (int i = idx + 1; i < obj->length && j < 511; i++) {
        buffer[j++] = obj->self[i];
    }
    buffer[j] = '\0';
    return buffer;
}

// Compare strings
int compare_strings(string* a, string* b) {
    if (!a->self || !b->self) return 0;
    if (a->length != b->length) return 0;
    for (int i = 0; i < a->length; i++) {
        if (a->self[i] != b->self[i]) return 0;
    }
    return 1;
}

// Add integer to string
void add_integer_to(string* obj, int integer) {
    if (!obj) return;
    if (integer == 0) {
        add_char(obj, '0');
        return;
    }

    if (integer < 0) {
        add_char(obj, '-');
        integer = -integer;
    }

    char buffer[12];
    int index = 0;

    while (integer > 0) {
        buffer[index++] = (integer % 10) + '0';
        integer /= 10;
    }

    for (int i = index - 1; i >= 0; i--) add_char(obj, buffer[i]);
}
