/*
 * fixed memory leak
*/

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

void memcpy_custom(char* dest, const char* src, int len) {
    for (int i = 0; i < len; i++) {
        dest[i] = src[i];
    }
}

static void ensure_capacity(string* obj, int min_capacity) {
    if (obj->capacity >= min_capacity) return;

    int new_capacity = obj->capacity ? obj->capacity * 2 : 16;
    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }

    char* new_buffer = (char*)kmalloc(new_capacity);
    if (!new_buffer) return;

    if (obj->self && obj->capacity > 0) {
        memcpy_custom(new_buffer, obj->self, obj->length);
        kfree(obj->self);
    }

    if (obj->length >= 0) {
        new_buffer[obj->length] = '\0';
    }

    obj->self = new_buffer;
    obj->capacity = new_capacity;
}

// Create a new string
void create(string* obj, char* str) {
    int len = get_len(str);

    obj->self = (char*)kmalloc(len + 1);  // +1 for null terminator
    if (!obj->self) {
        obj->length = 0;
        obj->capacity = 0;
        return;
    }

    memcpy_custom(obj->self, str, len);
    obj->self[len] = '\0';
    obj->length = len;
    obj->capacity = len + 1;
}

// Clear string
void clear_string(string* obj) {
    if (!obj->self || obj->capacity == 0) {
        obj->self = (char*)kmalloc(1);
        if (!obj->self) return;
        obj->capacity = 1;
    }
    obj->self[0] = '\0';
    obj->length = 0;
}

// Add a char
void add_char(string* obj, char c) {
    int len = obj->length;
    ensure_capacity(obj, len + 2);
    if (!obj->self) return;

    obj->self[len] = c;
    obj->self[len + 1] = '\0';
    obj->length++;
}

// Combine strings
void combine_strings(string* obj, const char* str2) {
    int len1 = obj->length;
    int len2 = get_len((char*)str2);
    ensure_capacity(obj, len1 + len2 + 1);
    if (!obj->self) return;

    memcpy_custom(obj->self + len1, str2, len2);
    obj->self[len1 + len2] = '\0';
    obj->length += len2;
}

// Remove char by index
void remove_index(string* obj, int index) {
    if (!obj->self || index < 0 || index >= obj->length) return;
    memcpy_custom(obj->self + index, obj->self + index + 1, obj->length - index - 1);
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
    int start = idx + 1;
    int len = obj->length - start;
    if (len > 511) len = 511;
    memcpy_custom(buffer, obj->self + start, len);
    buffer[len] = '\0';
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

// Check if string is empty
int is_empty_string(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ') return 0;
    }
    return 1;
}
