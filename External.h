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
	typedef Json::Path Path;

	template<typename T>
	inline Value MakeValue(T v) { return Value(v); }
	template <> inline Value MakeValue<SInt32>(SInt32 v) { return Value::Int(v); }
	template <> inline Value MakeValue<float>(float v) { return Value(v); }
	template <> inline Value MakeValue<BSFixedString>(BSFixedString v) { return Value(v.data); }
	template <> inline Value MakeValue<TESForm*>(TESForm* v) { return Value(Forms::GetFormString(v)); }
	//template <> inline Value MakeValue<TESForm*>(TESForm* v) { return Value(Forms::GetFormString(v)); }

	template <typename T>
	inline T ParseValue(Value value, T missing){ return; };
	template <> inline SInt32 ParseValue<SInt32>(Value value, SInt32 missing){ return (value.isNull() || !value.isConvertibleTo(Json::intValue)) ? missing : value.asInt(); }
	template <> inline float ParseValue<float>(Value value, float missing){ return (value.isNull() || !value.isConvertibleTo(Json::realValue)) ? missing : value.asFloat(); }
	template <> inline BSFixedString ParseValue<BSFixedString>(Value value, BSFixedString missing){
		return value.isString() ? BSFixedString(value.asCString()) : (!value.isNull() && value.isConvertibleTo(Json::stringValue)) ? BSFixedString(value.asString().c_str()) : missing;
	}
	template <> inline TESForm* ParseValue<TESForm*>(Value value, TESForm* missing) {
		return value.isString() ? Forms::ParseFormString(value.asString()) : missing;
	}
	
	/*template <> inline TESForm* ParseValue<TESForm*>(Value value, TESForm* missing) {
		if (value.isNull()) return missing;
		else if (value.isString()) return Forms::ParseFormString(value.asString());
		else if (value.isArray() && value.size() == 2) {
			return Forms::ParseForm(value.get(Value::ArrayIndex(0), Value::null).asInt(), value.get(1, Value::null).asCString());
		}
		else return missing;
	}*/

	template <typename T>
	inline T ParseValue(Value value){ T t = T(); return ParseValue<T>(value, t); }
	template <> inline SInt32 ParseValue<SInt32>(Value value){ return ParseValue<SInt32>(value, 0); }
	template <> inline float ParseValue<float>(Value value){ return ParseValue<float>(value, 0.0f); }
	template <> inline BSFixedString ParseValue<BSFixedString>(Value value){ return ParseValue<BSFixedString>(value, BSFixedString("")); }
	template <> inline TESForm* ParseValue<TESForm*>(Value value){ return ParseValue<TESForm*>(value, NULL); }

	// value type keys
	template<typename T> inline std::string const Type(){ return "x"; }
	template <> inline std::string const Type<SInt32>(){ return "int"; }
	template <> inline std::string const Type<float>(){ return "float"; }
	template <> inline std::string const Type<BSFixedString>(){ return "string"; }
	template <> inline std::string const Type<TESForm*>(){ return "form"; }

	template<typename T> inline const std::string List(){ return "x"; }
	template <> inline std::string const List<SInt32>(){ return "intList"; }
	template <> inline std::string const List<float>(){ return "floatList"; }
	template <> inline std::string const List<BSFixedString>(){ return "stringList"; }
	template <> inline std::string const List<TESForm*>(){ return "formList"; }
	
	class ExternalFile{
	private:
		Value root;
		Json::Reader reader;
		ICriticalSection s_dataLock;
	public:
		std::string name;
		std::string docpath;
		bool isModified;
		bool minify;

		ExternalFile(std::string doc) : isModified(false), minify(false) { name = doc; docpath = "Data\\SKSE\\Plugins\\StorageUtilData\\" + doc; LoadFile(); } //reader = Json::Features::strictMode(); 

		inline bool HasKey(std::string &type, std::string &key){ return root.isMember(type) && root[type].isMember(key); }
	
		void SetValue(std::string type, std::string key, Value value);
		Value GetValue(std::string type, std::string key, Value value);
		Value AdjustValue(std::string type, std::string key, Value value);
		bool UnsetValue(std::string type, std::string key);
		bool HasValue(std::string type, std::string key);

		int ListAdd(std::string type, std::string key, Value value, bool allowDuplicate);
		Value ListGet(std::string type, std::string key, int index);
		Value ListSet(std::string type, std::string key, int index, Value value);
		int ListRemove(std::string type, std::string key, Value removing, bool allInstances);
		bool ListRemoveAt(std::string type, std::string key, int index);
		bool ListInsertAt(std::string type, std::string key, int index, Value value);
		int ListClear(std::string type, std::string key);
		int ListCount(std::string type, std::string key);
		int ListCountValue(std::string type, std::string key, Value value, bool exclude);
		int ListFind(std::string type, std::string key, Value value);
		bool ListHas(std::string type, std::string key, Value value);
		int ListResize(std::string type, std::string key, int length, Value filler);

		template <typename T> T ListAdjust(std::string key, int index, T adjustBy);
		template <typename T> void ListSlice(std::string key, VMArray<T> Output, int startIndex);
		template <typename T> bool ListCopy(std::string key, VMArray<T> Input);
		template <typename T> VMResultArray<T> ToArray(std::string key);


		inline Value Resolve(std::string pathto) { Path path(pathto.front() != '.' ? '.' + pathto : pathto); return path.resolve(root, Value(Json::nullValue)); }
		inline Value Resolve(std::string pathto, Value missing) { Path path(pathto.front() != '.' ? '.' + pathto : pathto); return path.resolve(root, missing); }

		template <typename T> void SetPathValue(std::string path, Value var);
		template <typename T> T GetPathValue(std::string path, T defaultValue);
		template <typename T> VMResultArray<T> PathElements(std::string path, T invalidType);
		VMResultArray<BSFixedString> PathMembers(std::string path);
		int PathCount(std::string path);

		//Value ResolveValue(std::string path, Value value);
		bool CanResolve(std::string path);
		bool IsObject(std::string path);
		bool IsArray(std::string path);
		bool IsString(std::string path);
		bool IsNumber(std::string path);
		bool IsBool(std::string path);
		bool IsForm(std::string path);

		int CountPrefix(std::string type, std::string prefix);
		//int ClearPrefix(std::string type, std::string prefix);

		//bool ExternalFile::ImportOnForm(TESForm* FormRef);

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
	bool RevertFile(std::string name, bool savechanges, bool minify);
	bool ChangesPending(std::string name);
	void SaveFiles();
	void RevertFiles();

#ifdef _GLOBAL_EXTERNAL
	ExternalFile* GetSingleton();
	bool LoadGlobalFile();
	bool SaveGlobalFile();
#endif

}
