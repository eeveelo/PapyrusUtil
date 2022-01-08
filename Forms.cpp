#include "Forms.h"

//#include <boost/algorithm/string.hpp>
//#include <boost/lexical_cast.hpp>
//#include <vector>
#include <unordered_map>
#include <functional>

#include "skse64/GameAPI.h"

#include "skse64/GameRTTI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameReferences.h"

#include "skse64/Serialization.h"


namespace Forms {



	static std::unordered_map<UInt32, UInt32> s_savedModIndexMap;
	

	void ClearModList() {
		s_savedModIndexMap.clear();
	}
	// Old version. Keep to load existing co-save
	void LoadModList(SKSESerializationInterface * intfc)
	{
		_MESSAGE("Loading old mod list:");

		DataHandler * dhand = DataHandler::GetSingleton();

		char name[0x104] = { 0 };
		UInt16 nameLen = 0;

		UInt8 numSavedMods = 0;
		intfc->ReadRecordData(&numSavedMods, sizeof(numSavedMods));
		for (UInt32 i = 0; i < numSavedMods; i++)
		{
			intfc->ReadRecordData(&nameLen, sizeof(nameLen));
			intfc->ReadRecordData(&name, nameLen);
			name[nameLen] = 0;

			const ModInfo * modInfo = dhand->LookupModByName(name);
			if (modInfo) {
				UInt32 newIndex = modInfo->GetPartialIndex();
				s_savedModIndexMap[i] = newIndex;
				_MESSAGE("\t(%d -> %d)\t%s", i, newIndex, &name);
			}
			else {
				s_savedModIndexMap[i] = 0xFF;
				_MESSAGE("\t(%d -> UNLOADED)\t%s", i, &name);
			}
		}
	}

	// New save list from SKSE
	void SavePluginsList(SKSESerializationInterface * intfc)
	{
		DataHandler * dhand = DataHandler::GetSingleton();

		struct IsActiveFunctor
		{
			bool Accept(ModInfo * modInfo)
			{
				return modInfo && modInfo->IsActive();
			}
		};
		struct LoadedModVisitor
		{
			LoadedModVisitor(std::function<bool(ModInfo*)> func) : modInfoVisitor(func) { }
			bool Accept(ModInfo * modInfo)
			{
				return modInfoVisitor(modInfo);
			}
			std::function<bool(ModInfo*)> modInfoVisitor;
		};

		UInt16 modCount = dhand->modList.modInfoList.CountIf(IsActiveFunctor());

		intfc->OpenRecord('PLGN', 0);
		intfc->WriteRecordData(&modCount, sizeof(modCount));

		_MESSAGE("Saving plugin list:");


		dhand->modList.modInfoList.Visit(LoadedModVisitor([&](ModInfo* modInfo)
		{
			if (modInfo && modInfo->IsActive())
			{
				intfc->WriteRecordData(&modInfo->modIndex, sizeof(modInfo->modIndex));
				if (modInfo->modIndex == 0xFE) {
					intfc->WriteRecordData(&modInfo->lightIndex, sizeof(modInfo->lightIndex));
				}

				UInt16 nameLen = strlen(modInfo->name);
				intfc->WriteRecordData(&nameLen, sizeof(nameLen));
				intfc->WriteRecordData(modInfo->name, nameLen);
				if (modInfo->modIndex != 0xFE)
				{
					_MESSAGE("\t[%d]\t%s", modInfo->modIndex, &modInfo->name);
				}
				else
				{
					_MESSAGE("\t[FE:%d]\t%s", modInfo->lightIndex, &modInfo->name);
				}
			}
			return true; // Continue
		}));
	}

