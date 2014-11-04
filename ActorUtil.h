#pragma once

#include "skse/PluginAPI.h"
#include "skse/GameAPI.h"
#include "skse/GameTypes.h"
#include "skse/GameData.h"
#include "skse/PapyrusVM.h"

struct StaticFunctionTag;

namespace ActorUtil {
	void RegisterFuncs(VMClassRegistry* registry);

	void AddPackage(StaticFunctionTag* base, Actor* ActorRef, TESPackage* PackageRef, UInt32 priority, UInt32 flags);
	bool RemovePackage(StaticFunctionTag* base, Actor* ActorRef, TESPackage* PackageRef);
	UInt32 CountPackages(StaticFunctionTag* base, Actor* ActorRef);
	UInt32 ClearActor(StaticFunctionTag* base, Actor* ActorRef);
	UInt32 ClearPackage(StaticFunctionTag* base, TESPackage* PackageRef);
}