#include "MiscUtil.h"

//#include "skse/PluginAPI.h"
#include "skse/GameAPI.h"
#include "skse/GameData.h"
#include "skse/GameTypes.h"
#include "skse/GameForms.h"
#include "skse/GameRTTI.h"
#include "skse/GameCamera.h"
//#include "skse/GameReferences.h"
//#include "skse/GameObjects.h"

#include "skse/SafeWrite.h"

#include "skse/NiNodes.h"
#include "skse/NiGeometry.h"

#include "skse/PapyrusNativeFunctions.h"

#include "SafeRead.h"

#include <fstream>
#include <ctime>

#pragma warning(disable: 4996)
#pragma warning(disable: 4229)

namespace MiscUtil {

	void ToggleFreeCamera(StaticFunctionTag* base, bool arg1) {
		int funcAddr = 0x83E4C0;
		int thisPtr = Lib::SafeRead32(0x12E7288);
		int stopTime = arg1 ? 1 : 0;
		_asm
		{
			mov eax, stopTime
			mov ecx, thisPtr
			push eax
			call funcAddr
		}
	}

	void SetFreeCameraSpeed(StaticFunctionTag* base, float speed) {
		SafeWriteBuf(0x12B1EEC, &speed, 4);
	}

	void SetFreeCameraState(StaticFunctionTag* base, bool enable, float speed) {
		PlayerCamera* pc = PlayerCamera::GetSingleton();
		bool InFreeCamera = pc->cameraState->stateId == pc->kCameraState_Free;
		// Leave free camera
		if (InFreeCamera && !enable){
			ToggleFreeCamera(NULL, false);
		}
		// Enter free camera
		else if (!InFreeCamera && enable){
			SetFreeCameraSpeed(NULL, speed);
			ToggleFreeCamera(NULL, false);
			CALL_MEMBER_FN(pc, SetCameraState)(pc->cameraStates[pc->kCameraState_Free]);
		}
	}


	float GetNodeRotation(StaticFunctionTag* base, TESObjectREFR* obj, BSFixedString nodeName, bool firstPerson, UInt32 nth) {
		if (nth < 0 || nth > 8 || !obj)
			return -1.0f;

		NiAVObject* skeleton = obj->GetNiNode();
		PlayerCharacter* player = DYNAMIC_CAST(obj, TESObjectREFR, PlayerCharacter);
		if (player && player->loadedState)
			skeleton = firstPerson ? player->firstPersonSkeleton : player->loadedState->node;
		if (!skeleton)
			return -1.0f;

		NiAVObject* node;
		if (skeleton && nodeName.data[0])
			node = skeleton->GetObjectByName(&nodeName.data);
		if (!node)
			return -1.0f;

		return *node->m_worldTransform.rot.data[nth];
	}

	void PrintConsole(StaticFunctionTag* base, BSFixedString text) {
		if (!text.data)
			return;
		else if (strlen(text.data) < 1000)
			Console_Print(text.data);
		else { // Large strings printed to console crash the game - truncate it
			std::string msg = text.data;
			msg.resize(997);
			msg.append("...");
			Console_Print(msg.c_str());
		}
	}

	void SetMenus(StaticFunctionTag* base, bool enabled){
		UInt32 ptr = Lib::SafeRead32(0x12E3548);
		if (ptr != 0) {
			if (enabled)
				SafeWrite8(ptr + 0x118, 1);
			else
				SafeWrite8(ptr + 0x118, 0);
		}
	}

	int batAddr = 0;
	int overBat = 0;
	char textBat[256];
	typedef char(*__cdecl batCommand)(int, char*, int, int, int, int*, int, int);

	void ExecuteBat(StaticFunctionTag* base, BSFixedString fileName) {
		memset(textBat, 0, sizeof(char)* 256);
		memcpy(textBat, fileName.data, strlen(fileName.data));

		batAddr = (int)textBat;
		overBat = 1;

		batCommand func = (batCommand)0x547AA0;

		func(0, 0, 0, 0, 0, 0, 0, 0);
	}

