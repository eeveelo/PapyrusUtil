#include "Forms.h"

#include <boost/algorithm/string.hpp>
#include <vector>

#include "skse64/GameAPI.h"

#include "skse64/GameRTTI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameReferences.h"

#include "skse64/Serialization.h"


namespace Forms {

	// ESP/ESM
	static UInt8 s_savefileIndexMap[0xFF];
	static UInt8 s_numSavefileMods = 0;

	// ESL
	static UInt16	s_saveLightfileIndexMap[0xFFF];
	static UInt16	s_numSaveLightfileMods = 0;

	void ClearModList() {
		for (UInt32 i = 0; i < s_numSavefileMods; i++) s_savefileIndexMap[i] = -1;
		s_numSavefileMods = 0;
		for (UInt32 i = 0; i < s_numSaveLightfileMods; i++) s_saveLightfileIndexMap[i] = -1;
		s_numSaveLightfileMods = 0;
	}


	void LoadModList(SKSESerializationInterface * intfc)
	{
		_MESSAGE("Loading mod list:");

		DataHandler * dhand = DataHandler::GetSingleton();

		char name[0x104] = { 0 };
		UInt16 nameLen = 0;

		intfc->ReadRecordData(&s_numSavefileMods, sizeof(s_numSavefileMods));
		for (UInt32 i = 0; i < s_numSavefileMods; i++)
		{
			intfc->ReadRecordData(&nameLen, sizeof(nameLen));
			intfc->ReadRecordData(&name, nameLen);
			name[nameLen] = 0;

			const ModInfo * modInfo = dhand->LookupModByName(name);
			if (modInfo) {
				UInt32 newIndex = modInfo->GetPartialIndex();
				s_savefileIndexMap[i] = newIndex;
				_MESSAGE("\t(%d -> %d)\t%s", i, newIndex, &name);
			}
			else {
				s_savefileIndexMap[i] = 0xFF;
				_MESSAGE("\t(%d -> UNLOADED)\t%s", i, &name);
			}

			//UInt8 newIndex = dhand->GetLoadedModIndex(name);
			//s_savefileIndexMap[i] = newIndex;
		}
	}

