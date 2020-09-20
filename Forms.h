#pragma once

#include <sstream>

#include "skse64/PluginAPI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameData.h"

namespace Forms {

	// Forms IDs
	//inline UInt32 GetBaseID(UInt32 formID) { return formID != 0 ? (formID & 0x00FFFFFF) : 0; }
	//inline UInt32 GetBaseID(TESForm* obj) { return obj ? (obj->formID & 0x00FFFFFF) : 0; }
	//inline UInt32 GetModIndex(UInt32 formID) { return ((int)formID) >> 24; }
	//inline UInt32 GetModIndex(TESForm* obj) { return ((int)obj->formID) >> 24; }
	//inline UInt32 GetModIndex(const char* name);


	UInt32 GetBaseID(UInt32 formID);
	UInt32 GetBaseID(TESForm* obj);
	UInt32 GetModIndex(UInt32 formID);
	UInt32 GetModIndex(TESForm* obj);
	UInt32 GetModIndex(const char* name);

	// Load Order
	void ClearModList();
	void SavePluginsList(SKSESerializationInterface * intfc);
	void LoadPluginList(SKSESerializationInterface * intfc);

	// Old
	void LoadModList(SKSESerializationInterface * intfc);

	//void SaveModList(SKSESerializationInterface * intfc);


	//void LoadLightModList(SKSESerializationInterface * intfc);
	//void SaveLightModList(SKSESerializationInterface * intfc);
	//void LoadPreviousMods(std::stringstream &ss);

	//UInt8 ResolveModIndex(UInt8 modIndex);
	//UInt32 ResolveModIndex(UInt32 modIndex);
	//UInt16 ResolveLightModIndex(UInt16 modIndex);
	UInt32 ResolveFormID(UInt32 formID);
	TESForm* ResolveFormKey(UInt64 key);

	// Form Keys
	UInt64 GetNewKey(UInt64 key);
	UInt64 GetNewKey(UInt64 key, std::string modName);
	UInt64 GetFormKey(TESForm* form);
	TESForm* GetFormKey(UInt64 key);
	
	inline UInt32 GetKeyType(UInt64 key) { return (UInt32)(key >> 32); }
	inline UInt32 GetKeyID(UInt64 key) { return (UInt32)(key); }

	// Form Strings
	bool IsFormString(const std::string &str);
	const std::string GetFormString(TESForm* obj);
	TESForm* ParseFormString(const std::string &objString);
	//TESForm* ParseForm(UInt32 &obj, const char* mod);

	// Misc
	int GameGetForm(int formId);
	bool IsValidObject(TESForm* obj, UInt64 formId);


	/*class FormStore {
	public:
		FormStore(TESForm*);
	};*/

}