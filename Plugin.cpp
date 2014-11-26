#include "Plugin.h"

#include "skse/PapyrusVM.h"
#include "skse/PapyrusArgs.h"
#include "skse/PapyrusNativeFunctions.h"
#include "skse/GameData.h"

template <> void UnpackValue(VMArray<TESForm*> * dst, VMValue * src){
	UnpackArray(dst, src, GetTypeIDFromFormTypeID(TESForm::kTypeID, (*g_skyrimVM)->GetClassRegistry()) | VMValue::kType_Identifier);
}

template <> void UnpackValue(VMArray<Actor*> * dst, VMValue * src){
	UnpackArray(dst, src, GetTypeIDFromFormTypeID(Actor::kTypeID, (*g_skyrimVM)->GetClassRegistry()) | VMValue::kType_Identifier);
}

template <> void UnpackValue(VMArray<TESObjectREFR*> * dst, VMValue * src){
	UnpackArray(dst, src, GetTypeIDFromFormTypeID(TESObjectREFR::kTypeID, (*g_skyrimVM)->GetClassRegistry()) | VMValue::kType_Identifier);
}

#include "PackageData.h"

#include "Serialize.h"
#include "PapyrusUtil.h"
#include "StorageUtil.h"
#include "JsonUtil.h"
#include "ActorUtil.h"
#include "ObjectUtil.h"
#include "MiscUtil.h"

namespace Plugin {

	UInt32 GetVersion(StaticFunctionTag* base) {
		return PAPYRUSUTIL_VERSION;
	}

	void InitPlugin(){
		PackageData::InitPlugin();
		ObjectUtil::InitPlugin();
	}

	bool RegisterFuncs(VMClassRegistry* registry) {
		registry->RegisterFunction(new NativeFunction0<StaticFunctionTag, UInt32>("GetVersion", "PapyrusUtil", GetVersion, registry));
		registry->SetFunctionFlags("PapyrusUtil", "GetVersion", VMClassRegistry::kFunctionFlag_NoWait);

		PapyrusUtil::RegisterFuncs(registry);
		StorageUtil::RegisterFuncs(registry);
		JsonUtil::RegisterFuncs(registry);
		ActorUtil::RegisterFuncs(registry);
		ObjectUtil::RegisterFuncs(registry);
		MiscUtil::RegisterFuncs(registry);

		return true;
	}

}

