#pragma once

#include <Windows.h>

#define START_ASM1(b) void * jumpAddress = NULL; _asm { mov jumpAddress, offset b##CodeStart
#define START_ASM2(b) jmp b##CodeEnd
#define START_ASM3(b) b##CodeStart:

#define END_ASM(b, addr1, addr2) b##CodeEnd: \
} \
	Asm::WriteJump(addr1, addr2, (int)jumpAddress);

namespace Asm{
	bool WriteJump(int addressFrom1, int addressFrom2, int addressTo);
}
