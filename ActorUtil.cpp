#include "ActorUtil.h"

#include "skse/PluginAPI.h"
#include "skse/GameAPI.h"
#include "skse/GameData.h"
#include "skse/PapyrusNativeFunctions.h"

#include "skse/GameRTTI.h"
#include "skse/SafeWrite.h"

#include "SafeRead.h"
#include "Forms.h"
#include "Data.h"

namespace ActorUtil {
	typedef Data::Lists<TESForm*, UInt32> Packages;

	/*
	 *
	 *  Package override handling
	 *
	 */

	int endPack = 0;
	int endActor = 0;
	void PackageEnded(){
		Packages* Overrides = Data::GetPackages();
		if (!Overrides || Overrides->Data.size() == 0 || endPack == 0 || endActor == 0)
			return;

		TESForm* ActorRef = LookupFormByID(endActor);
		TESForm* FormRef = LookupFormByID(endPack);
		TESPackage* PackageRef = NULL;
		if (FormRef && FormRef->formType != kFormType_Package)
			PackageRef = DYNAMIC_CAST(FormRef, TESForm, TESPackage);
		if (!PackageRef)
			return;

		UInt64 obj = Forms::GetFormKey(ActorRef);
		if (Overrides->ListHas(obj, "package", PackageRef)){
			_MESSAGE("Override ended: %d", PackageRef->formID);
			Overrides->ListRemove(obj, "package", PackageRef, true);
			Overrides->ListRemove(obj, "flagged", PackageRef, true);
		}
		endActor = 0;
		endPack = 0;
	}

	bool IsValidPackage(int PackageID, int ActorID) {
		int valid = 0;
		int funcCall = 0x05E1F40;
		_asm
		{
			mov ecx, PackageID
			push ActorID
			call funcCall
			mov valid, eax
		}
		return valid != 0;
	}

	UInt32 DecidePackage(int ActorID, int PackageID){
		Packages* Overrides = Data::GetPackages();
		if (!Overrides || Overrides->Data.size() == 0 || ActorID == 0)
			return PackageID;

		UInt64 obj = Forms::GetFormKey((TESForm*)ActorID);
		Packages::List* package = Overrides->GetVector(obj, std::string("package"));
		if (package != NULL){
			for (Packages::List::reverse_iterator itr = package->rbegin(); itr != package->rend(); itr++){
				int pid = Forms::GameGetForm(*itr);
				TESForm* form = (TESForm*)pid;
				if (form != NULL && form->formType == kFormType_Package){
					if (Overrides->ListHas(obj, "flagged", form) || IsValidPackage(pid, ActorID)) {
						_MESSAGE("Override decided: %d", pid);
						PackageID = pid;
						break;
					}
				}
			}
		}
		return PackageID;
	}

	int endCall = 0xA51280;
	int endBack = 0x8CAB59;
	int packageFunc = 0x40E4F0;
	int jumpPackage = 0x6A9E95;
	void InitPlugin() {
		void * packageCodeStart;
		_asm
		{
			mov packageCodeStart, offset packageStart
			jmp packageEnd
		packageStart :
			call packageFunc
			cmp esi, 0
			je packageOrig
			push eax
			push esi
			call DecidePackage
			add esp, 8
		packageOrig:
			jmp jumpPackage
		packageEnd :
		}

		WriteRelJump(0x6A9E90, (UInt32)packageCodeStart);

		void * endPackageCodeStart;
		_asm
		{
			mov endPackageCodeStart, offset endPackageStart
			jmp endPackageEnd
		endPackageStart :
			push eax
			push ecx
			push edx
			mov eax, [edi]
			mov endActor, eax
			mov eax, [edi + 4]
			mov endPack, eax
			call PackageEnded
			pop edx
			pop ecx
			pop eax
		//endPackageOrig:
			call endCall
			jmp endBack
		endPackageEnd :
		}

		WriteRelJump(0x8CAB54, (UInt32)endPackageCodeStart);
	}

	/*
	 *
	 *  Papyrus Interface: ActorUtil.psc
	 *
	 */

	void AddPackage(StaticFunctionTag* base, Actor* ActorRef, TESPackage* PackageRef, UInt32 priority, UInt32 flags){
		if (!ActorRef || !PackageRef)
			return;
		UInt64 obj = Forms::GetFormKey(ActorRef);
		Packages* Overrides = Data::GetPackages();
		Overrides->ListAdd(obj, "package", PackageRef, false);
		if (flags == 1)
			Overrides->ListAdd(obj, "flagged", PackageRef, false);
	}

	bool RemovePackage(StaticFunctionTag* base, Actor* ActorRef, TESPackage* PackageRef){
		if (!ActorRef || !PackageRef)
			return false;
		UInt64 obj = Forms::GetFormKey(ActorRef);
		Packages* Overrides = Data::GetPackages();
		Overrides->ListRemove(obj, "flagged", PackageRef, true);
		return Overrides->ListRemove(obj, "package", PackageRef, true) != 0;
	}

	UInt32 CountPackages(StaticFunctionTag* base, Actor* ActorRef){
		if (!ActorRef)
			return 0;
		Packages* Overrides = Data::GetPackages();
		if (!Overrides || Overrides->Data.size() == 0)
			return 0;
		return Overrides->ListCount(Forms::GetFormKey(ActorRef), "package");
	}

	UInt32 ClearActor(StaticFunctionTag* base, Actor* ActorRef){
		if (!ActorRef)
			return 0;

		Packages* Overrides = Data::GetPackages();
		if (!Overrides || Overrides->Data.size() == 0)
			return 0;

		int removed = 0;
		Overrides->s_dataLock.Enter();
		Packages::Map::iterator itr = Overrides->Data.find(Forms::GetFormKey(ActorRef));
		if (itr != Overrides->Data.end()){
			removed = itr->second["packages"].size();
			Overrides->Data.erase(itr);
		}
		Overrides->s_dataLock.Leave();
		return removed;
	}

	UInt32 ClearPackage(StaticFunctionTag* base, TESPackage* PackageRef){
		if (!PackageRef)
			return 0;

		Packages* Overrides = Data::GetPackages();
		if (!Overrides || Overrides->Data.size() == 0)
			return 0;

		int removed = 0;
		for (Packages::Map::iterator itr = Overrides->Data.begin(); itr != Overrides->Data.end(); ++itr){
			removed += Overrides->ListRemove(itr->first, "package", PackageRef, true);
			Overrides->ListRemove(itr->first, "flagged", PackageRef, true);
		}
		return removed;
	}

	void RegisterFuncs(VMClassRegistry* registry) {
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
}