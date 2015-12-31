#pragma once

#define PAPYRUSUTIL_VERSION 32

#include "skse/PluginAPI.h"
#include "skse/GameAPI.h"

struct StaticFunctionTag;

namespace Plugin {
	void InitPlugin();
	bool RegisterFuncs(VMClassRegistry* registry);
	void HandleSKSEMessage(SKSEMessagingInterface::Message * msg);
	void Update();
	UInt32 GetVersion(StaticFunctionTag* base);
}
