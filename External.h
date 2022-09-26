#pragma once

#include <json/json.h>
//#define JSON_VALUE_USE_INTERNAL_MAP

//#include<ctime>

#define OLD_STYLE_JSON 1
//#define NEW_STYLE_JSON 1

#include "common/ICriticalSection.h"

#include "skse64/PluginAPI.h"
#include "skse64/GameAPI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameData.h"
#include "skse64/PapyrusArgs.h"

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


	/*template <typename T>
	inline T ParseValue(Value value, T missing) { return; };
	template <> inline SInt32 ParseValue<SInt32>(Value value, SInt32 missing) { return (value.isNull() || !value.isConvertibleTo(Json::intValue)) ? missing : value.asInt(); }
	template <> inline float ParseValue<float>(Value value, float missing) { return (value.isNull() || !value.isConvertibleTo(Json::realValue)) ? missing : value.asFloat(); }
	template <> inline BSFixedString ParseValue<BSFixedString>(Value value, BSFixedString missing) {
		return value.isString() ? BSFixedString(value.asCString()) : (!value.isNull() && value.isConvertibleTo(Json::stringValue)) ? BSFixedString(value.asString().c_str()) : missing;
	}
	template <> inline TESForm* ParseValue<TESForm*>(Value value, TESForm* missing) {
		return value.isString() ? Forms::ParseFormString(value.asString()) : missing;
	}*/

	template <typename T>
	inline T ParseValue(Value value, T missing){ return; };
	template <> inline SInt32 ParseValue<SInt32>(Value value, SInt32 missing){ return value.isNull() ? missing : value.asInt(); }
	template <> inline float ParseValue<float>(Value value, float missing){ return value.isNull() ? missing : value.asFloat(); }
	template <> inline BSFixedString ParseValue<BSFixedString>(Value value, BSFixedString missing){ return value.isString() ? BSFixedString(value.asCString()) : missing; }
	template <> inline TESForm* ParseValue<TESForm*>(Value value, TESForm* missing) { return value.isNull() ? missing : Forms::ParseFormString(value.asString()); }
	
	template <typename T>
	inline T ParseValue(Value value){ T t = T(); return ParseValue<T>(value, t); }
	template <> inline SInt32 ParseValue<SInt32>(Value value){ return ParseValue<SInt32>(value, 0); }
	template <> inline float ParseValue<float>(Value value){ return ParseValue<float>(value, 0.0f); }
	template <> inline BSFixedString ParseValue<BSFixedString>(Value value){ return ParseValue<BSFixedString>(value, BSFixedString("")); }
	template <> inline TESForm* ParseValue<TESForm*>(Value value){ return ParseValue<TESForm*>(value, NULL); }

	// value type keys
	template<typename T> inline const char* Type(){ return "x"; }
	template <> inline const char* Type<SInt32>(){ return "int"; }
	template <> inline const char* Type<float>(){ return "float"; }
	template <> inline const char* Type<BSFixedString>(){ return "string"; }
	template <> inline const char* Type<TESForm*>(){ return "form"; }

	template<typename T> inline const char* List(){ return "x"; }
	template <> inline const char* List<SInt32>(){ return "intList"; }
	template <> inline const char* List<float>(){ return "floatList";}
	template <> inline const char* List<BSFixedString>(){ return "stringList"; }
	template <> inline const char* List<TESForm*>(){ return "formList"; }
	

	class ExternalFile{
	private:
		Value root;
		ICriticalSection s_dataLock;
		//Json::Reader reader;
		Json::CharReaderBuilder reader;
		std::string readErrors;
	public:
		std::string name;
		std::string docpath;
		//std::time_t opened;
		bool isModified;
		bool isLoaded;
		bool minify;

		ExternalFile(std::string doc) : isModified(false), isLoaded(false), minify(false) {
			name = doc;
			docpath = "Data\\SKSE\\Plugins\\StorageUtilData\\" + doc;
			/*opened = std::time(0);*/
			root = Json::objectValue;
			LoadFile();
		} //reader = Json::Features::strictMode(); 
		
		bool IsGood() { return readErrors.empty(); }
		std::string GetErrors() { return readErrors; }

		inline Value make(SInt32 v) const;
		inline Value make(float v) const;
		inline Value make(BSFixedString v) const;
		inline Value make(TESForm* v) const;

		inline SInt32 parse(Value value, SInt32 missing) const;
		inline float parse(Value value, float missing) const;
		inline TESForm* parse(Value value, TESForm* missing) const;
		inline BSFixedString parse(Value value, BSFixedString missing) const;

		template<typename T> inline bool HasKey(const std::string &key);// { return root.isMember(Type<T>()) && root[Type<T>()].isMember(key); }

#ifdef NEW_STYLE_JSON
		inline void ENTER() { s_dataLock.Enter(); }
		inline void LEAVE() { s_dataLock.Leave(); }

		/*template<typename T> inline void getvar(const std::string &key, Value &var);
		inline void getvar(const char* type, const std::string &key, Value &var);

		template<typename T> inline void setvar(const std::string &key, Value &var);
		inline void setvar(const char* type, const std::string &key, Value &var);*/



		template <typename T> void SetValue(std::string key, T value);
		template <typename T> T GetValue(std::string key, T value);
		template <typename T> bool UnsetValue(std::string key);
		template <typename T> bool HasValue(std::string key);
		SInt32 AdjustValue(std::string key, SInt32 value);
		float AdjustValue(std::string key, float value);

		template <typename T> int ListAdd(std::string key, T value, bool allowDuplicate);
		template <typename T> T ListGet(std::string key, int index);
		template <typename T> T ListSet(std::string key, int index, T value);
		template <typename T> int ListRemove(std::string key, T removing, bool allInstances);
		template <typename T> bool ListRemoveAt(std::string key, int index);
		template <typename T> bool ListInsertAt(std::string key, int index, T value);
		template <typename T> int ListClear(std::string key);
		template <typename T> int ListCount(std::string key);
		template <typename T> int ListCountValue(std::string key, T value, bool exclude);
		template <typename T> int ListFind(std::string key, T value);
		template <typename T> bool ListHas(std::string key, T value);
		template <typename T> int ListResize(std::string key, int length, T filler);
#endif
#ifdef OLD_STYLE_JSON
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


		Value ListRandom(std::string type, std::string key);


		//void ListSort(std::string type, std::string key);

		template <typename T> T ListAdjust(std::string key, int index, T adjustBy);
		template <typename T> void ListSlice(std::string key, VMArray<T> Output, int startIndex);
		template <typename T> bool ListCopy(std::string key, VMArray<T> Input);
		template <typename T> VMResultArray<T> ToArray(std::string key);

		int CountPrefix(std::string type, std::string prefix);

#endif
		inline bool HasKey(const char* type, const std::string &key);// { return root.isMember(type) && root[type].isMember(key); }
		inline bool HasKey(std::string &type, const std::string &key);// { return root.isMember(type) && root[type].isMember(key); }
	


		inline Value Resolve(const std::string &pathto);// { Path path(pathto.front() != '.' ? '.' + pathto : pathto); return path.resolve(root, Value(Json::nullValue)); }
		inline Value Resolve(const std::string &pathto, Value missing);// { Path path(pathto.front() != '.' ? '.' + pathto : pathto); return path.resolve(root, missing); }

		template <typename T> void SetPathValue(const std::string &path, Value var);
		template <typename T> void SetPathArray(const std::string &path, VMArray<T> arr, bool append);
		void ClearPath(const std::string &path);
		void ClearPathIndex(const std::string &path, int idx);
		template <typename T> T GetPathValue(const std::string &path, T defaultValue);
		template <typename T> VMResultArray<T> PathElements(const std::string &path, T invalidType);
		VMResultArray<BSFixedString> PathMembers(const std::string &path);
		int PathCount(const std::string &path);

		template <typename T> int FindPathElement(const std::string &path, Value toFind);

		//Value ResolveValue(std::string path, Value value);
		bool CanResolve(const std::string &path);
		bool IsObject(const std::string &path);
		bool IsArray(const std::string &path);
		bool IsString(const std::string &path);
		bool IsNumber(const std::string &path);
		bool IsBool(const std::string &path);
		bool IsForm(const std::string &path);

		bool SetRawPathValue(const std::string &path, const std::string &raw);


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
	bool UnloadFile(std::string name);
	bool FileExists(std::string name);
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
