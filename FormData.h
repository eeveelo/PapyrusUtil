#pragma once

#include "common/ICriticalSection.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameReferences.h"

namespace FormData {
	class Store {
	private:
		ICriticalSection s_dataLock;
		TESForm* obj;
	public:

	};
}