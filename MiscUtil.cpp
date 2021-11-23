#include "MiscUtil.h"


#include <sstream>
#include <set>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>

#include "skse64/GameAPI.h"
#include "skse64/GameCamera.h"
#include "skse64/GameReferences.h"
#include "skse64/GameSettings.h"
#include "skse64/GameMenus.h"
#include "skse64/GameForms.h"
//#include "skse64/SafeWrite.h"

#include "skse64/GameRTTI.h"
#include "skse64/GameTypes.h"
#include "skse64/PapyrusArgs.h"

#include "Offsets.h"


//#include "SafeRead.h"

// TODO: test TESTScanCellNPCsByFaction and remove prepend

namespace MiscUtil {

	/*typedef void(*_ToggleFreeCam)(uintptr_t addr, bool stopTime);
	void ToggleFreeCamera(StaticFunctionTag* base, bool arg1) {
		int stopTime = arg1 ? 1 : 0;
		RelocPtr<uintptr_t> g_freeCam(0x02EEC9B8);
		RelocAddr<_ToggleFreeCam> ToggleFreeCam(0x0084BBD0);
		ToggleFreeCam(*g_freeCam, stopTime);
	}*/

	typedef void(*_ToggleFreeCam)(PlayerCamera * camera, bool stopTime);
	void ToggleFreeCamera(StaticFunctionTag* base, bool arg1) {
		int stopTime = arg1 ? 1 : 0;
		//RelocAddr<_ToggleFreeCam> ToggleFreeCam(0x008773A0);
		RelocAddr<_ToggleFreeCam> ToggleFreeCam((std::uintptr_t)Offset_ToggleFreeCam);
		ToggleFreeCam(PlayerCamera::GetSingleton(), stopTime);
	}

	//extern RelocPtr<SettingCollectionList*> g_iniSettingCollection;
	void SetFreeCameraSpeed(StaticFunctionTag* base, float speed) {
		Setting * setting = (*g_iniSettingCollection)->Get("fFreeCameraTranslationSpeed:Camera");
		if (setting)
			setting->SetDouble(speed);
	}

	void SetFreeCameraState(StaticFunctionTag* base, bool enable, float speed) {
		PlayerCamera* pc = PlayerCamera::GetSingleton();
		if (pc) {
			bool InFreeCamera = pc->cameraState->stateId == pc->kCameraState_Free;
			// Leave free ceramera
			if (InFreeCamera && !enable)
				ToggleFreeCamera(NULL, false);
			// Enter free camera
			else if (!InFreeCamera && enable){
				SetFreeCameraSpeed(NULL, speed);
				ToggleFreeCamera(NULL, false);
			}
		}
	}
	
	void PrintConsole(StaticFunctionTag* base, BSFixedString text) {
		if (!text.data)
			return;
		else if (strlen(text.data) < 1024)
			Console_Print(text.data);
		else { // Large strings printed to console crash the game - truncate it
			std::string msg = text.data;
			msg.resize(1020);
			msg.append("...");
			Console_Print(msg.c_str());
		}
	}

	/*void SetMenus(StaticFunctionTag* base, bool enabled){
		UInt32 ptr = Lib::SafeRead32(0x12E3548);
		if (ptr != 0) SafeWrite8(ptr + 0x118, (enabled ? 1 : 0));
	}*/
	void SetMenus(StaticFunctionTag* base, bool enabled){
		MenuManager::GetSingleton()->showMenus = enabled;
	}

	BSFixedString GetRaceEditorID(StaticFunctionTag* base, TESRace* RaceRef) {
		return RaceRef ? RaceRef->editorId.data : NULL;
	}

	BSFixedString GetActorRaceEditorID(StaticFunctionTag* base, Actor* ActorRef) {
		return ActorRef ? ActorRef->race->editorId.data : NULL;
	}


