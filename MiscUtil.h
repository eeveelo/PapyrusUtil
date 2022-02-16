#pragma once

#include "skse64/GameReferences.h"
#include "skse64/PapyrusVM.h"


struct StaticFunctionTag;

namespace MiscUtil {
	void RegisterFuncs(VMClassRegistry* registry);

	//void ToggleFreeCamera(StaticFunctionTag* base, bool arg1);
	//void SetFreeCameraSpeed(StaticFunctionTag* base, float speed);
	//void SetFreeCameraState(StaticFunctionTag* base, bool enable, float speed);
	void PrintConsole(StaticFunctionTag* base, BSFixedString text);
	//void SetMenus(StaticFunctionTag* base, bool enabled);
	BSFixedString GetRaceEditorID(StaticFunctionTag* base, TESRace* RaceRef);
	BSFixedString GetActorRaceEditorID(StaticFunctionTag* base, Actor* ActorRef);
}