	void LoadPluginList(SKSESerializationInterface * intfc)
	{
		DataHandler * dhand = DataHandler::GetSingleton();

		_MESSAGE("Loading plugin list:");

		char name[0x104] = { 0 };
		UInt16 nameLen = 0;

		UInt16 modCount = 0;
		intfc->ReadRecordData(&modCount, sizeof(modCount));
		for (UInt32 i = 0; i < modCount; i++)
		{
			UInt8 modIndex = 0xFF;
			UInt16 lightModIndex = 0xFFFF;
			intfc->ReadRecordData(&modIndex, sizeof(modIndex));
			if (modIndex == 0xFE) {
				intfc->ReadRecordData(&lightModIndex, sizeof(lightModIndex));
			}

			intfc->ReadRecordData(&nameLen, sizeof(nameLen));
			intfc->ReadRecordData(&name, nameLen);
			name[nameLen] = 0;

			UInt32 newIndex = 0xFF;
			UInt32 oldIndex = modIndex == 0xFE ? (0xFE000 | lightModIndex) : modIndex;

			const ModInfo * modInfo = dhand->LookupModByName(name);
			if (modInfo) {
				newIndex = modInfo->GetPartialIndex();
			}

			s_savedModIndexMap[oldIndex] = newIndex;

			_MESSAGE("\t(%d -> %d)\t%s", oldIndex, newIndex, name);
		}
	}

	UInt32 ResolveModIndex(UInt32 modIndex) {
		auto it = s_savedModIndexMap.find(modIndex);
		if (it != s_savedModIndexMap.end()) {
			return it->second;
		}
		return 0xFF;
	}



	UInt32 GetBaseID(UInt32 formID) { 
		if (formID == 0) return 0;
		return ((formID >> 24 == 0xFE) ? formID & 0x00000FFF : formID & 0x00FFFFFF);
	}
	UInt32 GetBaseID(TESForm* obj) {
		//return obj ? (obj->formID & 0x00FFFFFF) : 0;
		if (!obj) return 0;
		return ((obj->formID >> 24 == 0xFE) ? obj->formID & 0x00000FFF : obj->formID & 0x00FFFFFF);
	}

	UInt32 GetModIndex(UInt32 formID) {
		if (formID == 0) return 0;
		UInt32 modID = formID >> 24;
		if (modID == 0xFE) {
			modID = formID >> 12;
		}
		return modID;	
	}
	UInt32 GetModIndex(TESForm* obj) {
		if (obj->formID == 0) return 0;

		UInt32 modID = obj->formID >> 24;
		if (modID == 0xFE) {
			modID = obj->formID >> 12;
		}
		return modID;
	}
	
	UInt32 GetModIndex(const char* name) {
		DataHandler * dhand = DataHandler::GetSingleton();
		const ModInfo * modInfo = dhand->LookupModByName(name);
		if (modInfo) {
			UInt32 newIndex = modInfo->GetPartialIndex();
		}
		else {
			return 0xFF;
		}
	}



	//inline UInt8 ResolveModIndex(UInt8 modIndex) { return (modIndex < s_numSavefileMods) ? s_savefileIndexMap[modIndex] : 0xFF; }
	


	// Get current form ID from current load order
	UInt32 ResolveFormID(UInt32 formID) {
		/*if (formID == 0) return 0;
		UInt32 baseID = formID & 0x00FFFFFF;
		
		UInt8 oldMod = (UInt8)(formID >> 24);

		UInt32 newMod = ResolveModIndex(oldMod);
		if (newMod < 0 || newMod > 0xFF || (oldMod != 0xFF && newMod == 0xFF)) return 0;
		else return (((UInt32)newMod) << 24) | baseID;*/

		//_MESSAGE("ResolveFormID(0x%X)", (int)formID);

		if (formID == 0) return 0;
		UInt32 modID = formID >> 24;
	
		//_MESSAGE("\t- modID 1: %X", modID);

		// Light index
		if (modID == 0xFF) {
			//_MESSAGE("\t- modID FF: %X", formID);
			return formID; // FF
		}
		else if (modID == 0xFE) {
			modID = formID >> 12;
			//_MESSAGE("\t- modID light: %X", modID);
		}

		UInt32 loadedModID = ResolveModIndex(modID);
		if (loadedModID < 0xFF) {
			//_MESSAGE("\t- modID normal: %X", loadedModID);
			return (formID & 0x00FFFFFF) | (((UInt32)loadedModID) << 24); // ESP/ESM
		}
		else if (loadedModID > 0xFF) {
			//_MESSAGE("\t- modID bigger: %X", loadedModID);
			return (loadedModID << 12) | (formID & 0x00000FFF); // ESL
		}
		//_MESSAGE("\t- ended with 0");

		return 0;
	}

