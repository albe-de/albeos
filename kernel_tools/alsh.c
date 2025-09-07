#include "including/alsh.h"
#include "including/vega.h"

int num_commands = 0;
char *commands[] = {
    "move",
    "list",
    "mkfile",
    "rmfile",
    "-help"
};

int innit_alsh(){
    char **current_command = commands;
    while (*current_command) {
        num_commands++;
        current_command++;
    }

    num_commands-=2;
    return num_commands;
}

int compare_string(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    // Return the difference of the characters at the mismatch point, or 0 if equal
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}


int compare_cmd(char* command){
    for (int i = 0; i < num_commands; i++) {
        if (compare_string(command, commands[i]) == 0) return 1; // Match found
    }
    return 0; // No match found
}
