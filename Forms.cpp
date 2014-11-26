#include "Forms.h"

#include <boost/algorithm/string.hpp>
#include <vector>

#include "skse/GameAPI.h"

namespace Forms {

	static UInt8 s_savefileIndexMap[0xFF];
	static UInt8 s_numSavefileMods = 0;

	void LoadModList(SKSESerializationInterface * intfc) {
		_MESSAGE("Loading mod list:");

		DataHandler *Data = DataHandler::GetSingleton();
		char name[0x104] = { 0 };
		UInt16 nameLen = 0;

		s_numSavefileMods = 0;
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

	inline UInt8 ResolveModIndex(UInt8 modIndex) { return (modIndex < s_numSavefileMods) ? s_savefileIndexMap[modIndex] : 0xFF; }
	//inline UInt8 ResolveModIndex(UInt32 formID) { return ResolveModIndex((UInt8)(formID >> 24)); }
	
	UInt32 ResolveFormID(UInt32 formID) {
		if (formID == 0) return 0;
		UInt32 baseID = formID & 0x00FFFFFF;
		UInt8 oldMod = (UInt8)(formID >> 24);
		UInt8 newMod = ResolveModIndex(oldMod);
		if (newMod < 0 || newMod > 0xFF || (oldMod != 0xFF && newMod == 0xFF)) return 0;
		else return (((UInt32)newMod) << 24) | baseID;
	}

	void LoadPreviousMods(std::stringstream &ss) {
		_MESSAGE("Loading mod list (legacy):");
		DataHandler *Data = DataHandler::GetSingleton();
		s_numSavefileMods = 0;
		for (int i = 0; i < 256; i++) {
			std::string mod;
			ss >> mod;
			if (mod.size() == 1 && mod[0] == (char)0x1B) mod.clear();
			else std::replace(mod.begin(), mod.end(), (char)0x7, ' ');
			s_numSavefileMods += mod.length() > 3 ? 1 : 0;

			UInt8 newIndex = Data->GetModIndex(mod.c_str());
			newIndex = newIndex >= 255 ? -1 : newIndex;

			s_savefileIndexMap[i] = newIndex;
			_MESSAGE("\tlegacy - (%d -> %d)\t%s", i, newIndex, mod.c_str());
		}
	}

	// Other
	int GameGetForm(int formId) {
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
	}

	bool IsValidObject(TESForm* obj, UInt64 formId) {
		if (obj == NULL) return false;
		else if ((UInt32)(formId >> 32) != 0 && obj->formType != (UInt32)(formId >> 32)) return false;
		else if ((formId & 0xFFFFFFFF) != 0 && (UInt32)(formId & 0xFFFFFFFF) != obj->formID) return false;
		else return true;
	}

	// Form Keys
	UInt64 GetFormKey(TESForm* form) {
		if (!form) return 0;
		UInt64 key = form->formID;
		key |= ((UInt64)form->formType) << 32;
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
		return (((UInt64)type) << 32) | (UInt64)id;
	}

	// Form Strings
	std::string GetFormString(TESForm* obj){
		if (!obj) return "0";
		DataHandler *Data = DataHandler::GetSingleton();
		ModInfo *modInfo = Data->modList.loadedMods[GetModIndex(obj)];
		if (!modInfo) return "0";
		UInt32 id   = GetBaseID(obj);
		std::stringstream ss;
		ss << id << "|" << modInfo->name;
		return ss.str();
	}

	TESForm* ParseFormString(std::string &objString){
		if (objString.empty() || objString.size() < 5) return NULL;
		std::vector<std::string> var;
		boost::split(var, objString, boost::is_any_of("|"));
		UInt32 obj = atoi(var[0].c_str());
		UInt8 index = DataHandler::GetSingleton()->GetModIndex(var[1].c_str());
		obj = (((UInt32)index) << 24) | obj;
		return obj == 0 ? NULL : LookupFormByID(obj);
	}
	

}
