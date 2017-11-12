#pragma once

#include "skse64/PluginAPI.h"
#include "skse64/GameAPI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameData.h"
#include "skse64/PapyrusVM.h"

struct StaticFunctionTag;

namespace ActorUtil {
	void RegisterFuncs(VMClassRegistry* registry);

	void AddPackage(StaticFunctionTag* base, Actor* ActorRef, TESPackage* PackageRef, UInt32 priority, UInt32 flags);
	bool RemovePackage(StaticFunctionTag* base, Actor* ActorRef, TESPackage* PackageRef);
	UInt32 CountPackages(StaticFunctionTag* base, Actor* ActorRef);
	UInt32 ClearActor(StaticFunctionTag* base, Actor* ActorRef);
	UInt32 ClearPackage(StaticFunctionTag* base, TESPackage* PackageRef);
}