	void SaveModList(SKSESerializationInterface * intfc)
	{
		DataHandler * dhand = DataHandler::GetSingleton();
		UInt8 modCount = dhand->modList.loadedMods.count;

		intfc->OpenRecord('MODS', 0);
		intfc->WriteRecordData(&modCount, sizeof(modCount));

		_MESSAGE("Saving mod list:");

		for (UInt32 i = 0; i < modCount; i++)
		{
			ModInfo * modInfo = dhand->modList.loadedMods[i];
			UInt16 nameLen = strlen(modInfo->name);
			intfc->WriteRecordData(&nameLen, sizeof(nameLen));
			intfc->WriteRecordData(modInfo->name, nameLen);
			_MESSAGE("\t(%d)\t%s", i, &modInfo->name);
		}
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

			UInt8 newIndex = 255;
			const ModInfo * modInfo = Data->LookupModByName(mod.c_str());
			if (modInfo) {
				newIndex = modInfo->GetPartialIndex();
			}

			//UInt8 newIndex = Data->GetLoadedModIndex(mod.c_str());
			newIndex = (newIndex >= 255 || newIndex == -1) ? -1 : newIndex;

			s_savefileIndexMap[i] = newIndex;
			_MESSAGE("\tlegacy - (%d -> %d)\t%s", i, newIndex, mod.c_str());
		}
	}


	inline UInt32 GetModIndex(const char* name) {
		DataHandler * dhand = DataHandler::GetSingleton();
		const ModInfo * modInfo = dhand->LookupModByName(name);
		if (modInfo) {
			UInt32 newIndex = modInfo->GetPartialIndex();
		}
		else {
			return 0xFF;
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

	TESForm* ResolveFormKey(UInt64 key) {
		if (key < 1) return NULL;
		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = ResolveFormID((UInt32)(key));
		TESForm* form = id <= 0 ? NULL : LookupFormByID(id);
		return form ? (type == form->formType ? form : NULL) : NULL;
	}


	// ESL

	void LoadLightModList(SKSESerializationInterface * intfc)
	{
		// TODO
		/*_MESSAGE("Loading light mod list:");

		DataHandler * dhand = DataHandler::GetSingleton();

		char name[0x104] = { 0 };
		UInt16 nameLen = 0;

		intfc->ReadRecordData(&s_numSaveLightfileMods, sizeof(UInt16));

		for (UInt32 i = 0; i < s_numSaveLightfileMods; i++)
		{
			intfc->ReadRecordData(&nameLen, sizeof(nameLen));
			intfc->ReadRecordData(&name, nameLen);
			name[nameLen] = 0;

			UInt16 newIndex = dhand->GetLoadedLightModIndex(name);
			s_saveLightfileIndexMap[i] = newIndex;
			_MESSAGE("\t(%d -> %d)\t%s", i, newIndex, &name);
		}*/
	}

	void SaveLightModList(SKSESerializationInterface * intfc)
	{
		//TODO
		/*DataHandler * dhand = DataHandler::GetSingleton();
		UInt16 modCount = dhand->modList.loadedCCMods.count;

		intfc->OpenRecord('LIMD', 0);
		intfc->WriteRecordData(&modCount, sizeof(modCount));

		_MESSAGE("Saving light mod list:");

		for (UInt32 i = 0; i < modCount; i++)
		{
			ModInfo * modInfo = dhand->modList.loadedCCMods[i];
			UInt16 nameLen = strlen(modInfo->name);
			intfc->WriteRecordData(&nameLen, sizeof(nameLen));
			intfc->WriteRecordData(modInfo->name, nameLen);
			_MESSAGE("\t(%d)\t%s", i, &modInfo->name);
		}*/
	}

	UInt16 ResolveLightModIndex(UInt16 modIndex)
	{
		return (modIndex < s_numSaveLightfileMods) ? s_saveLightfileIndexMap[modIndex] : 0xFFFF;
	}
	// End ESL




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
	bool ends_with(const std::string &str, const std::string &suffix) {
		return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}


	bool IsFormString(const std::string &str) {
		if (str.size() < 4 || str.find("|") == std::string::npos) return false;
		else return ends_with(str, ".esp") || ends_with(str, ".esm") || ends_with(str, ".esl") || ends_with(str, ".FF");
	}

	const std::string GetFormString(TESForm* obj) {
		if (!obj) return "0";

		std::stringstream ss;
		UInt32 id = GetBaseID(obj);
		ss << id << "|";
		UInt8 index = GetModIndex(obj);
		if (index < 0xFF) {
			// esp & esm objects
			DataHandler *Data = DataHandler::GetSingleton();
			ModInfo *modInfo = Data->modList.loadedMods[GetModIndex(obj)];
			ss << modInfo->name;
		}
		else {
			// Temp objects - save form type as part of modname
			ss << (int)obj->formType << ".FF";
		}

		const std::string out = ss.str();
		return out;
	}

	TESForm* ParseFormString(const std::string &objString) {
		if (!IsFormString(objString)) return NULL;

		std::size_t pos = objString.find("|");
		//if (pos == std::string::npos) return NULL;

		UInt32 obj = atoi(objString.substr(0, pos).c_str());
		std::string mod = objString.substr(pos + 1);

		if (ends_with(objString, ".FF")) {
			// Temp objects - check form type
			mod.resize((mod.length() - 3));
			UInt8 type = atoi(mod.c_str());
			obj = (((UInt32)0xFF) << 24) | obj;
			TESForm* objform = obj == 0 ? NULL : LookupFormByID(obj);
			if (objform && objform->formType == type) return objform;
			else return NULL;
		}
		/* TODO else if (ends_with(objString, ".esl")) {
			UInt16 index = DataHandler::GetSingleton()->GetLoadedLightModIndex(mod.c_str());
			obj = (((UInt32)index) << 24) | obj;
			return obj == 0 ? NULL : LookupFormByID(obj);
		}*/
		else {
			// esp & esm objects
			UInt32 index = 0xFF;
			DataHandler * dhand = DataHandler::GetSingleton();
			const ModInfo * modInfo = dhand->LookupModByName(mod.c_str());
			if (modInfo) {
				index = modInfo->GetPartialIndex();
			}

			//UInt8 index = DataHandler::GetSingleton()->GetLoadedModIndex(mod.c_str());
			obj = (((UInt32)index) << 24) | obj;
			return obj == 0 ? NULL : LookupFormByID(obj);
		}


		/*std::vector<std::string> var;
		boost::split(var, objString, boost::is_any_of("|"));
		UInt32 obj = atoi(var[0].c_str());
		UInt8 index = DataHandler::GetSingleton()->GetModIndex(var[1].c_str());
		obj = (((UInt32)index) << 24) | obj;
		return obj == 0 ? NULL : LookupFormByID(obj);*/
	}

	TESForm* ParseForm(UInt32 &obj, const char* mod) {
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
	}
}
