#include "vga.h"

unsigned short* vga_buffer = (unsigned short*)VGA_ADDRESS;

void delay(int count) {
    volatile int i;
    for (i = 0; i < count * 50000; i++) {
        __asm__ __volatile__("nop");
    }
}

void clear_screen(void) {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        vga_buffer[i] = (COLOR_BLUE_SCREEN << 8) | ' ';
    }
}

void print_string(int x, int y, const char* str, unsigned char color) {
    int index = y * VGA_COLS + x;
    while (*str) {
        vga_buffer[index] = (color << 8) | (*str);
        str++;
        index++;
    }
}

void print_dec(int x, int y, unsigned int num, unsigned char color) {
    char str[16];
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
    } else {
        while (num > 0) {
            str[i++] = (num % 10) + '0';
            num /= 10;
        }
    }
    str[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }

    print_string(x, y, str, color);
}