	TESForm* ResolveFormKey(UInt64 key){
		if (key < 1) return NULL;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = ResolveFormID((UInt32)(key));
		TESForm* form = id <= 0 ? NULL : LookupFormByID(id);
		return form ? (type == form->formType ? form : NULL) : NULL;
	}






	
	// Other
	/*int GameGetForm(int formId) {
		if (formId == 0) return formId;
		int getTesFormId = 0x451A30;
		int form = 0;
		_asm
		{
			mov ecx, 0xFF0000
				push formId
				call getTesFormId
				add esp, 4
				mov form, eax
		}
		return form;
	}*/

	bool IsValidObject(TESForm* obj, UInt64 formId) {
		_MESSAGE("IsValidObject(0x%X, 0x%X)", (int)obj->formID, formId);
		if (!obj || obj == NULL || formId == 0) return false;
		_MESSAGE("\tType: %d", (int)obj->formType);
		if ((UInt32)(formId >> 32) != 0 && obj->formType != (UInt32)(formId >> 32)) return false;
		else if ((formId & 0xFFFFFFFF) != 0 && (UInt32)(formId & 0xFFFFFFFF) != obj->formID) return false;
		else return true;
	}

	// Form Keys
	UInt64 GetFormKey(TESForm* form) {
		if (!form) return 0;
		UInt64 key = form->formID;
		key |= ((UInt64)form->formType) << 32;
		//_MESSAGE("GetFormKey(TESForm): FormID: 0x%X FormType: %llu FormKEY: 0x%X", (int)form->formID, (int)form->formType, (int)key);
		return key;
	}

	TESForm* GetFormKey(UInt64 key) {
		if (key < 1) return NULL;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = (UInt32)(key);
		return id != 0 ? LookupFormByID(id) : NULL;
	}

	UInt64 GetNewKey(UInt64 key) {
		if (key == 0) return 0;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = ResolveFormID((UInt32)(key));
		//_MESSAGE("GetNewKey(0x%X): Type: %lu ID: 0x%X", (int)key, type, (int)id);
		return (((UInt64)type) << 32) | (UInt64)id;
	}

	// Form Strings
	bool ends_with(const std::string &str, const std::string &suffix) {
		return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}


	bool IsFormString(const std::string &str) {
		if (str.size() < 4 || str.find("|") == std::string::npos) return false;
		else return ends_with(str, ".esp") || ends_with(str, ".esm") || ends_with(str, ".esl") || ends_with(str, ".FF");
	}

	const std::string GetFormString(TESForm* obj){
		if (!obj) return "0";
		 
		//_MESSAGE("GetFormString(0x%X)", (int)obj->formID);

		std::stringstream ss;
		UInt32 id = GetBaseID(obj);

		// Set hex base ID
		ss << "0x" << std::hex << id << "|";
				
		//TESFile *mod = obj->GetFinalSourceFile(); // GameForms.h, TESFile, unk_14()

		// Set Mod name
		UInt32 index = GetModIndex(obj);
		if (index < 0xFF) {
			// esp & esm objects
			DataHandler *Data = DataHandler::GetSingleton();
			ModInfo *modInfo = Data->modList.loadedMods[index];
			ss << modInfo->name;
		}
		else if (index > 0xFF) {
			// esl objects
			UInt32 light = index & 0x00FFF;
			DataHandler *Data = DataHandler::GetSingleton();
			ModInfo *modInfo = Data->modList.loadedCCMods[light];
			ss << modInfo->name;
		}
		else {
			// Temp objects - save form type as part of modname
			ss << (int)obj->formType << ".FF";
		}
		
		const std::string out = ss.str();

		//_MESSAGE("\tResult: %s", ss.str().c_str());

		return out;
	}

