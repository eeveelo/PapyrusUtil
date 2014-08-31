#include "PapyrusUtil.h"

#include "skse/PapyrusVM.h"
#include "skse/PapyrusArgs.h"
#include "skse/PapyrusNativeFunctions.h"

#include "skse/GameData.h"

template <> void UnpackValue(VMArray<TESForm*> * dst, VMValue * src){
	UnpackArray(dst, src, GetTypeIDFromFormTypeID(TESForm::kTypeID, (*g_skyrimVM)->GetClassRegistry()) | VMValue::kType_Identifier);
}

#include "Serialize.h"

#include "StorageUtil.h"
#include "JsonUtil.h"
#include "ActorUtil.h"
#include "ObjectUtil.h"
#include "MiscUtil.h"

namespace PapyrusUtil {

	UInt32 GetVersion(StaticFunctionTag* base) {
		return PAPYRUSUTIL_VERSION;
	}

	bool RegisterFuncs(VMClassRegistry* registry) {
		// PapyrusUtil.psc
		registry->RegisterFunction(new NativeFunction0<StaticFunctionTag, UInt32>("GetVersion", "PapyrusUtil", GetVersion, registry));
		registry->SetFunctionFlags("PapyrusUtil", "GetVersion", VMClassRegistry::kFunctionFlag_NoWait);
		// StorageUtil.psc
		StorageUtil::RegisterFuncs(registry);
		// JsonUtil.psc
		JsonUtil::RegisterFuncs(registry);
		// ActorUtil.psc
		ActorUtil::RegisterFuncs(registry);
		// ObjectUtil.psc
		ObjectUtil::RegisterFuncs(registry);
		// MiscUtil.psc
		MiscUtil::RegisterFuncs(registry);

		return true;
	}

	void InitPlugin(){
		// package override patch
		ActorUtil::InitPlugin();
		// idle override patch
		ObjectUtil::InitPlugin();
		// run bat patch
		MiscUtil::InitPlugin();
	}

}

