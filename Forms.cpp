#include "Forms.h"

#include <boost/algorithm/string.hpp>
#include <vector>

#include "skse/GameAPI.h"


namespace Forms {

	std::string CurrentMods[256];
	std::string PrevMods[256];


	static UInt8 s_savefileIndexMap[0xFF];
	static UInt8 s_numSavefileMods = 0;

	void LoadModList(SKSESerializationInterface * intfc) {
		_MESSAGE("Loading mod list:");
		DataHandler *Data = DataHandler::GetSingleton();
		char name[0x104] = { 0 };
		UInt16 nameLen = 0;
		intfc->ReadRecordData(&s_numSavefileMods, sizeof(s_numSavefileMods));
		for (UInt32 i = 0; i < s_numSavefileMods; i++) {
			intfc->ReadRecordData(&nameLen, sizeof(nameLen));
			intfc->ReadRecordData(&name, nameLen);

			name[nameLen] = 0;
			UInt8 newIndex = Data->GetModIndex(name);
			s_savefileIndexMap[i] = newIndex;

			_MESSAGE("\t(%d -> %d)\t%s", i, newIndex, &name);
		}

	}

	void SaveModList(SKSESerializationInterface * intfc) {
		DataHandler *Data = DataHandler::GetSingleton();
		UInt8 modCount = Data->modList.loadedModCount;

		intfc->OpenRecord('MODS', 0);
		intfc->WriteRecordData(&modCount, sizeof(modCount));

		_MESSAGE("Saving mod list:");

		for (UInt32 i = 0; i < modCount; i++) {
			ModInfo * modInfo = Data->modList.loadedMods[i];
			UInt16 nameLen = strlen(modInfo->name);
			intfc->WriteRecordData(&nameLen, sizeof(nameLen));
			intfc->WriteRecordData(modInfo->name, nameLen);
			_MESSAGE("\t(%d)\t%s", i, &modInfo->name);
		}
	}

	UInt8 ResolveModIndex(UInt8 modIndex) {
		return (modIndex < s_numSavefileMods) ? s_savefileIndexMap[modIndex] : 0xFF;
	}

	UInt32 ResolveFormID(UInt32 formID) {
		if (formID == 0) return 0;
		UInt32 baseID = formID & 0x00FFFFFF;
		UInt8 oldMod = (UInt8)(formID >> 24);
		UInt8 newMod = ResolveModIndex(oldMod);
		if (newMod < 0 || (oldMod != 0xFF && newMod == 0xFF)) return 0;
		else return (((UInt32)newMod) << 24) | baseID;
	}









	void LoadCurrentMods() {
		if (CurrentMods[0].size() > 3)
			return;
		DataHandler* Data = DataHandler::GetSingleton();
		if (Data != NULL) {
			for (int i = 0; i < 256; ++i)
				CurrentMods[i] = Data->modList.loadedMods[i] ? Data->modList.loadedMods[i]->name : "";
		}
	}

	void SaveCurrentMods(std::stringstream &ss) {
		for (int i = 0; i < 256; ++i) {
			std::string mod = CurrentMods[i];
			if (mod.empty())
				mod += (char)0x1B;
			else
				std::replace(mod.begin(), mod.end(), ' ', (char)0x7);
			ss << ' ' << mod;
		}
	}

	void LoadPreviousMods(std::stringstream &ss) {
		//int ver;
		//ss >> ver;
		for (int i = 0; i < 256; i++) {
			std::string mod;
			ss >> mod;
			//std::stringstream ms;
			//ms << i << " => " << mod;
			//_MESSAGE(ms.str().c_str());
			if (mod.size() == 1 && mod[0] == (char)0x1B)
				mod.clear();
			else
				std::replace(mod.begin(), mod.end(), (char)0x7, ' ');
			PrevMods[i] = mod;
		}
		_MESSAGE(PrevMods[0].c_str());
		_MESSAGE(PrevMods[1].c_str());
	}
	
	void ClearPreviousMods(){
		for (unsigned int i = 0; i < 256; ++i)
			PrevMods[i] = "";
	}

