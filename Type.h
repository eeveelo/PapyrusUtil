#pragma once

#include "skse/GameTypes.h"
#include "skse/GameData.h"

//#include "Lib.h"

namespace Type {

	struct FormPair {
		UInt32 id;
		const char* mod;
		TESForm* asForm() {
			if (id == 0 || !mod)
				return NULL;
			UInt8 index = DataHandler::GetSingleton()->GetModIndex(mod);
			UInt32 obj = (((UInt32)index) << 24) | id;
			return obj == 0 ? NULL : LookupFormByID(obj);
		}
	};

	inline UInt32 GetBaseID(TESForm* obj) {
		return obj ? (obj->formID & 0x00FFFFFF) : 0;
	}
	inline UInt32 GetBaseID(UInt32 formID) {
		return formID != 0 ? (formID & 0x00FFFFFF) : 0;
	}
	const char* GetModName(TESForm* obj) {
		if (!obj)
			return NULL;
		UInt8 order = ((int)obj->formID) >> 24;
		ModInfo* mod = DataHandler::GetSingleton()->modList.loadedMods[order];
		if (!mod)
			return NULL;
		return mod->name;
	}
	const char* GetModName(UInt32 formID) {
		if (formID == 0)
			return NULL;
		UInt8 order = ((int)formID) >> 24;
		ModInfo* mod = DataHandler::GetSingleton()->modList.loadedMods[order];
		if (!mod)
			return NULL;
		return mod->name;
	}
	FormPair MakeFormPair(TESForm* obj) {
		FormPair form;
		form.id = obj ? (obj->formID & 0x00FFFFFF) : 0;
		form.mod = GetModName(obj);
		return form;
	}
	FormPair MakeFormPair(UInt32 formID) {
		FormPair form;
		form.id = formID & 0x00FFFFFF;
		form.mod = GetModName(formID);
		return form;
	}

	char* DupeString(const char* value){
		unsigned int length = (unsigned int)strlen(value);
		char *newString = static_cast<char*>(malloc(length + 1));
		memcpy(newString, value, length);
		newString[length] = 0;
		return newString;
	}


	enum ValueType {
		nullValue = 0, ///< 'null' value
		intValue,      ///< SInt21
		floatValue,    ///< float
		stringValue,   ///< char*
		formValue,     ///< char*
	};

	class Value {
		union Data {
			SInt32 int_;
			float float_;
			char* string_;
			FormPair form_;
		} value_;
		ValueType type_ : 8;

		Value(ValueType type = nullValue);
		Value(SInt32 value);
		Value(float value);
		Value(BSFixedString value);
		Value(std::string value);
		Value(TESForm* value);
		Value(FormPair value);
		Value(UInt32 value);
	};

	Value::Value(SInt32 value) : type_(intValue) { value_.int_ = value; }
	Value::Value(float value) : type_(floatValue) { value_.float_ = value; }
	Value::Value(std::string value) : type_(stringValue) { value_.string_ = DupeString(value.c_str()); }
	Value::Value(BSFixedString value) : type_(stringValue) { value_.string_ = DupeString(value.data); }
	Value::Value(TESForm* value) : type_(formValue) { value_.form_.id = GetBaseID(value->formID); value_.form_.mod = GetModName(value); }
	Value::Value(FormPair value) : type_(formValue) { value_.form_ = value; }
	Value::Value(UInt32 value) : type_(formValue) { value_.form_.id = GetBaseID(value); value_.form_.mod = GetModName(value); }

}