	//https://pastebin.com/i8jzU9wg
	/*struct CellLocker
	{
		CellLocker(TESObjectCELL* cell)
		{
			locked = cell;
			if (cell != NULL)
			{
				static int _lockCellEnterHelper = 0x4C0180;
				_asm
				{
					pushad
					pushfd

						mov ecx, cell
						call _lockCellEnterHelper

						popfd
						popad
				};
			}
		}

		~CellLocker()
		{
			TESObjectCELL * cell = locked;
			if (cell != NULL)
			{
				static int _lockCellExitHelper = 0x4C0190;
				_asm
				{
					pushad
					pushfd

						mov ecx, cell
						call _lockCellExitHelper

						popfd
						popad
				}
			}
		}

	private:
		TESObjectCELL * locked;
	};*/

	/*typedef void(*_TESObjectCELL_RefLocker_LockUnlock)(TESObjectCELL * thisPtr);
	RelocAddr<_TESObjectCELL_RefLocker_LockUnlock> TESObjectCELL_CellRefLockEnter(0x00271030);
	RelocAddr<_TESObjectCELL_RefLocker_LockUnlock> TESObjectCELL_CellRefLockExit(0x00271050);
	struct CellLocker {
		CellLocker(TESObjectCELL* cell){
			locked = cell;
			if (cell != NULL){
				TESObjectCELL_CellRefLockEnter(cell);
			}
		}

		~CellLocker(){
			TESObjectCELL * cell = locked;
			if (cell != NULL){
				TESObjectCELL_CellRefLockExit(cell);
			}
		}

	private:
		TESObjectCELL * locked;
	};*/



	/*bool HasKeyword(TESObjectREFR* ObjRef, std::string& kw){
		BGSKeywordForm* pKeywords = DYNAMIC_CAST(ObjRef, TESObjectREFR, BGSKeywordForm);
		if (!pKeywords) {
			pKeywords = DYNAMIC_CAST(ObjRef, TESForm, BGSKeywordForm);
			if (!pKeywords) {
				pKeywords = DYNAMIC_CAST(ObjRef->baseForm, TESForm, BGSKeywordForm);
			}
		}
		if (!pKeywords || pKeywords->numKeywords < 1) {
			return false;
		}
		
		for (UInt32 i = 0; i < pKeywords->numKeywords; i++)	{
			if (pKeywords->keywords[i]) {
				BSFixedString bskw = pKeywords->keywords[i]->keyword;
				if (!bskw || !bskw.data || bskw.data[0] == '\0') continue;
				if (boost::iequals(kw.c_str(), bskw.data)) return true;
			}
		}
		return false;
	}*/


	bool HasKeyword2(Actor* ActorRef, BGSKeyword* findKeyword) {
		TESNPC* npc = DYNAMIC_CAST(ActorRef->baseForm, TESForm, TESNPC);
		if (!npc) return false;

		BGSKeywordForm* pKeywords = DYNAMIC_CAST(npc, TESNPC, BGSKeywordForm);
		if (!pKeywords) {
			pKeywords = DYNAMIC_CAST(npc, TESForm, BGSKeywordForm);
			if (!pKeywords) {
				pKeywords = DYNAMIC_CAST(npc, TESForm, BGSKeywordForm);
			}
		}
		if (pKeywords) return pKeywords->HasKeyword(findKeyword);
		else return false;
	}

	/*bool HasKeyword(Actor* ActorRef, BGSKeyword* findKeyword) {
		BGSKeywordForm* pKeywords = DYNAMIC_CAST(ActorRef, Actor, BGSKeywordForm);
		if (!pKeywords) {
			pKeywords = DYNAMIC_CAST(ActorRef, TESForm, BGSKeywordForm);
			if (!pKeywords) {
				pKeywords = DYNAMIC_CAST(ActorRef->baseForm, TESForm, BGSKeywordForm);
			}
		}
		if (pKeywords) return pKeywords->HasKeyword(findKeyword);
		else return false;
	}*/

