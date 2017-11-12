#include "ObjectUtil.h"

#include "skse64/PluginAPI.h"
#include "skse64/GameAPI.h"
#include "skse64/GameData.h"
#include "skse64/GameRTTI.h"
#include "skse64/SafeWrite.h"
#include "skse64/PapyrusNativeFunctions.h"

#include "SafeRead.h"
#include "Data.h"
#include "Forms.h"

#include <boost/algorithm/string.hpp>

namespace ObjectUtil {
	typedef Data::Values<TESForm*, UInt32> Idles;

	Idles* s_Idles = NULL;
	Idles* GetSingleton(){
		if (s_Idles == NULL)
			s_Idles = new Idles();
		return s_Idles;
	}


	/*
	*
	*  Idle override patching
	*
	*/

	const char* AnimEvent = NULL;
	void DecideAnimReplace(int a1){
		if (!s_Idles || a1 == 0)
			return;
		AnimEvent = NULL;
		int obj = Lib::SafeRead32(a1 + 4);
		if (obj == 0)
			return;
		UInt64 key = Forms::GetFormKey((TESForm*)obj);
		if (key == 0)
			return;

		// Look for a replacer
		std::string AnimEventName((const char*)Lib::SafeRead32(a1 + 0x14));
		TESForm* FormRef = GetSingleton()->GetValue(key, AnimEventName, NULL);
		if (!FormRef)
			FormRef = GetSingleton()->GetValue(0, AnimEventName, NULL);
		if (!FormRef)
			return;

		// Get the Idle object to use
		TESIdleForm* IdleRef = NULL;
		if (FormRef && FormRef->formType == kFormType_Idle)
			IdleRef = DYNAMIC_CAST(FormRef, TESForm, TESIdleForm);
		if (!IdleRef)
			return;

		// Override
		//_MESSAGE(IdleRef->animationEvent.data);
		SafeWrite32(a1 + 0x24, (UInt32)IdleRef);
		AnimEvent = IdleRef->animationEvent.data;
		SafeWrite32(a1 + 0x14, (UInt32)AnimEvent);
		/*
		Normal a1								Modified a1
		+0		vtable
		+4		actor
		+8		0
		+C		BGSAction (e.g. ActionIdle)				?BGSAction(Idle)
		+10		2
		+14		AnimationEventName*						AnimationEventName *
		+18		AnimationParams?
		+1C		0
		+20		0
		+24		TESIdleForm*
		+28		0
		+2C		1
		+30		Return address of function?
		+34		0
		+38		40h
		+3C		0
		+40		1	Animation handler?
		+44		0
		+48		0
		+4C		Unk*
		+50		Script* <- dynamically allocated
		+54		0
		+58		Animation event NOT POINTER <- this is not the case when BGSAction is not idle
		+... 	0
		*/
	}

	int animJNEBack = 0x6F0305;
	int animJEBack = 0x6F0312;
	void InitPlugin() {
		
		void * animCodeStart;
		_asm
		{
			mov animCodeStart, offset animStart
			jmp animEnd
		animStart :
			push eax
			push ecx
			push edx
			push esi
			call DecideAnimReplace
			add esp, 4
			pop edx
			pop ecx
			pop eax

			test byte ptr[esi + 0x2C], 1
			je animJEJump
			jmp animJNEBack
		animJEJump :
			jmp animJEBack
		animEnd :
		}

		WriteRelJump(0x6F02FF, (UInt32)animCodeStart);
		
	}
	

	/*
	 *
	 *  Papyrus Interface: ObjectUtil.psc
	 *
	 */

	void SetReplaceAnimation(StaticFunctionTag* base, TESObjectREFR* ObjRef, BSFixedString Anim, TESIdleForm* IdleRef) {
		if (!IdleRef || !Anim.data || !*Anim.data)
			return;
		GetSingleton()->SetValue(Forms::GetFormKey(ObjRef), Anim.data, IdleRef);
	}

	bool RemoveReplaceAnimation(StaticFunctionTag* base, TESObjectREFR* ObjRef, BSFixedString Anim) {
		if (!s_Idles || !Anim.data || !*Anim.data)
			return false;
		return GetSingleton()->UnsetValue(Forms::GetFormKey(ObjRef), Anim.data);
	}

