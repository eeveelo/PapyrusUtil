#pragma once

#define PAPYRUSUTIL_VERSION 28

#include "skse/PluginAPI.h"
#include "skse/GameAPI.h"

struct StaticFunctionTag;

namespace Plugin {
	void InitPlugin();
	bool RegisterFuncs(VMClassRegistry* registry);
	UInt32 GetVersion(StaticFunctionTag* base);
}