	bool HasKeyword(TESObjectREFR* ObjRef, BGSKeyword* findKeyword) {
		BGSKeywordForm* pKeywords = DYNAMIC_CAST(ObjRef, TESObjectREFR, BGSKeywordForm);
		if (!pKeywords) {
			pKeywords = DYNAMIC_CAST(ObjRef, TESForm, BGSKeywordForm);
			if (!pKeywords) {
				pKeywords = DYNAMIC_CAST(ObjRef->baseForm, TESForm, BGSKeywordForm);
			}
		}
		if (pKeywords) return pKeywords->HasKeyword(findKeyword);
		else return false;
	}

	/*bool HasKeyword2(TESObjectREFR* ObjRef, BGSKeyword* findKeyword) {
		Actor* ActorRef = DYNAMIC_CAST(ObjRef, TESObjectREFR, Actor);
		BGSKeywordForm* pKeywords = DYNAMIC_CAST(ActorRef, TESObjectREFR, BGSKeywordForm);
		if (!pKeywords) {
			pKeywords = DYNAMIC_CAST(ActorRef, TESForm, BGSKeywordForm);
			if (!pKeywords) {
				pKeywords = DYNAMIC_CAST(ActorRef->baseForm, TESForm, BGSKeywordForm);
			}
		}
		if (pKeywords) return pKeywords->HasKeyword(findKeyword);
		else return false;
	}

	bool HasKeyword(TESObjectREFR* ObjRef, BGSKeyword* findKeyword) {
		BSFixedString kw = (findKeyword) ? findKeyword->keyword.Get() : NULL;
		return ObjRef->

		_MESSAGE("Checking keywords");
		BGSKeywordForm* keywords2 = DYNAMIC_CAST(ObjRef, TESForm, BGSKeywordForm);
		if(keywords2) _MESSAGE("Ref Keywords: %d", (int)keywords2->numKeywords);


		BGSKeywordForm* keywords = DYNAMIC_CAST(ObjRef->baseForm, TESForm, BGSKeywordForm);

		if (keywords) {
			_MESSAGE("Base Keywords: %d", (int)keywords->numKeywords);
			return keywords->HasKeyword(findKeyword);
		}
		else return false;
	}*/

	//bool IsWithinRadius(TESObjectREFR* CenterObj, TESObjectREFR* ObjRef, float distance) {
	bool IsWithinRadius(TESObjectREFR* CenterObj, TESObjectREFR* ObjRef, float &distance) {
		NiPoint3 &a = CenterObj->pos;
		NiPoint3 &b = ObjRef->pos;
		float tempx, tempy, tempz, tempd;
		tempx = std::abs(a.x - b.x);
		tempy = std::abs(a.y - b.y);
		tempz = std::abs(a.z - b.z);
		if (tempx + tempy + tempz < distance)
			return true; // very small distances
		if (tempx + tempy + tempz > distance / 2)
			return false; // very large distances
		tempx = tempx * tempx;
		tempy = tempy * tempy;
		tempz = tempz * tempz;
		tempd = distance * distance;
		if (tempx + tempy + tempz < tempd)
			return true; // near but within distance
		return false;
	}

	/*inline bool ValidateCellList(tArray<TESObjectREFR*> &objList) {
		if (objList.count == 0) {
			_MESSAGE("\tERROR - objectList is empty.");
			return false;
		}
		else if (!objList.entries) {
			_MESSAGE("\tERROR - objectList entries are null.");
			return false;
		}
		else {
			_MESSAGE("\tobjectList count: %lu", objList.count);
			return true;
		}
	}*/

	inline TESObjectREFR* GetCellObj(TESObjectCELL* Cell, UInt32 &idx) {
		if (Cell->refData.refArray[idx].unk08 && Cell->refData.refArray[idx].ref) {
			return Cell->refData.refArray[idx].ref;
		}
		return NULL;
	}