	UInt32 CountReplaceAnimation(StaticFunctionTag* base, TESObjectREFR* ObjRef) {
		if (!s_Idles)
			return 0;
		int count = 0;
		Idles* Overrides = GetSingleton();
		Overrides->s_dataLock.Enter();

		Idles::Map::iterator itr = Overrides->Data.find(Forms::GetFormKey(ObjRef));
		if (itr != Overrides->Data.end())
			count = itr->second.size();

		Overrides->s_dataLock.Leave();
		return count;
	}

	UInt32 ClearReplaceAnimation(StaticFunctionTag* base, TESObjectREFR* ObjRef) {
		if (!s_Idles)
			return 0;
		int removed = 0;
		Idles* Overrides = GetSingleton();
		Overrides->s_dataLock.Enter();

		Idles::Map::iterator itr = Overrides->Data.find(Forms::GetFormKey(ObjRef));
		if (itr != Overrides->Data.end()){
			removed = itr->second.size();
			Overrides->Data.erase(itr);
		}

		Overrides->s_dataLock.Leave();
		return removed;
	}

	BSFixedString GetKeyReplaceAnimation(StaticFunctionTag* base, TESObjectREFR* ObjRef, UInt32 index) {
		if (!s_Idles)
			return NULL;
		std::string anim;
		Idles* Overrides = GetSingleton();
		Overrides->s_dataLock.Enter();

		Idles::Map::iterator itr = Overrides->Data.find(Forms::GetFormKey(ObjRef));
		if (itr != Overrides->Data.end() && index < itr->second.size()) {
			Idles::Obj::iterator itr2 = itr->second.begin();
			std::advance(itr2, index);
			anim = itr2->first;
		}

		Overrides->s_dataLock.Leave();
		return BSFixedString(anim.c_str());
	}

	TESIdleForm* GetValueReplaceAnimation(StaticFunctionTag* base, TESObjectREFR* ObjRef, BSFixedString Anim){
		if (!s_Idles || !Anim.data || !*Anim.data)
			return NULL;
		TESIdleForm* IdleRef = NULL;
		Idles* Overrides = GetSingleton();
		Overrides->s_dataLock.Enter();

		UInt64 objKey = Forms::GetFormKey(ObjRef);
		if (!Overrides->Data[objKey].empty()) {
			std::string key(Anim.data);
			boost::to_lower(key);
			Idles::Obj::iterator itr = Overrides->Data[objKey].find(key);
			if (itr != Overrides->Data[objKey].end()){
				TESForm* FormRef = LookupFormByID(itr->second);
				if (FormRef && FormRef->formType == kFormType_Idle)
					IdleRef = DYNAMIC_CAST(FormRef, TESForm, TESIdleForm);
			}			
		}

		Overrides->s_dataLock.Leave();
		return IdleRef;
	}

	void RegisterFuncs(VMClassRegistry* registry) {
		registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, TESObjectREFR*, BSFixedString, TESIdleForm*>("SetReplaceAnimation", "ObjectUtil", SetReplaceAnimation, registry));
		registry->SetFunctionFlags("ObjectUtil", "SetReplaceAnimation", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, TESObjectREFR*, BSFixedString>("RemoveReplaceAnimation", "ObjectUtil", RemoveReplaceAnimation, registry));
		registry->SetFunctionFlags("ObjectUtil", "RemoveReplaceAnimation", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, TESObjectREFR*>("CountReplaceAnimation", "ObjectUtil", CountReplaceAnimation, registry));
		registry->SetFunctionFlags("ObjectUtil", "CountReplaceAnimation", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, TESObjectREFR*>("ClearReplaceAnimation", "ObjectUtil", ClearReplaceAnimation, registry));
		registry->SetFunctionFlags("ObjectUtil", "ClearReplaceAnimation", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, BSFixedString, TESObjectREFR*, UInt32>("GetKeyReplaceAnimation", "ObjectUtil", GetKeyReplaceAnimation, registry));
		registry->SetFunctionFlags("ObjectUtil", "GetKeyReplaceAnimation", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, TESIdleForm*, TESObjectREFR*, BSFixedString>("GetValueReplaceAnimation", "ObjectUtil", GetValueReplaceAnimation, registry));
		registry->SetFunctionFlags("ObjectUtil", "GetValueReplaceAnimation", VMClassRegistry::kFunctionFlag_NoWait);

	}
}