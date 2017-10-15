#pragma once

#define PAPYRUSUTIL_VERSION 33

#include "skse/PluginAPI.h"
#include "skse/GameAPI.h"

struct StaticFunctionTag;

namespace Plugin {
	void InitPlugin();
	bool RegisterFuncs(VMClassRegistry* registry);
	void HandleSKSEMessage(SKSEMessagingInterface::Message * msg);
	UInt32 GetVersion(StaticFunctionTag* base);
}