	// Light version of boost function. No error checking.
	/*template<typename T2, typename T1>
	inline T2 lexical_cast(const T1 &in) {
		T2 out;
		std::stringstream ss;
		ss << in;
		ss >> out;
		return out;
	}*/

	TESForm* ParseFormString(const std::string &objString) {
		if (!IsFormString(objString)) return NULL;

		//_MESSAGE("ParseFormString(%s)", objString.c_str());

		std::size_t pos = objString.find("|");
		std::string objID = objString.substr(0, pos).c_str();
		//_MESSAGE("\tobjID: %s", objID.c_str());
		//if (pos == std::string::npos) return NULL;

		UInt32 obj = 0;
		if (objString.rfind("0x", 0) == 0) {
			//_MESSAGE("\thex obj...");
			obj = std::stoul(objID, nullptr, 16);
		}
		else {
			//_MESSAGE("\tdec obj...");
			obj = atoi(objID.c_str());
		}

		//_MESSAGE("\tObject Result: 0x%X / %llu", (int)obj, obj);

		 
		std::string mod = objString.substr(pos + 1);

		if (ends_with(objString, ".FF")) {
			// Temp objects - check form type
			mod.resize((mod.length() - 3));
			//UInt8 type = atoi(mod.c_str());
			UInt8 type = std::stoi(mod.c_str(), nullptr, 16);
			obj = (((UInt32)0xFF) << 24) | obj;
			TESForm* objform = obj == 0 ? NULL : LookupFormByID(obj);
			if (objform && objform->formType == type) return objform;
			else {
				//_MESSAGE("Checking Form: 0x%X", (int)obj);
				//_MESSAGE("Type: %u - Expected: %u / 0x%X", type, objform->formType, (int)objform->formType);
				return NULL;
			}
		}
		else {
			// esp & esm objects
			DataHandler * dhand = DataHandler::GetSingleton();
			const ModInfo * modInfo = dhand->LookupModByName(mod.c_str());

			if (modInfo && modInfo->IsActive()) {
				obj = modInfo->GetFormID(obj);
				//_MESSAGE("\tObj: 0x%X", (int)obj);
			}
			else {
				//_MESSAGE("MOD IS INACTIVE!");
				return NULL; // Couldn't find mod.
			}
			/*
			_MESSAGE("\tMod Name: %s", modInfo->name);

			UInt32 index = 0xFF;
			if (modInfo) {
				index = modInfo->GetPartialIndex();
			}

			_MESSAGE("\tMod Index: 0x%X", (int)index);
			_MESSAGE("\tObj: 0x%X", (int)obj);

			if (index < 0xFF) {
				obj = (((UInt32)index) << 24) | obj;
				//_MESSAGE("\tObj ESP: 0x%X", (int)obj);
			}
			else {
				obj = (((UInt32)index << 12) | (obj & 0x00000FFF));
				//_MESSAGE("\tObj ESL: 0x%X", (int)obj);
			}
			//UInt8 index = DataHandler::GetSingleton()->GetLoadedModIndex(mod.c_str());
			*/
		}

		return obj == 0 ? NULL : LookupFormByID(obj);

	}

	/*TESForm* ParseForm(UInt32 &obj, const char* mod) {
		//UInt8 index = DataHandler::GetSingleton()->GetLoadedModIndex(mod);
		UInt32 index = 0xFF;
		DataHandler * dhand = DataHandler::GetSingleton();
		const ModInfo * modInfo = dhand->LookupModByName(mod);
		if (modInfo) {
			index = modInfo->GetPartialIndex();
		}

		if (index >= 0xFF) index = 0xFF;
		obj = (((UInt32)index) << 24) | obj;
		return obj == 0 ? NULL : LookupFormByID(obj);
	}*/
}
