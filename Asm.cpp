#include "Asm.h"

namespace Asm{
	bool WriteJump(int addressFrom1, int addressFrom2, int addressTo){
		DWORD oldProtect = 0;

		int len1 = addressFrom2 - addressFrom1;
		if (VirtualProtect((void *)addressFrom1, len1, PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			*((unsigned char *)addressFrom1) = (unsigned char)0xE9;
			*((int *)(addressFrom1 + 1)) = (int)addressTo - addressFrom1 - 5;
			for (int i = 5; i < len1; i++)
				*((unsigned char *)(i + addressFrom1)) = (unsigned char)0x90;
			if (VirtualProtect((void *)addressFrom1, len1, oldProtect, &oldProtect))
				return true;
		}

		_MESSAGE("WriteJump(0x%X, 0x%X, 0x%X) failed!", addressFrom1, addressFrom2, addressTo);
		return false;
	}
}
