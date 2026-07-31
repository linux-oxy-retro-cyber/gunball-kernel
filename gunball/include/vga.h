#ifndef VGA_H
#define VGA_H

#define VGA_ADDRESS 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25

// Cores VGA (Fundo Azul 0x10 + Texto)
#define COLOR_BLUE_SCREEN   0x1F
#define COLOR_WHITE_ON_BLUE 0x1F
#define COLOR_YELLOW_ON_BLUE 0x1E
#define COLOR_GREEN_ON_BLUE 0x1A

void clear_screen(void);
void print_string(int x, int y, const char* str, unsigned char color);
void print_dec(int x, int y, unsigned int num, unsigned char color);
void delay(int count);

#endif
