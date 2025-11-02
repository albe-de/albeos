#define VGA_MEMORY ((volatile char*) 0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR_WHITE 0x0F

int lines = 0;
int x = 1;
int y = 1;

/* writes char to vega buffer */
void vga_put_char(int x, int y, char c, char color) {
    volatile char* vga = VGA_MEMORY + (y * VGA_WIDTH + x) * 2;
    *vga = c;            // character
    *(vga + 1) = color;  // attribute (color)
}

void clear_screen(){
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_put_char(x, y, ' ', VGA_COLOR_WHITE);
        }
    }

    lines, y = 0, 1;
}

void write(const char* message){
    for (const char* p = message; *p != '\0'; p++) {
        vga_put_char(x++, y, *p, VGA_COLOR_WHITE);
    }
    lines++;
    y+=1;
    x=1;
}
