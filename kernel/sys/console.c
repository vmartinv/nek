#include <types.h>
#include <stdio.h>
#ifdef BOARDx86generic
#include <arch/x86/ports.h>
#include <arch/x86/textmode_console.h>
#include <graphics/video.h>
#endif

volatile uint32_t term_x;
volatile uint32_t term_y;
unsigned lines, cols;

///
///  Determines if the screen needs to be scrolled, and scrolls.
///

inline static void scroll() {
    if (term_y >= lines) {
        textmode_scroll(0, lines-1);
        term_y = lines-1;
    }
}

void console_move_write_cursor(int x, int y){
	term_x=x, term_y=y;
}

///  Better than textmode_write, it formats the output at a basic level.
void putchar(unsigned char c) {
    if (c == 0x08 && term_x) {
        term_x--;
    } else if (c == 0x09) {
        term_x = (term_x+8) & ~7;
    } else if (c == '\r') {
       term_x = 0;
    } else if (c == '\n') {
        #ifndef ARCHx86
        textmode_write(term_x, term_y, c);
        #endif
        term_x = 0;
        term_y++;
    } else if (c >= ' ') {
        textmode_write(term_x, term_y, c);
        term_x++;
    }
    if (term_x >= cols) {
        term_x = 0;
        term_y++;
    }
    // Scroll the screen if needed.
    scroll();
    // Move the hardware cursor.
    textmode_setcursor(term_x, term_y);
    //~ video_show_console();
}

void putchar_now(unsigned char c) {
	int x=term_x, y=term_y;
	putchar(c);
	video_flush_char(x, y);
}

///  Prints a basic string
inline static void print(const char *c) {
    while(*c)
        putchar(*c++);
}

void console_printdiv() {
    #ifndef OPT_NO_ENCHANCED_LOGGING
    for(unsigned i=0; i<cols; i++)
		putchar('-');
    #endif
}
void console_clear() {
    textmode_clear();
    term_x = term_y = 0;
    textmode_setcursor(0, 0);
}
/// Initialises the whole thing
void console_init() {
#ifdef CONSOLE_ONLY
		lines=25, cols=80;
		textmode_init((uint16_t*)0xB8000, lines, cols);
#else
		lines=video_get_lines(), cols=video_get_cols();
		textmode_init(video_get_text_buffer(), lines, cols);
#endif

#ifdef ARCHx86
  #ifdef CONSOLE_ONLY
        uint16_t offset;
        outb(0x3D4, 14);
        offset = inb(0x3D5) << 8;
        outb(0x3D4, 15);
        offset |= inb(0x3D5);
        term_x = offset % cols;
        term_y = offset / cols;
   #else
        term_x = term_y = 0;
        console_clear();
  #endif
#endif
    textmode_setcursor(term_x, term_y);
}
