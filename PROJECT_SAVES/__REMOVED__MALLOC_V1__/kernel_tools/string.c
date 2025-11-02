#include "including/string.h"
#include "including/vega.h"   // for write / debug if needed

int get_len(const char* str) {
    int i = 0;
    while (str[i] != '\0') i++;
    return i;
}

char* get_string(string* obj) { return obj->self; }

int compare_strings(string* a, string* b) {
    if (a->length != b->length) return 0;
    for (int i = 0; i < a->length; i++) {
        if (a->self[i] != b->self[i]) return 0;
    }
    return 1;
}

void create(string* obj, const char* str) {
    int len = get_len(str);
    obj->self = (char*)my_malloc(len + 1);
    if (!obj->self) {
        // fatal: no memory
        write("ERROR: malloc failed in create\n");
        return;
    }
    obj->length = len;
    for (int i = 0; i < len; i++) obj->self[i] = str[i];
    obj->self[len] = '\0';
}


void clear_string(string* obj) {
    if (obj->self) obj->self[0] = '\0';
    obj->length = 0;
}

void free_string(string* obj) {
    if (obj->self) {
        my_free(obj->self);
        obj->self = 0;
        obj->length = 0;
    }
}

void add_char(string* obj, char c) {
    char* newbuf = (char*)my_malloc(obj->length + 2); // +1 char + '\0'
    for (int i = 0; i < obj->length; i++) newbuf[i] = obj->self[i];
    newbuf[obj->length] = c;
    newbuf[obj->length + 1] = '\0';

    my_free(obj->self);
    obj->self = newbuf;
    obj->length++;
}

void combine_strings(string* obj, const char* str2) {
    int add_len = get_len(str2);
    char* newbuf = (char*)my_malloc(obj->length + add_len + 1);

    for (int i = 0; i < obj->length; i++) newbuf[i] = obj->self[i];
    for (int j = 0; j < add_len; j++) newbuf[obj->length + j] = str2[j];
    newbuf[obj->length + add_len] = '\0';

    my_free(obj->self);
    obj->self = newbuf;
    obj->length += add_len;
}

void add_integer_to(string* obj, int integer) {
    char buffer[12]; // enough for 32-bit int
    int idx = 0;

    if (integer == 0) buffer[idx++] = '0';
    else {
        if (integer < 0) {
            buffer[idx++] = '-';
            integer = -integer;
        }
        char digits[11];
        int d = 0;
        while (integer > 0) {
            digits[d++] = (integer % 10) + '0';
            integer /= 10;
        }
        for (int i = d - 1; i >= 0; i--) buffer[idx++] = digits[i];
    }
    buffer[idx] = '\0';
    combine_strings(obj, buffer);
}

int index_inof(string* obj, char target) {
    for (int i = 0; i < obj->length; i++) {
        if (obj->self[i] == target) return i;
    }
    return -1;
}

char* truncate_after_char(string* obj, char c) {
    int idx = index_inof(obj, c);
    if (idx != -1) {
        obj->self[idx] = '\0';
        obj->length = idx;
    }
    return obj->self;
}

char* substring_after_char(string* obj, char c) {
    int idx = index_inof(obj, c);
    if (idx == -1 || idx + 1 >= obj->length) return "";

    int newlen = obj->length - (idx + 1);
    char* buf = (char*)my_malloc(newlen + 1);
    for (int i = 0; i < newlen; i++) buf[i] = obj->self[idx + 1 + i];
    buf[newlen] = '\0';
    return buf; // caller should free this
}

void truncate_after_index(string* obj, int index) {
    if (index >= 0 && index < obj->length) {
        obj->self[index] = '\0';
        obj->length = index;
    }
}

void remove_char(string* obj, char c) {
    int i = index_inof(obj, c);
    if (i != -1) remove_index(obj, i);
}

void remove_index(string* obj, int index) {
    if (index < 0 || index >= obj->length) return;

    for (int i = index; i < obj->length - 1; i++) {
        obj->self[i] = obj->self[i + 1];
    }
    obj->self[obj->length - 1] = '\0';
    obj->length--;
}