	VMResultArray<TESObjectREFR*> ScanCellObjects(StaticFunctionTag* base, UInt32 FindType, TESObjectREFR* CenterObj, float SearchRadius, BGSKeyword* FindKeyword) {
		VMResultArray<TESObjectREFR*> output;
		if (CenterObj && CenterObj->parentCell) {
			TESObjectCELL* Cell = CenterObj->parentCell;
			//TESObjectCELL::ReferenceData refData = Cell->refData;

			_MESSAGE("ScanCellObjects(%d, 0x%X, %.2f) searching cell 0x%X with %d objects", FindType, (int)CenterObj->formID, SearchRadius, (int)Cell->formID, int(Cell->refData.maxSize - Cell->refData.freeEntries));
			
			UInt32 count = Cell->refData.maxSize;
			for (UInt32 idx = 0, n = 0; idx < count; idx++) {
				TESObjectREFR* ObjRef = GetCellObj(Cell, idx);

				if (ObjRef == NULL || ObjRef->baseForm->formType != FindType) continue; // Invalid
				else if (SearchRadius > 0.0f && !IsWithinRadius(CenterObj, ObjRef, SearchRadius)) continue; // Outside search radius
				else if (FindKeyword && !HasKeyword(ObjRef, FindKeyword)) continue; // Missing keyword
				else output.push_back(ObjRef); // MATCH

			}
			_MESSAGE("\tResults: %d", (int)output.size());
		}
		return output;
	}


	VMResultArray<Actor*> ScanCellNPCs(StaticFunctionTag* base, TESObjectREFR* CenterObj, float SearchRadius, BGSKeyword* FindKeyword, bool ignoredead) {
		VMResultArray<Actor*> output;
		if (CenterObj && CenterObj->parentCell) {
			TESObjectCELL* Cell = CenterObj->parentCell;
			
			_MESSAGE("ScanCellNPCs(0x%X, %.2f) searching cell 0x%X with %d objects", (int)CenterObj->formID, SearchRadius, (int)Cell->formID, int(Cell->refData.maxSize - Cell->refData.freeEntries));
			
			//std::string kw = (FindKeyword != NULL) ? FindKeyword->keyword.Get() : "";

			UInt32 count = Cell->refData.maxSize;
			for (UInt32 idx = 0, n = 0; idx < count; idx++) {
				TESObjectREFR* ObjRef = GetCellObj(Cell, idx);
				Actor *ActorRef = ObjRef != NULL ? DYNAMIC_CAST(ObjRef, TESObjectREFR, Actor) : NULL;
				if (ActorRef == NULL || (ignoredead && ActorRef->IsDead(1))) continue;
				else if (SearchRadius > 0.0f && !IsWithinRadius(CenterObj, ActorRef, SearchRadius)) continue;
				//else if (!kw.empty() && !HasKeyword(ActorRef, kw)) continue;
				else if (FindKeyword && !HasKeyword2(ActorRef, FindKeyword)) continue;
				else output.push_back(ActorRef);
			}
			_MESSAGE("\tResults: %d", (int)output.size());

		}
		return output;
	}

	// Copied from skse/PapyrusActor.cpp
	typedef std::set<TESFaction*> FactionRankSet;
	class CollectUniqueFactions : public Actor::FactionVisitor {
	public:
		CollectUniqueFactions::CollectUniqueFactions(FactionRankSet * rankSet, SInt8 min, SInt8 max, TESFaction* FactionRef) : m_rankSet(rankSet), m_min(min), m_max(max), FindFaction(FactionRef) { }
		virtual bool Accept(TESFaction * faction, SInt8 rank)
		{
			if (rank >= m_min && rank <= m_max && FindFaction->formID == faction->formID)
				m_rankSet->insert(faction);
			return false;
		}

	private:
		SInt8 m_min;
		SInt8 m_max;
		FactionRankSet * m_rankSet;
		TESFaction* FindFaction;
	};
	// copied end

	bool IsInFaction(Actor* ActorRef, TESFaction* FactionRef, SInt8 min, SInt8 max) {
		if (ActorRef != NULL && FactionRef != NULL) {
			FactionRankSet rankSet;
			CollectUniqueFactions factionVisitor(&rankSet, min, max, FactionRef);
			ActorRef->VisitFactions(factionVisitor);
			if (rankSet.size() > 0)
				return true;
		}
		return false;
	}


