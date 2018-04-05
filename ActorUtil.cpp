#include "ActorUtil.h"

#include "PackageData.h"

namespace ActorUtil {

	void AddPackage(StaticFunctionTag* base, Actor* ActorRef, TESPackage* PackageRef, UInt32 priority, UInt32 flags){
		PackageData::Packages* Overrides = PackageData::GetPackages();
		if (Overrides) Overrides->AddPackage(ActorRef, PackageRef, priority, flags);
	}

	bool RemovePackage(StaticFunctionTag* base, Actor* ActorRef, TESPackage* PackageRef){
		PackageData::Packages* Overrides = PackageData::GetPackages();
		return Overrides ? Overrides->RemovePackage(ActorRef, PackageRef) : false;
	}

	UInt32 CountPackages(StaticFunctionTag* base, Actor* ActorRef){
		PackageData::Packages* Overrides = PackageData::GetPackages();
		return Overrides ? Overrides->CountPackages(ActorRef) : 0;
	}

	UInt32 ClearActor(StaticFunctionTag* base, Actor* ActorRef){
		PackageData::Packages* Overrides = PackageData::GetPackages();
		return Overrides ? Overrides->ClearActor(ActorRef) : 0;
	}

	UInt32 ClearPackage(StaticFunctionTag* base, TESPackage* PackageRef){
		PackageData::Packages* Overrides = PackageData::GetPackages();
		return Overrides ? Overrides->ClearPackage(PackageRef) : 0;
	}
}

#include "skse64/PapyrusNativeFunctions.h"

void ActorUtil::RegisterFuncs(VMClassRegistry* registry) {
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, Actor*, TESPackage*, UInt32, UInt32>("AddPackageOverride", "ActorUtil", AddPackage, registry));
	registry->SetFunctionFlags("ActorUtil", "AddPackageOverride", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, Actor*, TESPackage*>("RemovePackageOverride", "ActorUtil", RemovePackage, registry));
	registry->SetFunctionFlags("ActorUtil", "RemovePackageOverride", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, Actor*>("CountPackageOverride", "ActorUtil", CountPackages, registry));
	registry->SetFunctionFlags("ActorUtil", "CountPackageOverride", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, Actor*>("ClearPackageOverride", "ActorUtil", ClearActor, registry));
	registry->SetFunctionFlags("ActorUtil", "ClearPackageOverride", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, TESPackage*>("RemoveAllPackageOverride", "ActorUtil", ClearPackage, registry));
	registry->SetFunctionFlags("ActorUtil", "RemoveAllPackageOverride", VMClassRegistry::kFunctionFlag_NoWait);


}