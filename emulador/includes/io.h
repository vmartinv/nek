#ifndef IO_H
#define IO_H
#include <nestypes.h>

struct Controls{
	enum names{A=0,B,SELECT,START,UP,DOWN,LEFT,RIGHT,CANTCONTROLS} ;
	int scancodes[CANTCONTROLS];
	u8 read();
};

namespace IO {
	extern Controls player_buttons[2];
	void Init();
	u8 JoyRead(unsigned idx);
	void JoyStrobe(unsigned v);
	void JoyUpdate();
	void PutPixel(unsigned px,unsigned py, unsigned pixel, int offset);
	void FlushScanline(unsigned py);
}

#endif