	VMResultArray<Actor*> ScanCellNPCsByFaction(StaticFunctionTag* base, TESFaction* FindFaction, TESObjectREFR* CenterObj, float SearchRadius, SInt32 gte, SInt32 lte, bool ignoredead) {
		VMResultArray<Actor*> output;
		if (CenterObj != NULL && FindFaction != NULL && CenterObj->parentCell) {

			if (gte > SCHAR_MAX) gte = SCHAR_MAX;
			if (gte < SCHAR_MIN) gte = SCHAR_MIN;
			if (lte < SCHAR_MIN) lte = SCHAR_MIN;
			if (lte > SCHAR_MAX) lte = SCHAR_MAX;

			TESObjectCELL* Cell = CenterObj->parentCell;

			_MESSAGE("ScanCellNPCsByFaction(0x%X, 0x%X, %.2f) searching cell 0x%X with %d objects", (int)FindFaction->formID, (int)CenterObj->formID, SearchRadius, (int)Cell->formID, int(Cell->refData.maxSize - Cell->refData.freeEntries));

			UInt32 count = Cell->refData.maxSize;
			for (UInt32 idx = 0, n = 0; idx < count; idx++) {
				TESObjectREFR* ObjRef = GetCellObj(Cell, idx);
				Actor *ActorRef = ObjRef != NULL ? DYNAMIC_CAST(ObjRef, TESObjectREFR, Actor) : NULL;
				if (ActorRef == NULL || (ignoredead && ActorRef->IsDead(1))) continue;
				else if (SearchRadius > 0.0f && !IsWithinRadius(CenterObj, ActorRef, SearchRadius)) continue;
				else if (!IsInFaction(ActorRef, FindFaction, gte, lte)) continue;
				else output.push_back(ActorRef);
			}
			_MESSAGE("\tResults: %d", (int)output.size());

		}
		return output;
	}
	



