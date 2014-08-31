#pragma once

#define PAPYRUSUTIL_VERSION 27

#include "skse/PluginAPI.h"
#include "skse/GameAPI.h"

struct StaticFunctionTag;

namespace PapyrusUtil {
	UInt32 GetVersion(StaticFunctionTag* base);
	bool RegisterFuncs(VMClassRegistry* registry);
	void InitPlugin();
}
