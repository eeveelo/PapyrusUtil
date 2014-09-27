#include "Forms.h"

#include <boost/algorithm/string.hpp>
#include <vector>



namespace Forms {
	std::string CurrentMods[256];
	std::string PrevMods[256];

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
		if (oldId == 0)
			return 0;
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