	/*
	VMResultArray<TESObjectREFR*> ScanCellObjects2(StaticFunctionTag* base, UInt32 FindType, TESObjectREFR* CenterObj, float SearchRadius, BGSKeyword* FindKeyword) {
		VMResultArray<TESObjectREFR*> output;
		if (CenterObj && CenterObj->parentCell) {
			TESObjectCELL* Cell = CenterObj->parentCell;
			//TESObjectCELL::ReferenceData refData = Cell->refData;

			UInt32 count = Cell->refData.maxSize;
			_MESSAGE("ScanCellObjects(%d, 0x%X, %.2f) searching cell 0x%X with %d objects", FindType, (int)CenterObj->formID, SearchRadius, (int)Cell->formID, count);


			for (UInt32 idx = 0, n = 0; idx < count; idx++) {
				if (Cell->refData.refArray[idx].unk08 && Cell->refData.refArray[idx].ref && Cell->refData.refArray[idx].ref->baseForm->formType == FindType) {
					TESObjectREFR* ObjRef = Cell->refData.refArray[idx].ref;
					if (ObjRef == NULL) continue; // invalid
					else if (SearchRadius > 0.0f && !IsWithinRadius(CenterObj, ObjRef, SearchRadius)) continue; // Outside search radius
					else if (FindKeyword && !HasKeyword(ObjRef, FindKeyword)) continue; // Missing keyword
					else output.push_back(ObjRef); // MATCH
				}
			}
			_MESSAGE("\tResults: %d", (int)output.size());
		}
		return output;
	}


	VMResultArray<TESObjectREFR*> ScanCellObjects(StaticFunctionTag* base, UInt32 FindType, TESObjectREFR* CenterObj, float SearchRadius, BGSKeyword* FindKeyword) {
		VMResultArray<TESObjectREFR*> output;
		if (CenterObj && CenterObj->parentCell) {
			TESObjectCELL* Cell = CenterObj->parentCell;
			CellLocker _locker(Cell);
			tArray<TESObjectREFR*> objList = Cell->objectList;
			_MESSAGE("ScanCellObjects(%d, 0x%X, %.2f) searching cell 0x%X", FindType, (int)CenterObj->formID, SearchRadius, (int)Cell->formID);

			TESObjectCELL::ReferenceData refData = Cell->refData;
			_MESSAGE("ref: 0x%X, type: %d", (int)refData.refArray->ref->formID, (int)refData.refArray->ref->formType);
			TESObjectCELL* Cell2 = refData.refArray->ref->parentCell;
			_MESSAGE("parentCell: 0x%X, type: %d", (int)Cell2->formID, (int)Cell2->formType);

			if (ValidateCellList(objList)) {
				UInt32 count = objList.count;
				for (UInt32 idx = 0; idx < count; ++idx) {
					TESObjectREFR* ObjRef = objList[idx];
					if (ObjRef == NULL || ObjRef->baseForm->GetFormType() != FindType) continue; // Wrong type
					else if (SearchRadius > 0.0f && !IsWithinRadius(CenterObj, ObjRef, SearchRadius)) continue; // Outside search radius
					else if (FindKeyword && !HasKeyword(ObjRef, FindKeyword)) continue; // Missing keyword
					else output.push_back(ObjRef); // MATCH
				}
				_MESSAGE("\tResults: %d", (int)output.size());
			}
		}
		return output;
	}


		VMResultArray<Actor*> ScanCellNPCs(StaticFunctionTag* base, TESObjectREFR* CenterObj, float SearchRadius, BGSKeyword* FindKeyword, bool ignoredead) {
		VMResultArray<Actor*> output;
		if (CenterObj != NULL) {
			_MESSAGE("ScanCellNPCs(Center: 0x%X, Radius: %.2f) Starting...", (int)CenterObj->formID, SearchRadius);
			TESObjectCELL* Cell = CenterObj->parentCell;
			if (Cell) {
				//CellLocker _locker(Cell);
				tArray<TESObjectREFR*> objList = Cell->objectList;
				UInt32 count = objList.count;
				for (UInt32 idx = 0; idx < count; ++idx) {
					TESObjectREFR* ObjRef = objList[idx];
					Actor *ActorRef = ObjRef != NULL ? DYNAMIC_CAST(ObjRef, TESObjectREFR, Actor) : NULL;
					if (ActorRef == NULL || (ignoredead && ActorRef->IsDead(1))) continue;
					else if (SearchRadius > 0.0f && !IsWithinRadius(CenterObj, ActorRef, SearchRadius)) continue;
					else if (FindKeyword && !HasKeyword(ActorRef, FindKeyword)) continue;
					else output.push_back(ActorRef);
				}
			}
			_MESSAGE("\tResults: %d", (int)output.size());
		}
		return output;
	}
	*/







	namespace fs = boost::filesystem;

	bool FileExists(StaticFunctionTag* base, BSFixedString name) {
		if (!name.data || name.data[0] == '\0') return false;
		fs::path Path = fs::path(name.data);
		return fs::exists(Path);
	}

