#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <arch/x86/descriptor_tables.h>
#include <arch/x86/ports.h>

#define CONTROL 0x1
#define ALT 0x2
#define ALTGR 0x4
#define LSHIFT 0x8
#define RSHIFT 0x10
#define CAPSLOCK 0x20
#define SCROLLLOCK 0x40
#define NUMLOCK 0x80

#define RELEASED_MASK 0x80
//http://www.win.tue.nl/~aeb/linux/kbd/scancodes-10.html#ss10.6 (see set1)
// keyboard layouts:
uint8_t kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,					/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */ 
};

#define MAX_BUFFER 256
static uint8_t keydown[256];
static uint8_t buffer[MAX_BUFFER];
static int tail = 0, head=0;

static int last=0;

#define is_enter_bkspc(c) ((c) == '\n' || (c) == 8)
#define is_space(c)       ((c) == ' ')


void kbd_handler(registers_t *regs) {
	uint8_t scancode = inb(0x60);
	if (scancode & RELEASED_MASK) {
		scancode&=~RELEASED_MASK;
		keydown[scancode] = 0;
		last=scancode;
	} else {
		keydown[scancode] = 1;
		uint8_t c = kbdus[scancode];
		if (isdigit(c) || isalpha(c) || is_enter_bkspc(c) || is_space(c)){
			buffer[head] = c;
			head=(head+1)%MAX_BUFFER;
			if(tail==head) tail=(tail+1)%MAX_BUFFER;
		}
	}
}

int wait_scancode(){
	last=-1;
	int q=0;
	while(last==-1){q+=rand();}
	return last;
	
}


void kbd_clear_buffer(){
	tail=head=0;
}
void kbd_init(){
	kbd_clear_buffer();
	register_interrupt_handler(IRQ1, &kbd_handler);
}


uint8_t getchar()
{
	
	if(tail==head){return 0;}
	uint8_t c = buffer[tail];
	tail=(tail+1)%MAX_BUFFER;
	return c;
}

bool kbd_iskeydown(int scancode)
{
	return !!keydown[scancode];
}

bool kbd_iskeyup(int scancode)
{
	return !keydown[scancode];
}