	// Forms
	inline UInt32 GetBaseID(TESForm* obj) { return obj ? (obj->formID & 0x00FFFFFF) : 0; }
	inline UInt32 GetBaseID(UInt32 formID) { return formID != 0 ? (formID & 0x00FFFFFF) : 0; }
	inline std::string GetFormOwner(TESForm* obj){ return CurrentMods[((int)obj->formID) >> 24]; }
	inline std::string GetFormOwner(UInt32 obj){ return CurrentMods[((int)obj) >> 24]; }

	UInt8 GetNewOrder(UInt8 order) {
		if (order == 0xFF || order == 0 || !_stricmp(CurrentMods[order].c_str(), PrevMods[order].c_str()))
			return order;
		for (int i = 0; i < 256; i++) {
			if (!_stricmp(CurrentMods[i].c_str(), PrevMods[order].c_str()))
				return (UInt8)i;
		}
		return 0xFF;
	}

	UInt8 GetNewOrder(std::string modName) {
		for (int i = 0; i < 256; i++) {
			if (!_stricmp(CurrentMods[i].c_str(), modName.c_str()))
				return (UInt8)i;
		}
		return 0xFF;
	}

	UInt32 GetNewId(UInt32 oldId, std::string modName) {
		if (oldId == 0)
			return 0;
		UInt32 id = oldId & 0x00FFFFFF;
		UInt8 oldMod = (UInt8)(oldId >> 24);
		UInt8 mod = GetNewOrder(modName);
		if (oldMod != 0xFF && mod == 0xFF)
			return 0;
		return (((UInt32)mod) << 24) | id;
	}

	UInt32 GetNewId(UInt32 oldId){
		if (oldId == 0) return 0;
		UInt32 id = oldId & 0x00FFFFFF;
		UInt8 oldMod = (UInt8)(oldId >> 24);
		UInt8 mod = GetNewOrder(oldMod);
		if (oldMod != 0xFF && mod == 0xFF)
			return 0;
		return (((UInt32)mod) << 24) | id;
	}

	// Form Keys
	UInt64 GetFormKey(TESForm* form) {
		if (!form)
			return 0;
		UInt64 key = form->formID;
		key |= ((UInt64)form->formType) << 32;
		return key;
	}

	TESForm* GetFormKey(UInt64 key) {
		if (key < 1)
			return NULL;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = (UInt32)(key);
		return id != 0 ? LookupFormByID(id) : NULL;
	}

	UInt64 GetNewKey(UInt64 key) {
		if (key == 0)
			return 0;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = GetNewId((UInt32)(key));
		//std::stringstream ss;
		//int bid = (UInt32)(key);
		//ss << "key: " << key << " - type: " << type << " - id: " << bid << " - new: ";
		//ss << id;
		//_MESSAGE(ss.str().c_str());
		return (((UInt64)type) << 32) | (UInt64)id;
	}

	UInt64 GetNewKey(UInt64 key, std::string modName) {
		if (key == 0)
			return 0;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = GetNewId((UInt32)(key), modName);
		return (((UInt64)type) << 32) | (UInt64)id;
	}

	// Form Strings
	std::string GetFormString(TESForm* obj){
		if (!obj)
			return "0";
		std::stringstream ss;
		int order = ((int)obj->formID) >> 24;
		UInt32 id = obj ? (obj->formID & 0x00FFFFFF) : 0;
		ss << id << "|" << CurrentMods[order];
		return ss.str();
	}

	TESForm* ParseFormString(std::string &objString){
		if (objString.empty() || objString.size() < 3)
			return NULL;
		std::vector<std::string> var;
		boost::split(var, objString, boost::is_any_of("|"));
		UInt32 obj = atoi(var[0].c_str());
		UInt8 index = DataHandler::GetSingleton()->GetModIndex(var[1].c_str());
		obj = (((UInt32)index) << 24) | obj;
		return obj == 0 ? NULL : LookupFormByID(obj);
	}