	BSFixedString ReadFromFile(StaticFunctionTag* base, BSFixedString fileName) {
		std::ifstream doc;
		try {
			doc.open(fileName.data, std::ifstream::in | std::ifstream::binary);
		}
		catch (std::exception*){
			return NULL;
		}
		if (doc.fail())
			return NULL;

		std::string contents;
		doc.seekg(0, std::ios::end);
		contents.resize(doc.tellg());
		doc.seekg(0, std::ios::beg);
		doc.read(&contents[0], contents.size());
		doc.close();
		return BSFixedString(contents.c_str());
	}

	bool WriteToFile(StaticFunctionTag* base, BSFixedString fileName, BSFixedString text, bool append, bool timestamp) {
		std::ofstream doc;
		try {
			doc.open(fileName.data, append ? (std::ofstream::out | std::ofstream::app) : std::ofstream::out);
		}
		catch (std::exception*) {
			return false;
		}
		if (doc.fail())
			return false;

		if (timestamp) {
			time_t curTime;
			time(&curTime);
			struct tm * timeinfo = localtime(&curTime);
			doc << "[" << asctime(timeinfo) << "] ";
		}

		doc << text.data;
		doc.close();
		return true;
	}

	BSFixedString GetRaceEditorID(StaticFunctionTag* base, TESRace* raceForm) {
		if (!raceForm)
			return NULL;
		return BSFixedString(raceForm->editorId.data);
	}

	BSFixedString GetActorRaceEditorID(StaticFunctionTag* base, Actor* actorRef) {
		if (!actorRef)
			return NULL;
		return BSFixedString(actorRef->race->editorId.data);
	}

	void RegisterFuncs(VMClassRegistry* registry) {
		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, bool>("ToggleFreeCamera", "MiscUtil", ToggleFreeCamera, registry));
		registry->SetFunctionFlags("MiscUtil", "ToggleFreeCamera", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, float>("SetFreeCameraSpeed", "MiscUtil", SetFreeCameraSpeed, registry));
		registry->SetFunctionFlags("MiscUtil", "SetFreeCameraSpeed", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, bool, float>("SetFreeCameraState", "MiscUtil", SetFreeCameraState, registry));
		registry->SetFunctionFlags("MiscUtil", "SetFreeCameraState", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, BSFixedString>("PrintConsole", "MiscUtil", PrintConsole, registry));
		registry->SetFunctionFlags("MiscUtil", "PrintConsole", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, bool>("SetMenus", "MiscUtil", SetMenus, registry));
		registry->SetFunctionFlags("MiscUtil", "SetMenus", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, BSFixedString>("ExecuteBat", "MiscUtil", ExecuteBat, registry));
		registry->SetFunctionFlags("MiscUtil", "ExecuteBat", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, BSFixedString, BSFixedString>("ReadFromFile", "MiscUtil", ReadFromFile, registry));
		registry->SetFunctionFlags("MiscUtil", "ReadFromFile", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, bool, BSFixedString, BSFixedString, bool, bool>("WriteToFile", "MiscUtil", WriteToFile, registry));
		registry->SetFunctionFlags("MiscUtil", "WriteToFile", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, BSFixedString, TESRace*>("GetRaceEditorID", "MiscUtil", GetRaceEditorID, registry));
		registry->SetFunctionFlags("MiscUtil", "GetRaceEditorID", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, BSFixedString, Actor*>("GetActorRaceEditorID", "MiscUtil", GetActorRaceEditorID, registry));
		registry->SetFunctionFlags("MiscUtil", "GetActorRaceEditorID", VMClassRegistry::kFunctionFlag_NoWait);
	}

	int jumpBat = 0x547AAE;
	int jumpBat2 = 0x547AEF;
	void InitPlugin(){
		void * batCodeStart;
		_asm
		{
			mov batCodeStart, offset batStart
			jmp batEnd
		batStart :
			sub esp, 0x104
			cmp overBat, 0
			je batOrig
			mov overBat, 0
			mov eax, batAddr
			mov ecx, 0
		batCopy:
			mov dl, [eax + ecx]
			mov[esp + ecx], dl
			cmp dl, 0
			je batFinish
			inc ecx
			jmp batCopy
		batFinish :
			jmp jumpBat2
		batOrig :
			jmp jumpBat
		batEnd :
		}

		WriteRelJump(0x547AA8, (UInt32)batCodeStart);
	}
}