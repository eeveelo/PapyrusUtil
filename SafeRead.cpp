#include "SafeRead.h"

#include "skse64/GameData.h"
#include "skse64/GameTypes.h"

namespace Lib {
	UInt32 SafeRead32(UInt32 addr){
		UInt32 oldProtect;
		UInt32 returnValue;
		try {
			VirtualProtect((void *)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
			returnValue = *((UInt32 *)addr);
			VirtualProtect((void *)addr, 4, oldProtect, &oldProtect);
		}
		catch (std::exception&){
			_MESSAGE("SafeRead32 Exception: %d", (int)addr);
		}
		return returnValue;
	}

	char SafeRead8(UInt32 addr){
		UInt32 oldProtect;
		char returnValue;

		try{
			VirtualProtect((void *)addr, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			returnValue = *((char *)addr);
			VirtualProtect((void *)addr, 1, oldProtect, &oldProtect);
		}
		catch (std::exception&){
			_MESSAGE("SafeRead8 Exception: %d", (int)addr);
		}

		return returnValue;
	}
}