	// Other
	int GameGetForm(int formId) {
		if (formId == 0)
			return 0;
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
	}

	bool IsValidObject(TESForm* obj, UInt64 formId) {
		if (obj == NULL)
			return false;
		else if ((UInt32)(formId >> 32) != 0 && obj->formType != (UInt32)(formId >> 32))
			return false;
		else if ((formId & 0xFFFFFFFF) != 0 && (UInt32)(formId & 0xFFFFFFFF) != obj->formID)
			return false;
		return true;
	}

}

/*

#include "Forms.h"

#include <boost/algorithm/string.hpp>
#include <vector>

namespace Forms {

	static UInt8 s_savefileIndexMap[0xFF];
	static UInt8 s_numSavefileMods = 0;

	void LoadModList(SKSESerializationInterface * intfc) {
		_MESSAGE("Loading mod list:");

		DataHandler *Data = DataHandler::GetSingleton();

		char name[0x104] ={ 0 };
		UInt16 nameLen = 0;

		intfc->ReadRecordData(&s_numSavefileMods, sizeof(s_numSavefileMods));
		for (UInt32 i = 0; i < s_numSavefileMods; i++) {
			intfc->ReadRecordData(&nameLen, sizeof(nameLen));
			intfc->ReadRecordData(&name, nameLen);
			name[nameLen] = 0;

			UInt8 newIndex = Data->GetModIndex(name);
			s_savefileIndexMap[i] = newIndex;

			_MESSAGE("\t(%d -> %d)\t%s", i, newIndex, &name);
		}
	}

	void SaveModList(SKSESerializationInterface * intfc) {
		DataHandler *Data = DataHandler::GetSingleton();
		UInt8 modCount = Data->modList.loadedModCount;

		intfc->OpenRecord('MODS', 0);
		intfc->WriteRecordData(&modCount, sizeof(modCount));

		_MESSAGE("Saving mod list:");

		for (UInt32 i = 0; i < modCount; i++) {
			ModInfo * modInfo = Data->modList.loadedMods[i];
			UInt16 nameLen = strlen(modInfo->name);
			intfc->WriteRecordData(&nameLen, sizeof(nameLen));
			intfc->WriteRecordData(modInfo->name, nameLen);
			_MESSAGE("\t(%d)\t%s", i, &modInfo->name);
		}
	}

	UInt8 ResolveModIndex(UInt8 modIndex) {
		return (modIndex < s_numSavefileMods) ? s_savefileIndexMap[modIndex] : 0xFF;
	}
	UInt8 ResolveModIndex(std::string modName) {
		DataHandler *Data = DataHandler::GetSingleton();
		UInt8 newIndex = Data->GetModIndex(modName.c_str());
	}


	// Forms
	inline UInt32 GetBaseID(TESForm* obj) { return obj ? (obj->formID & 0x00FFFFFF) : 0; }
	inline UInt32 GetBaseID(UInt32 formID) { return formID != 0 ? (formID & 0x00FFFFFF) : 0; }
	std::string GetFormOwner(TESForm* obj){ return CurrentMods[((int)obj->formID) >> 24]; }
	std::string GetFormOwner(UInt32 obj){
		DataHandler *Data = DataHandler::GetSingleton();
		ModInfo  *modInfo = Data->modList.loadedMods[ResolveModIndex(obj >> 24)];
		return modInfo ? modInfo->name : "";
	}

	UInt8 GetNewOrder(std::string modName) {
		for (int i = 0; i < 256; i++) {
			if (!_stricmp(CurrentMods[i].c_str(), modName.c_str()))
				return (UInt8)i;
		}
		return 0xFF;
	}

	UInt32 GetNewId(UInt32 oldId, std::string modName) {
		if (oldId == 0)
			return 0;
		UInt32 id = oldId & 0x00FFFFFF;
		UInt8 oldMod = (UInt8)(oldId >> 24);
		UInt8 mod = GetNewOrder(modName);
		if (oldMod != 0xFF && mod == 0xFF)
			return 0;
		return (((UInt32)mod) << 24) | id;
	}

	UInt32 GetNewId(UInt32 oldId){
		if (oldId == 0) return 0;
		UInt32 id = oldId & 0x00FFFFFF;
		UInt8 oldMod = (UInt8)(oldId >> 24);
		UInt8 mod = GetNewOrder(oldMod);
		if (oldMod != 0xFF && mod == 0xFF)
			return 0;
		return (((UInt32)mod) << 24) | id;
	}

	// Form Keys
	UInt64 GetFormKey(TESForm* form) {
		if (!form)
			return 0;
		UInt64 key = form->formID;
		key |= ((UInt64)form->formType) << 32;
		return key;
	}

	TESForm* GetFormKey(UInt64 key) {
		if (key < 1)
			return NULL;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = (UInt32)(key);
		return id != 0 ? LookupFormByID(id) : NULL;
	}

	UInt64 GetNewKey(UInt64 key) {
		if (key == 0)
			return 0;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = GetNewId((UInt32)(key));
		//std::stringstream ss;
		//int bid = (UInt32)(key);
		//ss << "key: " << key << " - type: " << type << " - id: " << bid << " - new: ";
		//ss << id;
		//_MESSAGE(ss.str().c_str());
		return (((UInt64)type) << 32) | (UInt64)id;
	}

	UInt64 GetNewKey(UInt64 key, std::string modName) {
		if (key == 0)
			return 0;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = GetNewId((UInt32)(key), modName);
		return (((UInt64)type) << 32) | (UInt64)id;
	}

	// Form Strings
	std::string GetFormString(TESForm* obj){
		if (!obj)
			return "0";
		std::stringstream ss;
		int order = ((int)obj->formID) >> 24;
		UInt32 id = obj ? (obj->formID & 0x00FFFFFF) : 0;
		ss << id << "|" << CurrentMods[order];
		return ss.str();
	}

	TESForm* ParseFormString(std::string &objString){
		if (objString.empty() || objString.size() < 3)
			return NULL;
		std::vector<std::string> var;
		boost::split(var, objString, boost::is_any_of("|"));
		UInt32 obj = atoi(var[0].c_str());
		UInt8 index = DataHandler::GetSingleton()->GetModIndex(var[1].c_str());
		obj = (((UInt32)index) << 24) | obj;
		return obj == 0 ? NULL : LookupFormByID(obj);
	}


	// Other
	int GameGetForm(int formId) {
		if (formId == 0)
			return 0;
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
	}

	bool IsValidObject(TESForm* obj, UInt64 formId) {
		if (obj == NULL)
			return false;
		else if ((UInt32)(formId >> 32) != 0 && obj->formType != (UInt32)(formId >> 32))
			return false;
		else if ((formId & 0xFFFFFFFF) != 0 && (UInt32)(formId & 0xFFFFFFFF) != obj->formID)
			return false;
		return true;
	}

}


/*

std::string CurrentMods[256];
std::string PrevMods[256];


void SaveCurrentMods(std::stringstream &ss) {
for (int i = 0; i < 256; ++i) {
std::string mod = CurrentMods[i];
if (mod.empty())
mod += (char)0x1B;
else
std::replace(mod.begin(), mod.end(), ' ', (char)0x7);
ss << ' ' << mod;
}
}

void LoadPreviousMods(std::stringstream &ss) {
//int ver;
//ss >> ver;
for (int i = 0; i < 256; i++) {
std::string mod;
ss >> mod;
//std::stringstream ms;
//ms << i << " => " << mod;
//_MESSAGE(ms.str().c_str());
if (mod.size() == 1 && mod[0] == (char)0x1B)
mod.clear();
else
std::replace(mod.begin(), mod.end(), (char)0x7, ' ');
PrevMods[i] = mod;
}
_MESSAGE(PrevMods[0].c_str());
_MESSAGE(PrevMods[1].c_str());
}

void ClearPreviousMods(){
for (unsigned int i = 0; i < 256; ++i)
PrevMods[i] = "";
}

*/