#ifndef APU_H
#define APU_H
#include <nestypes.h>

namespace APU{
	void tick();
	void Write(u8 index, u8 value);
	u8 Read();
}
#endif
