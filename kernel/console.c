#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <video.h>
#ifdef BOARDx86generic
#include <arch/x86/ports.h>
#endif

volatile uint32_t term_x;
volatile uint32_t term_y;

///
///  Determines if the screen needs to be scrolled, and scrolls.
///

void scroll() {
    if (term_y >= 25) {
        video_scroll(0, 24);
        term_y = 24;
    }
}

///  Better than textmode_write, it formats the output at a basic level.
void printc(unsigned char c) {
    if (c == 0x08 && term_x) {
        term_x--;
    } else if (c == 0x09) {
        term_x = (term_x+8) & ~(8-1);
    } else if (c == '\r') {
       term_x = 0;
    } else if (c == '\n') {
        #ifndef ARCHx86
        video_printchar(term_x, term_y, c);
        #endif
        term_x = 0;
        term_y++;
    } else if (c >= ' ') {
        video_printchar(term_x, term_y, c);
        term_x++;
    }
    if (term_x >= 80) {
        term_x = 0;
        term_y++;
    }
    // Scroll the screen if needed.
    scroll();
    // Move the hardware cursor.
    video_setcursor(term_x, term_y);
}

///  Prints a basic string
void print(const char *c) {
    int i = 0;
    while (c[i]) {
        printc(c[i++]);
    }
}
void console_printdiv() {
    #ifndef OPT_NO_ENCHANCED_LOGGING
    printf("------+---------------------------------");
    printf("----------------------------------------");  // 80 lines
    #endif
}
void console_clear() {
    video_clear();
}
/// Initialises the whole thing
void console_init() {
    #ifdef ARCHx86
        uint16_t offset;
        outb(0x3D4, 14);
        offset = inb(0x3D5) << 8;
        outb(0x3D4, 15);
        offset |= inb(0x3D5);
        term_x = offset % 80;
        term_y = offset / 80;
    #else
        term_x = 0;
        term_y = 0;
        console_clear();
    #endif
    video_setcursor(term_x, term_y);
}
