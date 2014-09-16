#ifdef BOARDx86generic
#include <arch/x86/textmode_console.h>
#else
#include <arch/arm/serial.h>
#endif
#include <stdint.h>
void video_printchar(int x, int y, unsigned char c) {
    #ifdef ARCHx86
    textmode_write(x, y, c);
    #endif
    #ifdef ARCHarm
    serial_write(c);
    #endif
}

void video_printcoloredchar(int x, int y,
    unsigned char c, unsigned char attribute) {
    #ifdef BOARDx86generic
    textmode_write_color(x, y, c, attribute);
    #endif
}

void video_printstring(int x, int y, char *c) {
    int i = 0;
    while (c[i]) {
        video_printchar(x + i, y, c[i]);
        i++;
    }
}

void video_printcoloredstring(int x, int y, unsigned char attribute, char *c) {
    int i = 0;
    while (c[i]) {
        video_printcoloredchar(x + i, y , c[i], attribute);
        i++;
    }
}

void video_scroll(int from, int to) {
    #ifdef BOARDx86generic
    textmode_scroll(from, to);
    #endif
}

void video_setcursor(int x, int y) {
    #ifdef BOARDx86generic
    textmode_setcursor(x, y);
    #endif
}

void video_settextfore(uint8_t color) 
{
    #ifdef BOARDx86generic
    extern uint8_t tm_cback;
    textmode_setcolor(tm_cback, color);
    #endif
}
void video_reset_attr() {
    #ifdef BOARDx86generic
    textmode_resetcolor();
    #endif
}

void video_clear() {
    #ifdef BOARDx86generic
    textmode_clear();
    #endif
}