	VMResultArray<BSFixedString> FilesInFolder(StaticFunctionTag* base, BSFixedString dirpath, BSFixedString extension) {
		VMResultArray<BSFixedString> arr;
		if (dirpath.data && dirpath.data[0] != '\0') {
			fs::path dir(dirpath.data);
			fs::directory_iterator end_iter;
			if (fs::exists(dir) && fs::is_directory(dir)) {
				std::string ext;
				if (extension.data[0] == '.') ext = extension.data;
				else {
					ext = ".";
					ext.append(extension.data);
				}
				_MESSAGE("dir: %s ext: %s", dirpath.data, ext.c_str());
				for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter) {
					if (fs::is_regular_file(dir_iter->status())) {
						fs::path filepath = dir_iter->path();
						std::string file = filepath.filename().generic_string();
						//std::string ext = filepath.extension().generic_string();
						//_MESSAGE("file: %s ext: %s", file.c_str(), ext.c_str());
						if (ext == ".*" || boost::iequals(filepath.extension().generic_string(), ext))
							arr.push_back(BSFixedString(file.c_str()));
					}
				}
			}
		}
		return arr;
	}

	VMResultArray<BSFixedString> FoldersInFolder(StaticFunctionTag* base, BSFixedString dirpath) {
		VMResultArray<BSFixedString> arr;
		if (dirpath.data && dirpath.data[0] != '\0') {
			fs::path dir(dirpath.data);
			fs::directory_iterator end_iter;
			if (fs::exists(dir) && fs::is_directory(dir)) {
				_MESSAGE("dir: %s", dirpath.data);

				for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter) {
					if (fs::is_directory(dir_iter->status())) {
						fs::path filepath = dir_iter->path();
						std::string file = filepath.filename().generic_string();
						arr.push_back(BSFixedString(file.c_str()));
					}
				}
			}
		}
		return arr;
	}

	BSFixedString ReadFromFile(StaticFunctionTag* base, BSFixedString filename) {
		if (!filename.data || filename.data[0] == '\0') return BSFixedString("");
		std::string output = "0";
		fs::path File(filename.data);
		if (fs::exists(File) && !fs::is_directory(File)) {
			fs::ifstream doc;
			try
			{
				doc.open(File, std::ios::in | std::ios::binary);
				if (!doc.fail()) {
					std::stringstream ss;
					ss << doc.rdbuf();
					output = ss.str();
				}
			}
			catch (std::exception&)
			{
				_MESSAGE("Failed to load/read file: %s", File.generic_string().c_str());
			}
			if (doc.is_open())
				doc.close();

		}
		return BSFixedString(output.c_str());
	}

	bool WriteToFile(StaticFunctionTag* base, BSFixedString filename, BSFixedString input, bool append, bool timestamp) {
		if (!filename.data || filename.data[0] == '\0') return false;
		bool output = false;
		fs::path File(filename.data);
		fs::ofstream doc;
		try
		{
			if(append) doc.open(File, std::ios::out | std::ios::app | std::ios::binary);
			else doc.open(File, std::ios::out | std::ios::binary);
			if (doc.is_open()) {
				doc << input.data;
				doc.close();
				output = true;
			}
		}
		catch (std::exception&)
		{
			_MESSAGE("Failed to load/write file: %s", File.generic_string().c_str());
		}
		if (doc.is_open()) doc.close();
		return output;
	}


} // MiscUtil



#include "skse64/PapyrusNativeFunctions.h"

void MiscUtil::RegisterFuncs(VMClassRegistry* registry) {
	
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

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, BSFixedString, TESRace*>("GetRaceEditorID", "MiscUtil", GetRaceEditorID, registry));
	registry->SetFunctionFlags("MiscUtil", "GetRaceEditorID", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, BSFixedString, Actor*>("GetActorRaceEditorID", "MiscUtil", GetActorRaceEditorID, registry));
	registry->SetFunctionFlags("MiscUtil", "GetActorRaceEditorID", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("FileExists", "MiscUtil", FileExists, registry));
	registry->SetFunctionFlags("MiscUtil", "FileExists", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, VMResultArray<TESObjectREFR*>, UInt32, TESObjectREFR*, float, BGSKeyword*>("ScanCellObjects", "MiscUtil", ScanCellObjects, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, VMResultArray<Actor*>, TESObjectREFR*, float, BGSKeyword*, bool>("ScanCellNPCs", "MiscUtil", ScanCellNPCs, registry));
	registry->RegisterFunction(new NativeFunction6<StaticFunctionTag, VMResultArray<Actor*>, TESFaction*, TESObjectREFR*, float, SInt32, SInt32, bool>("ScanCellNPCsByFaction", "MiscUtil", ScanCellNPCsByFaction, registry));

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BSFixedString>, BSFixedString, BSFixedString>("FilesInFolder", "MiscUtil", FilesInFolder, registry));
	registry->SetFunctionFlags("MiscUtil", "FilesInFolder", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, VMResultArray<BSFixedString>, BSFixedString>("FoldersInFolder", "MiscUtil", FoldersInFolder, registry));
	registry->SetFunctionFlags("MiscUtil", "FoldersInFolder", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, BSFixedString, BSFixedString>("ReadFromFile", "MiscUtil", ReadFromFile, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, bool, BSFixedString, BSFixedString, bool, bool>("WriteToFile", "MiscUtil", WriteToFile, registry));

}
