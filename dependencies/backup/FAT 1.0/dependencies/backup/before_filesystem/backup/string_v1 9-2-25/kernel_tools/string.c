#include "including/string.h"

/**
 *
 * string manager for Albe's OS
 * 09/02/25
 *
 */

char* get_string(string* obj) {return obj->self;}
int get_len(char* message) {
    int i = 0;
    for (const char* p = message; *p != '\0'; p++) i++;
    return i;
}

// gets index in obj of char
int index_inof(string* obj, char target) {
    int i = 0;
    for (const char* chr = obj->self; *chr != '\0'; chr++) {
        if (*chr == target) return i;
        i++;
    }
    return -1;
}

void add_char(string* obj, char c) {
    int len = obj->length;

    if (len < MAX_STRING_SIZE - 1) {
        obj->self[len] = c;
        obj->self[len + 1] = '\0';
        obj->length++;
    }
}

void combine_strings(string* obj, const char* str2){
    for (str2; *str2 != '\0'; str2++) {
        add_char(obj, *str2);
    }
}


void add_integer_to(string* obj, int integer) {
    if (integer == 0) {
        add_char(obj, '0');
        return;
    }

    // negative integers
    if (integer < 0) {
        add_char(obj, '-');
        integer = -integer; // Make it positive for further processing
    }

    // 32 bit integers
    char buffer[12];
    int index = 0;

    // Extract digits in reverse order
    while (integer > 0) {
        buffer[index++] = (integer % 10) + '0'; // Convert digit to character
        integer /= 10;
    }

    for (int i = index - 1; i >= 0; i--) {
        add_char(obj, buffer[i]);
    }
}

void remove_char(string* obj, char c) {
    int len = obj->length;
    int found = index_inof(obj, c);

    if (found != -1) {
        for (int i = found; i < len - 1; i++) {
            obj->self[i] = obj->self[i + 1]; // Shift characters left
        }
        obj->self[len - 1] = '\0'; // Null terminate the string
        obj->length--;
    }
}

void remove_index(string* obj, int index) {
    if (index < 0 || index >= obj->length) return;
    int len = obj->length;

    for (int i = index; i < len - 1; i++) {
        obj->self[i] = obj->self[i + 1]; // Shift characters left
    }
    obj->self[len - 1] = '\0'; // Null terminate the string
    obj->length--;
}

void clear_string(string* obj) {
    obj->length = 0; // Set length to 0
    obj->self[0] = '\0'; // Set the first character to null terminator
}

/**
 * @brief Creates and initializes a string object by dynamically allocating memory.
 *
 * @param obj A pointer to the string object to be initialized.
 * @param str const char* -> the actual string
 */
void create(string* obj, char* str) {
    int len = get_len(str);  // Get the length of the string
    if (len < MAX_STRING_SIZE) { // Ensure it fits in the fixed-size array
        for (int i = 0; i < len; i++) {
            obj->self[i] = str[i];
        }
        obj->self[len] = '\0'; // Null terminate
        obj->length = len;
    }
}


/* EXAMPLE USAGE /*

string st2;

write("Index of 'o' in the string '");
create(&st2, "feifsodvf' is: ");
add_integer_to(&st2, index_inof((&st2), 'o'));
write(get_string(&st2));
string buffer1 = get_buffer();
enter_line();

write("After removing index 5, ");
remove_index(&st2, 5);
write(get_string(&st2));
enter_line();

write("After removing char 's', ");
remove_char(&st2, 's');
write(get_string(&st2));
string buffer2 = get_buffer();
enter_line();
enter_line();

write(get_string(&buffer1));
enter_line();
write(get_string(&buffer2));

*/
