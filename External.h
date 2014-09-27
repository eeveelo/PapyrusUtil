#pragma once

#include <json/json.h>
//#define JSON_VALUE_USE_INTERNAL_MAP

#include "common/ICriticalSection.h"

#include "skse/PluginAPI.h"
#include "skse/GameAPI.h"
#include "skse/GameTypes.h"
#include "skse/GameData.h"
#include "skse/PapyrusArgs.h"

#include "Forms.h"

namespace External {
	typedef Json::Value Value;

	class ExternalFile{
	private:
		Value root;
		Json::Reader reader;
		ICriticalSection s_dataLock;
	public:
		std::string name;
		bool isModified;
		bool styledWrite;
		ExternalFile(std::string doc) : isModified(false), styledWrite(true) { name = doc; LoadFile(); }
		ExternalFile(Value doc, bool save) : isModified(false), styledWrite(true){ root = doc; if(save) SaveFile(); }

		inline bool HasKey(std::string &type, std::string &key){ return root.isMember(type) && root[type].isMember(key); }

		// Global key=>value
		
		//inline Value* GetMember(std::string &type, std::string &key){
		//	return root.isMember(type) ? &root[type].get(key, NULL) : NULL;
		//}
		
		template <typename T> T SetValue(std::string key, T value);
		template <typename T> T GetValue(std::string key, T missing);
		template <typename T> T AdjustValue(std::string key, T value);

		bool UnsetValue(std::string type, std::string &key);
		template <typename T> inline bool UnsetValue(std::string key){ return false; }
		template <> inline bool UnsetValue<SInt32>(std::string key){ return UnsetValue("int", key); }
		template <> inline bool UnsetValue<float>(std::string key){ return UnsetValue("float", key); }
		template <> inline bool UnsetValue<BSFixedString>(std::string key){ return UnsetValue("string", key); }
		template <> inline bool UnsetValue<TESForm*>(std::string key){ return UnsetValue("form", key); }

		bool HasValue(std::string type, std::string &key);
		template <typename T> inline bool HasValue(std::string key){ return false; }
		template <> inline bool HasValue<SInt32>(std::string key){ return HasValue("int", key); }
		template <> inline bool HasValue<float>(std::string key){ return HasValue("float", key); }
		template <> inline bool HasValue<BSFixedString>(std::string key){ return HasValue("string", key); }
		template <> inline bool HasValue<TESForm*>(std::string key){ return HasValue("form", key); }

		template <typename T> int ListAdd(std::string key, T value, bool allowDuplicate);
		template <typename T> T ListGet(std::string key, int index);
		template <typename T> T ListSet(std::string key, int index, T value);
		template <typename T> T ListAdjust(std::string key, int index, T value);
		template <typename T> int ListRemove(std::string key, T value, bool allInstances);
		template <typename T> bool ListRemoveAt(std::string key, int index);
		template <typename T> bool ListInsertAt(std::string key, int index, T value);
		template <typename T> int ListClear(std::string key);
		template <typename T> int ListCount(std::string key);
		template <typename T> int ListFind(std::string key, T value);
		template <typename T> bool ListHas(std::string key, T value);
		template <typename T> void ListSlice(std::string key, VMArray<T> Output, int startIndex);
		template <typename T> int ListResize(std::string key, int length, T filler);
		template <typename T> bool ListCopy(std::string key, VMArray<T> Input);

		// Special case for strings
		int ListRemove(std::string key, BSFixedString value, bool allInstances);
		int ListFind(std::string key, BSFixedString value);
		bool ListHas(std::string key, BSFixedString value);

		// Parse values
		template<typename T>
		inline Value MakeValue(T v) { return Value(v); }
		template <> inline Value MakeValue<SInt32>(SInt32 v) { return Value::Int(v); }
		template <> inline Value MakeValue<float>(float v) { return Value(v); }
		template <> inline Value MakeValue<BSFixedString>(BSFixedString v) { return Value(v.data); }
		template <> inline Value MakeValue<TESForm*>(TESForm* v) { return Value(Forms::GetFormString(v)); }

		template <typename T>
		inline T ParseValue(Value value, T missing){ return; };
		template <> inline SInt32 ParseValue<SInt32>(Value value, SInt32 missing){ return value.isNull() ? missing : value.asInt(); }
		template <> inline float ParseValue<float>(Value value, float missing){ return value.isNull() ? missing : value.asFloat(); }
		template <> inline BSFixedString ParseValue<BSFixedString>(Value value, BSFixedString missing){ return value.isNull() ? missing : BSFixedString(value.asCString()); }
		template <> inline TESForm* ParseValue<TESForm*>(Value value, TESForm* missing){ return value.isNull() ? missing : Forms::ParseFormString(value.asString()); }

		template <typename T>
		inline T ParseValue(Value value){ T t = T(); return ParseValue<T>(value, t); }
		template <> inline SInt32 ParseValue<SInt32>(Value value){ return ParseValue<SInt32>(value, 0); }
		template <> inline float ParseValue<float>(Value value){ return ParseValue<float>(value, 0.0f); }
		template <> inline BSFixedString ParseValue<BSFixedString>(Value value){ return ParseValue<BSFixedString>(value, BSFixedString("")); }
		template <> inline TESForm* ParseValue<TESForm*>(Value value){ return ParseValue<TESForm*>(value, NULL); }

		// value type keys
		template<typename T> inline std::string Type(){ return "x"; }
		template <> inline std::string Type<SInt32>(){ return "int"; }
		template <> inline std::string Type<float>(){ return "float"; }
		template <> inline std::string Type<BSFixedString>(){ return "string"; }
		template <> inline std::string Type<TESForm*>(){ return "form"; }

		template<typename T> inline std::string List(){ return "x"; }
		template <> inline std::string List<SInt32>(){ return "intList"; }
		template <> inline std::string List<float>(){ return "floatList"; }
		template <> inline std::string List<BSFixedString>(){ return "stringList"; }
		template <> inline std::string List<TESForm*>(){ return "formList"; }

		// read/write
		bool LoadFile();
		bool SaveFile();
		bool SaveFile(bool styled);
		void RevertFile();
		void CopyFileTo(std::string copyTo);
		void ClearAll();

	};

	typedef std::vector<ExternalFile*> FileVector;
	ExternalFile* GetFile(std::string name);
	ExternalFile* GetSingleton();
	bool LoadGlobalFile();
	bool SaveGlobalFile();

	void SaveFiles();
	void RevertFiles();

}
