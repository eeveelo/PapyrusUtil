#include "AnimUtil.h"
//#include <boost/algorithm/string.hpp>
#include "common/ICriticalSection.h"
#include "skse64/SafeWrite.h"
#include "Asm.h"

namespace AnimUtil {
	
	ICriticalSection* s_timeLock = NULL;
	TimeDataList* TimeInfo = NULL;

	int _graphUpdate1 = 0x64F84A;
	int _graphUpdate2 = 0x64F854;

	void InitPlugin(){
		_MESSAGE("AnimUtil::InitPlugin() - 1");
		if(s_timeLock == NULL) s_timeLock = new ICriticalSection();
		if(TimeInfo == NULL) TimeInfo = new TimeDataList();

		// Hook animation graph update speed. 
		{
			START_ASM1(GU1)
			START_ASM2(GU1)
			START_ASM3(GU1)

			pushad
			pushfd

			push edi
			push ecx
			call _ASM_GetSpeed
			add esp, 8

			popfd
			popad

			mov esi, ecx
			mov [esp+0xC], 0
			jmp _graphUpdate2

			END_ASM(GU1, _graphUpdate1, _graphUpdate2)
		};

		// Allow animation to play in reverse when negative time.
		{
			char jmpOpcode = (char)0xEB;
			SafeWriteBuf(0xBB032A, &jmpOpcode, 1);
		}
		_MESSAGE("AnimUtil::InitPlugin() - 2");
	}



	void _ASM_GetSpeed(int animPtr, int animData){
		float speed = 1.0f;

		if (TimeInfo == NULL) TimeInfo = new TimeDataList();
		if (!TimeInfo->empty()){
			_MESSAGE("_ASM_GetSpeed(%d, %d)", animPtr, animData);
			if (s_timeLock == NULL) s_timeLock = new ICriticalSection();
			s_timeLock->Enter();
			for (TimeDataList::iterator itr = TimeInfo->begin(); itr != TimeInfo->end(); ++itr){
				if ((*itr)->AnimPtr == animPtr)
					speed *= (*itr)->CurrentTime;
			}
			s_timeLock->Leave();
		}

		*((float*)animData) *= speed;
	}
	
	void ClearAll(){
		_MESSAGE("AnimUtil::ClearAll()");

		s_timeLock->Enter();

		for (TimeDataList::iterator itr = TimeInfo->begin(); itr != TimeInfo->end();){
			TimeData * data = *itr;
			itr = TimeInfo->erase(itr);
			delete data;
		}

		s_timeLock->Leave();
	}

	void TimeData::Update(float time){
		if (TargetTime == CurrentTime) return;
		if (Transition <= 0.0f || Transition <= time) {
			CurrentTime = TargetTime;
			Transition = 0.0f;
			return;
		}
		if (time <= 0.0f) return;

		float ratio = time / Transition;
		float diff = TargetTime - CurrentTime;
		diff *= ratio;
		CurrentTime += diff;
		Transition -= time;
	}

	bool TimeData::ShouldRemove(){
		return TargetTime == CurrentTime && CurrentTime == 1.0f;
	}

	void Update(float time){
		s_timeLock->Enter();

		for (TimeDataList::iterator itr = TimeInfo->begin(); itr != TimeInfo->end();){
			(*itr)->Update(time);
			if ((*itr)->ShouldRemove())
			{
				itr = TimeInfo->erase(itr);
				continue;
			}

			int actor = (int)LookupFormByID((*itr)->FormId);
			if (actor == 0)
				(*itr)->AnimPtr = 0;
			else
				(*itr)->AnimPtr = actor + 0x20;

			itr++;
		}

		s_timeLock->Leave();
	}


	void AddSpeedModifier(StaticFunctionTag * base, TESObjectREFR * obj, float speed, float transition, BSFixedString name){
		if (!obj || !name.data || !*name.data)
			return;
		
		int formId = obj->formID;

		s_timeLock->Enter();

		float curTime = 1.0f;
		for (TimeDataList::iterator itr = TimeInfo->begin(); itr != TimeInfo->end();){
			if ((*itr)->FormId == formId && !_stricmp((*itr)->Name.c_str(), name.data))
			{
				TimeData * data = *itr;
				TimeInfo->erase(itr);
				curTime = data->CurrentTime;
				delete data;
				break;
			}

			itr++;
		}

		{
			TimeData * data = new TimeData();
			data->AnimPtr = 0;
			data->FormId = formId;
			data->Name = name.data;
			data->TargetTime = speed;
			data->CurrentTime = curTime;
			data->Transition = transition;

			TimeInfo->push_back(data);
		}

		s_timeLock->Leave();
	}

	void RemoveSpeedModifier(StaticFunctionTag * base, TESObjectREFR * obj, BSFixedString name){
		if (!obj || !name.data || !*name.data) return;

		int formId = obj->formID;

		s_timeLock->Enter();

		for (TimeDataList::iterator itr = TimeInfo->begin(); itr != TimeInfo->end();){
			if ((*itr)->FormId == formId && !_stricmp((*itr)->Name.c_str(), name.data))
			{
				TimeData * data = *itr;
				TimeInfo->erase(itr);
				delete data;
				break;
			}

			itr++;
		}

		s_timeLock->Leave();
	}

	void ClearSpeedModifier(StaticFunctionTag * base, BSFixedString name){
		if (!name.data || !*name.data)
			return;

		s_timeLock->Enter();

		for (TimeDataList::iterator itr = TimeInfo->begin(); itr != TimeInfo->end();){
			if (!_stricmp((*itr)->Name.c_str(), name.data))
			{
				TimeData * data = *itr;
				itr = TimeInfo->erase(itr);
				delete data;
				continue;
			}

			itr++;
		}

		s_timeLock->Leave();
	}
}

#include "skse64/PapyrusNativeFunctions.h"

void AnimUtil::RegisterFuncs(VMClassRegistry* registry) {
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, TESObjectREFR*, float, float, BSFixedString>("AddSpeedModifier", "AnimUtil", AnimUtil::AddSpeedModifier, registry));
	registry->SetFunctionFlags("AnimUtil", "AddSpeedModifier", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, TESObjectREFR*, BSFixedString>("RemoveSpeedModifier", "AnimUtil", AnimUtil::RemoveSpeedModifier, registry));
	registry->SetFunctionFlags("AnimUtil", "RemoveSpeedModifier", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, BSFixedString>("ClearSpeedModifier", "AnimUtil", AnimUtil::ClearSpeedModifier, registry));
	registry->SetFunctionFlags("AnimUtil", "ClearSpeedModifier", VMClassRegistry::kFunctionFlag_NoWait);
}
