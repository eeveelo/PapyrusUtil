#include "Plugin.h"

#include "skse64_common\BranchTrampoline.h"
#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusArgs.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64/GameData.h"

/*
template <> void UnpackValue(VMArray<TESForm*> * dst, VMValue * src){
	UnpackArray(dst, src, GetTypeIDFromFormTypeID(TESForm::kTypeID, (*g_skyrimVM)->GetClassRegistry()) | VMValue::kType_Identifier);
}

template <> void UnpackValue(VMArray<Actor*> * dst, VMValue * src){
	UnpackArray(dst, src, GetTypeIDFromFormTypeID(Actor::kTypeID, (*g_skyrimVM)->GetClassRegistry()) | VMValue::kType_Identifier);
}

template <> void UnpackValue(VMArray<TESObjectREFR*> * dst, VMValue * src){
	UnpackArray(dst, src, GetTypeIDFromFormTypeID(TESObjectREFR::kTypeID, (*g_skyrimVM)->GetClassRegistry()) | VMValue::kType_Identifier);
}*/

#include "PackageData.h"

#include "Serialize.h"
#include "PapyrusUtil.h"
#include "StorageUtil.h"
#include "JsonUtil.h"
#include "ActorUtil.h"
//#include "ObjectUtil.h"
#include "MiscUtil.h"
//#include "AnimUtil.h"
//#include "Asm.h"


namespace Plugin {

	//int frameUpdate1 = 0x69CC2E;
	//int frameUpdate2 = 0x69CC34;

	UInt32 GetVersion(StaticFunctionTag* base) {
		return PAPYRUSUTIL_VERSION;
	}

	void InitPlugin(){
		_MESSAGE("Init...");

		if (!g_branchTrampoline.Create(1024 * 64))
		{
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return;
		}

		if (!g_localTrampoline.Create(1024 * 64, GetModuleHandle("PapyrusUtil.dll")))
		{
			_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return;
		}


		//_MESSAGE("Plugin::InitPlugin() - 1");
		
		//if (!InitAddress()) {
		//	_ERROR("Failed to load address library!");
		//}

		PackageData::InitPlugin();
		
		//ObjectUtil::InitPlugin();
		//AnimUtil::InitPlugin();


		_MESSAGE("-done");

	}


	bool RegisterFuncs(VMClassRegistry* registry) {
		_MESSAGE("Registering functions...");
		registry->RegisterFunction(new NativeFunction0<StaticFunctionTag, UInt32>("GetVersion", "PapyrusUtil", GetVersion, registry));
		registry->SetFunctionFlags("PapyrusUtil", "GetVersion", VMClassRegistry::kFunctionFlag_NoWait);

		PapyrusUtil::RegisterFuncs(registry);
		StorageUtil::RegisterFuncs(registry);
		JsonUtil::RegisterFuncs(registry);
		ActorUtil::RegisterFuncs(registry);
		//ObjectUtil::RegisterFuncs(registry);
		MiscUtil::RegisterFuncs(registry);
		//AnimUtil::RegisterFuncs(registry);
		_MESSAGE("-done");
		return true;
	}

	void HandleSKSEMessage(SKSEMessagingInterface::Message * msg) {
		/*_MESSAGE("HandleSKSEMessage - 1");
		if (!msg) return;

		bool GameChanged(msg->type == SKSEMessagingInterface::kMessage_NewGame || msg->type == SKSEMessagingInterface::kMessage_PostLoadGame);
		if (GameChanged){
			AnimUtil::ClearAll();
		}
		_MESSAGE("HandleSKSEMessage - 2");*/
	}



}

