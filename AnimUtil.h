#pragma once

#include <boost/container/list.hpp>

#include "skse64/GameForms.h"
#include "skse64/GameTypes.h"
#include "skse64/GameReferences.h"

struct StaticFunctionTag;
class VMClassRegistry;

namespace AnimUtil {
	struct TimeData
	{
		int FormId;
		float TargetTime;
		float CurrentTime;
		float Transition;
		int AnimPtr;
		std::string Name;

		void Update(float time);
		bool ShouldRemove();
	};

	typedef boost::container::list<TimeData*> TimeDataList;

	void InitPlugin();
	void ClearAll();
	void Update(float time);
	void _ASM_GetSpeed(int animPtr, int animData);

	void AddSpeedModifier(StaticFunctionTag * base, TESObjectREFR * obj, float speed, BSFixedString name);
	void RemoveSpeedModifier(StaticFunctionTag * base, TESObjectREFR * obj, BSFixedString name);
	void ClearSpeedModifier(StaticFunctionTag * base, BSFixedString name);
	void RegisterFuncs(VMClassRegistry* registry);

}
