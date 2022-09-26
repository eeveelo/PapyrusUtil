#include "JsonUtil.h"

#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "skse64/GameAPI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameForms.h"
#include "skse64/GameData.h"

#include "Data.h"
#include "External.h"

namespace JsonUtil {
	using namespace External;

	//bool IsValidKey(BSFixedString &key) { return !(key == NULL || !key.data || strlen(key.data) == 0); }
	inline bool IsValidKey(BSFixedString &key) { return key.data && strlen(key.data) > 0; }

	inline bool IsEmpty(SInt32 &value){ return value == 0; }
	inline bool IsEmpty(float &value){ return value == 0.0f; }
	inline bool IsEmpty(BSFixedString &value){ return !value.data || value.data[0] == '\0'; }
	inline bool IsEmpty(TESForm* value){ return value == NULL; }

	template <typename T> inline T Empty() { return T(); }
	template <> inline SInt32 Empty<SInt32>() { return 0; }
	template <> inline float Empty<float>() { return 0.0f; }
	template <> inline BSFixedString Empty<BSFixedString>() { return BSFixedString(""); }
	template <> inline TESForm* Empty<TESForm*>() { return NULL; }

	std::string Filename(std::string name){
		if (name.empty()) name = "noname.json";
		else if (name.find(".json") == std::string::npos) name += ".json";
		return "Data\\SKSE\\Plugins\\StorageUtilData\\" + name;
	}

	bool SaveJson(StaticFunctionTag* base, BSFixedString name, bool styled){
		ExternalFile* File = GetFile(name.data);
		if (!File) return false;
		else{
			bool output = File->SaveFile(styled);
			External::UnloadFile(name.data);
			return output;
		}
	}
	bool LoadJson(StaticFunctionTag* base, BSFixedString name){
		ExternalFile* File = GetFile(name.data);
		if (!File) return false;
		else return File->LoadFile();
	}

	void ClearAll(StaticFunctionTag* base, BSFixedString name){
		ExternalFile* File = GetFile(name.data);
		if (File) File->ClearAll();
	}

	bool UnloadFile(StaticFunctionTag* base, BSFixedString name, bool savechanges, bool minify){
		bool output = External::RevertFile(name.data, savechanges, minify);
		if (output) External::UnloadFile(name.data);
		return output;
	}
	bool IsPendingSave(StaticFunctionTag* base, BSFixedString name){
		return External::ChangesPending(name.data);
	}


	bool IsGood(StaticFunctionTag* base, BSFixedString name) {
		ExternalFile* File = GetFile(name.data);
		return File && File->IsGood();
	}

	BSFixedString GetErrors(StaticFunctionTag* base, BSFixedString name) {
		ExternalFile* File = GetFile(name.data);
		if (File) return BSFixedString(File->GetErrors().c_str());
		else return BSFixedString("");
	}

	void ClearPath(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		//_MESSAGE("ClearPath(%s, %s)", name.data, path.data);
		if (FileExists(name.data)){
			ExternalFile* File = GetFile(name.data);
			if (File && IsValidKey(path)) File->ClearPath(path.data);
		}
	}

	void ClearPathIndex(StaticFunctionTag* base, BSFixedString name, BSFixedString path, SInt32 idx) {
		//_MESSAGE("ClearPath(%s, %s)", name.data, path.data);
		if (FileExists(name.data)) {
			ExternalFile* File = GetFile(name.data);
			if (File && IsValidKey(path)) File->ClearPathIndex(path.data, idx);
		}
	}

	template <typename T>
	void SetPathValue(StaticFunctionTag* base, BSFixedString name, BSFixedString path, T value) {
		ExternalFile* File = GetFile(name.data);
		if (File && IsValidKey(path))
			File->SetPathValue<T>(path.data, MakeValue<T>(value));
	}
	/*template <> void SetPathValue(StaticFunctionTag* base, BSFixedString name, BSFixedString path, BSFixedString value) {
		ExternalFile* File = GetFile(name.data);
		if (File && IsValidKey(path)) {
			File->SetPathValue(std::string(path.data), std::string(value.data));
		}
	}*/

	template <typename T>
	T GetPathValue(StaticFunctionTag* base, BSFixedString name, BSFixedString path, T missing) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(path)) return missing;
		else return File->GetPathValue<T>(path.data, missing);
	}


	template <typename T>
	void SetPathArray(StaticFunctionTag* base, BSFixedString name, BSFixedString path, VMArray<T> arr, bool append) {
		ExternalFile* File = GetFile(name.data);
		if (File && IsValidKey(path))
			File->SetPathArray<T>(path.data, arr, append);
	}

	bool SetRawPathValue(StaticFunctionTag* base, BSFixedString name, BSFixedString path, BSFixedString raw) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(path)) return false;
		else return File->SetRawPathValue(path.data, raw.data);
	}

	/*template <> BSFixedString GetPathValue(StaticFunctionTag* base, BSFixedString name, BSFixedString path, BSFixedString missing) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(path)) return missing;
		std::string value = File->GetPathValue(path.data, std::string(missing.data));
		return BSFixedString(value.c_str());
	}*/


	template <typename T> VMResultArray<T> PathElements(StaticFunctionTag* base, BSFixedString name, BSFixedString path, T invalidType) {
		ExternalFile* File = GetFile(name.data);
		if (File && IsValidKey(path)) return File->PathElements<T>(path.data, invalidType);
		VMResultArray<T> arr;
		return arr;
	}
	VMResultArray<BSFixedString> PathMembers(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		ExternalFile* File = GetFile(name.data);
		if (File && IsValidKey(path)) return File->PathMembers(path.data);
		VMResultArray<BSFixedString> arr;
		return arr;
	}
	SInt32 PathCount(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		ExternalFile* File = GetFile(name.data);
		return (File && IsValidKey(path)) ? File->PathCount(path.data) : -1;
	}
	bool CanResolve(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		ExternalFile* File = GetFile(name.data);
		return File && IsValidKey(path) && File->CanResolve(path.data);
	}
	bool IsObject(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		ExternalFile* File = GetFile(name.data);
		return File && IsValidKey(path) && File->IsObject(path.data);
	}
	bool IsArray(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		ExternalFile* File = GetFile(name.data);
		return File && IsValidKey(path) && File->IsArray(path.data);
	}
	bool IsString(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		ExternalFile* File = GetFile(name.data);
		return File && IsValidKey(path) && File->IsString(path.data);
	}
	bool IsNumber(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		ExternalFile* File = GetFile(name.data);
		return File && IsValidKey(path) && File->IsNumber(path.data);
	}
	bool IsBool(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		ExternalFile* File = GetFile(name.data);
		return File && IsValidKey(path) && File->IsBool(path.data);
	}
	bool IsForm(StaticFunctionTag* base, BSFixedString name, BSFixedString path) {
		ExternalFile* File = GetFile(name.data);
		return File && IsValidKey(path) && File->IsForm(path.data);
	}

	template <typename T>
	SInt32 FindPathElement(StaticFunctionTag* base, BSFixedString name, BSFixedString path, T value) {
		ExternalFile* File = GetFile(name.data);
		return (File && IsValidKey(path)) ? File->FindPathElement<T>(path.data, MakeValue<T>(value)) : -1;
	}

	template <typename T>
	T SetValue2(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value) {
		ExternalFile* File = GetFile(name.data);
		if (!File || IsEmpty(key)) return Empty<T>();
		else File->SetValue<T>(key.data, value);
		return value;
	}

	template <typename T>
	T GetValue2(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T missing) {
		ExternalFile* File = GetFile(name.data);
		if (!File || IsEmpty(key)) return Empty<T>();
		else return File->GetValue<T>(key.data, missing);
	}




	template <typename T>
	T SetValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return Empty<T>();
		else File->SetValue(Type<T>(), key.data, MakeValue<T>(value));
		return value;
	}

	template <typename T>
	T GetValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T missing) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->GetValue(Type<T>(), key.data, MakeValue<T>(missing)));
	}

	template <typename T>
	T AdjustValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->AdjustValue(Type<T>(), key.data, MakeValue<T>(value)));
	}

	template <typename T>
	bool UnsetValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return false;
		else return File->UnsetValue(Type<T>(), key.data);
	}

	template <typename T>
	bool HasValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return false;
		else return File->HasValue(Type<T>(), key.data);
	}

	template <typename T>
	SInt32 ListAdd(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value, bool allowDuplicate) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return -1;
		else return File->ListAdd(List<T>(), key.data, MakeValue<T>(value), allowDuplicate);
	}

	template <typename T>
	T ListGet(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->ListGet(List<T>(), key.data, index));
	}

	template <typename T>
	T ListSet(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index, T value) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->ListSet(List<T>(), key.data, index, MakeValue<T>(value)));
	}

	template <typename T>
	T ListAdjust(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index, T adjustBy) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->ListAdjust(key.data, index, adjustBy));
	}

	template <typename T>
	UInt32 ListRemove(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value, bool allInstances) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return 0;
		else return File->ListRemove(List<T>(), key.data, MakeValue<T>(value), allInstances);
	}

	template <typename T>
	bool ListRemoveAt(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return false;
		else return File->ListRemoveAt(List<T>(), key.data, index);
	}

	template <typename T>
	bool ListInsertAt(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index, T value) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return false;
		else return File->ListInsertAt(List<T>(), key.data, index, MakeValue<T>(value));
	}

	template <typename T>
	UInt32 ListClear(StaticFunctionTag* base, BSFixedString name, BSFixedString key) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return 0;
		else return File->ListClear(List<T>(), key.data);
	}

	template <typename T>
	UInt32 ListCount(StaticFunctionTag* base, BSFixedString name, BSFixedString key) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return 0;
		return File->ListCount(List<T>(), key.data);
	}

	template <typename T>
	UInt32 ListCountValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value, bool exclude) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return 0;
		return File->ListCountValue(List<T>(), key.data, MakeValue<T>(value), exclude);
	}

	template <typename T>
	SInt32 ListFind(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value) {
		ExternalFile* File = GetFile(name.data);
		
		if (!File) { _MESSAGE("Invalid File: %s", name.data); return -1; }
		else if (!IsValidKey(key)) { _MESSAGE("Invalid Key: %s", key.data); return -1; }
		//if (!File || !IsValidKey(key)) return -1;
		else return File->ListFind(List<T>(), key.data, MakeValue<T>(value));
	}

	template <typename T>
	bool ListHas(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return false;
		else return File->ListFind(List<T>(), key.data, MakeValue<T>(value)) != -1;
	}

	template <typename T>
	SInt32 ListResize(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 length, T filler) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key) || length > 500) return 0;
		else return File->ListResize(List<T>(), key.data, length, MakeValue<T>(filler));
	}

	template <typename T>
	void ListSlice(StaticFunctionTag* base, BSFixedString name, BSFixedString key, VMArray<T> Output, UInt32 startIndex){
		ExternalFile* File = GetFile(name.data);
		if (File && Output.arr && Output.Length() > 0 && IsValidKey(key))
			File->ListSlice<T>(key.data, Output, startIndex);
	}

	template <typename T>
	bool ListCopy(StaticFunctionTag* base, BSFixedString name, BSFixedString key, VMArray<T> Input) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key) || !Input.arr || Input.Length() < 1) return false;
		else return File->ListCopy<T>(key.data, Input);
	}
	template <typename T>
	VMResultArray<T> ToArray(StaticFunctionTag* base, BSFixedString name, BSFixedString key){
		ExternalFile* File = GetFile(name.data);
		VMResultArray<T> arr;
		if (File && IsValidKey(key))
			arr = File->ToArray<T>(key.data);
		return arr;
	}


	template <typename T>
	UInt32 CountValuePrefix(StaticFunctionTag* base, BSFixedString name, BSFixedString prefix) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(prefix)) return 0;
		else return File->CountPrefix(Type<T>(), prefix.data);
	}

	template <typename T>
	UInt32 CountListPrefix(StaticFunctionTag* base, BSFixedString name, BSFixedString prefix) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(prefix)) return 0;
		else return File->CountPrefix(List<T>(), prefix.data);
	}

	UInt32 CountAllPrefix(StaticFunctionTag* base, BSFixedString name, BSFixedString prefix) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(prefix)) return 0;
		int count = 0;
		count += File->CountPrefix(Type<SInt32>(), prefix.data);
		count += File->CountPrefix(Type<float>(), prefix.data);
		count += File->CountPrefix(Type<BSFixedString>(), prefix.data);
		count += File->CountPrefix(Type<TESForm*>(), prefix.data);
		count += File->CountPrefix(List<SInt32>(), prefix.data);
		count += File->CountPrefix(List<float>(), prefix.data);
		count += File->CountPrefix(List<BSFixedString>(), prefix.data);
		count += File->CountPrefix(List<TESForm*>(), prefix.data);
		return count;
	}


	template <typename T>
	T ListRandom(StaticFunctionTag* base, BSFixedString name, BSFixedString key) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->ListRandom(List<T>(), key.data));
	}


	/*template <typename T>
	UInt32 ClearValuePrefix(StaticFunctionTag* base, BSFixedString name, BSFixedString prefix) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(prefix)) return 0;
		else return File->ClearPrefix(Type<T>(), prefix.data);
	}

	template <typename T>
	UInt32 ClearListPrefix(StaticFunctionTag* base, BSFixedString name, BSFixedString prefix) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(prefix)) return 0;
		else return File->ClearPrefix(List<T>(), prefix.data);
	}

	UInt32 ClearAllPrefix(StaticFunctionTag* base, BSFixedString name, BSFixedString prefix) {
		ExternalFile* File = GetFile(name.data);
		if (!File || !IsValidKey(prefix)) return 0;
		int count = 0;
		count += File->ClearPrefix(Type<SInt32>(), prefix.data);
		count += File->ClearPrefix(Type<float>(), prefix.data);
		count += File->ClearPrefix(Type<BSFixedString>(), prefix.data);
		count += File->ClearPrefix(Type<TESForm*>(), prefix.data);
		count += File->ClearPrefix(List<SInt32>(), prefix.data);
		count += File->ClearPrefix(List<float>(), prefix.data);
		count += File->ClearPrefix(List<BSFixedString>(), prefix.data);
		count += File->ClearPrefix(List<TESForm*>(), prefix.data);
		return count;
	}*/

	namespace fs = boost::filesystem;

	VMResultArray<BSFixedString> JsonFilesInFolder(StaticFunctionTag* base, BSFixedString dirpath) {
		VMResultArray<BSFixedString> arr;
		if (dirpath.data && dirpath.data[0] != '\0') {
			std::string dirstr = "Data\\SKSE\\Plugins\\StorageUtilData\\";
			dirstr.append(dirpath.data);
			fs::path someDir(dirstr);
			fs::directory_iterator end_iter;
			if (fs::exists(someDir) && fs::is_directory(someDir)){
				for (fs::directory_iterator dir_iter(someDir); dir_iter != end_iter; ++dir_iter){
					if (fs::is_regular_file(dir_iter->status())) {
						fs::path filepath = dir_iter->path();
						std::string file = filepath.filename().generic_string();
						//std::string ext = filepath.extension().generic_string();
						//_MESSAGE("file: %s ext: %s", file.c_str(), ext.c_str());
						if (boost::iequals(filepath.extension().generic_string(), ".json"))
							arr.push_back(BSFixedString(file.c_str()));						
					}
				}
			}
		}
		return arr;
	}

} // JsonUtil



#include "skse64/PapyrusNativeFunctions.h"

void JsonUtil::RegisterFuncs(VMClassRegistry* registry){

	// File manipulation
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, bool>("Save", "JsonUtil", SaveJson, registry));
	//registry->SetFunctionFlags("JsonUtil", "Save", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("Load", "JsonUtil", LoadJson, registry));
	//registry->SetFunctionFlags("JsonUtil", "Load", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, BSFixedString>("ClearAll", "JsonUtil", ClearAll, registry));
	registry->SetFunctionFlags("JsonUtil", "ClearAll", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, bool, bool>("Unload", "JsonUtil", UnloadFile, registry));
	//registry->SetFunctionFlags("JsonUtil", "Unload", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("IsPendingSave", "JsonUtil", IsPendingSave, registry));
	registry->SetFunctionFlags("JsonUtil", "IsPendingSave", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("IsGood", "JsonUtil", IsGood, registry));
	registry->SetFunctionFlags("JsonUtil", "IsGood", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, BSFixedString, BSFixedString>("GetErrors", "JsonUtil", GetErrors, registry));
	registry->SetFunctionFlags("JsonUtil", "GetErrors", VMClassRegistry::kFunctionFlag_NoWait);

	//registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("JsonExists", "JsonUtil", FileExists, registry));
	//registry->SetFunctionFlags("JsonUtil", "JsonExists", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, BSFixedString, BSFixedString>("ClearPath", "JsonUtil", ClearPath, registry));
	registry->SetFunctionFlags("JsonUtil", "ClearPath", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, BSFixedString, SInt32>("ClearPathIndex", "JsonUtil", ClearPathIndex, registry));
	registry->SetFunctionFlags("JsonUtil", "ClearPathIndex", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, BSFixedString, SInt32>("SetPathIntValue", "JsonUtil", SetPathValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, BSFixedString, float>("SetPathFloatValue", "JsonUtil", SetPathValue<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, BSFixedString, BSFixedString>("SetPathStringValue", "JsonUtil", SetPathValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, BSFixedString, TESForm*>("SetPathFormValue", "JsonUtil", SetPathValue<TESForm*>, registry));
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, BSFixedString, bool>("SetPathBoolValue", "JsonUtil", SetPathValue<bool>, registry));
	registry->SetFunctionFlags("JsonUtil", "SetPathIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetPathFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetPathStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetPathFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("GetPathIntValue", "JsonUtil", GetPathValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, BSFixedString, float>("GetPathFloatValue", "JsonUtil", GetPathValue<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, BSFixedString>("GetPathStringValue", "JsonUtil", GetPathValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString, TESForm*>("GetPathFormValue", "JsonUtil", GetPathValue<TESForm*>, registry));
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, bool>("GetPathBoolValue", "JsonUtil", GetPathValue<bool>, registry));
	registry->SetFunctionFlags("JsonUtil", "GetPathIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "GetPathFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "GetPathStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "GetPathFormValue", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("JsonUtil", "GetPathBoolValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<SInt32>, BSFixedString, BSFixedString, SInt32>("PathIntElements", "JsonUtil", PathElements<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<float>, BSFixedString, BSFixedString, float>("PathFloatElements", "JsonUtil", PathElements<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<BSFixedString>, BSFixedString, BSFixedString, BSFixedString>("PathStringElements", "JsonUtil", PathElements<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<TESForm*>, BSFixedString, BSFixedString, TESForm*>("PathFormElements", "JsonUtil", PathElements<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "PathIntElements", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "PathFloatElements", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "PathStringElements", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "PathFormElements", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, BSFixedString, BSFixedString, VMArray<SInt32>, bool>("SetPathIntArray", "JsonUtil", SetPathArray<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, BSFixedString, BSFixedString, VMArray<float>, bool>("SetPathFloatArray", "JsonUtil", SetPathArray<float>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, BSFixedString, BSFixedString, VMArray<BSFixedString>, bool>("SetPathStringArray", "JsonUtil", SetPathArray<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, BSFixedString, BSFixedString, VMArray<TESForm*>, bool>("SetPathFormArray", "JsonUtil", SetPathArray<TESForm*>, registry));
	//registry->SetFunctionFlags("JsonUtil", "SetPathIntArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("JsonUtil", "SetPathFloatArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("JsonUtil", "SetPathStringArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("JsonUtil", "SetPathFormArray", VMClassRegistry::kFunctionFlag_NoWait);


	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("FindPathIntElement", "JsonUtil", FindPathElement<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, float>("FindPathFloatElement", "JsonUtil", FindPathElement<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, BSFixedString>("FindPathStringElement", "JsonUtil", FindPathElement<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, TESForm*>("FindPathFormElement", "JsonUtil", FindPathElement<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "FindPathIntElement", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FindPathFloatElement", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FindPathStringElement", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FindPathFormElement", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BSFixedString>, BSFixedString, BSFixedString>("PathMembers", "JsonUtil", PathMembers, registry));
	registry->SetFunctionFlags("JsonUtil", "PathMembers", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, BSFixedString>("PathCount", "JsonUtil", PathCount, registry));
	registry->SetFunctionFlags("JsonUtil", "PathCount", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("CanResolvePath", "JsonUtil", CanResolve, registry));
	registry->SetFunctionFlags("JsonUtil", "CanResolvePath", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("IsPathObject", "JsonUtil", IsObject, registry));
	registry->SetFunctionFlags("JsonUtil", "IsPathObject", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("IsPathArray", "JsonUtil", IsArray, registry));
	registry->SetFunctionFlags("JsonUtil", "IsPathArray", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("IsPathString", "JsonUtil", IsString, registry));
	registry->SetFunctionFlags("JsonUtil", "IsPathString", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("IsPathNumber", "JsonUtil", IsNumber, registry));
	registry->SetFunctionFlags("JsonUtil", "IsPathNumber", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("IsPathBool", "JsonUtil", IsBool, registry));
	registry->SetFunctionFlags("JsonUtil", "IsPathBool", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("IsPathForm", "JsonUtil", IsForm, registry));
	registry->SetFunctionFlags("JsonUtil", "IsPathForm", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, BSFixedString>("SetRawPathValue", "JsonUtil", SetRawPathValue, registry));
	//registry->SetFunctionFlags("JsonUtil", "SetRawPathValue", VMClassRegistry::kFunctionFlag_NoWait);

	// Global  values
	/*registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("SetIntValue2", "JsonUtil", SetValue2<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, BSFixedString, float>("SetFloatValue2", "JsonUtil", SetValue2<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, BSFixedString>("SetStringValue2", "JsonUtil", SetValue2<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString, TESForm*>("SetFormValue2", "JsonUtil", SetValue2<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "SetIntValue2", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetFloatValue2", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetStringValue2", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetFormValue2", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("SetIntValue2NoWait", "JsonUtil", SetValue2<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, BSFixedString, float>("SetFloatValue2NoWait", "JsonUtil", SetValue2<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, BSFixedString>("SetStringValue2NoWait", "JsonUtil", SetValue2<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString, TESForm*>("SetFormValue2NoWait", "JsonUtil", SetValue2<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "SetIntValue2NoWait", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetFloatValue2NoWait", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetStringValue2NoWait", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetFormValue2NoWait", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("GetIntValue2", "JsonUtil", GetValue2<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, BSFixedString, float>("GetFloatValue2", "JsonUtil", GetValue2<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, BSFixedString>("GetStringValue2", "JsonUtil", GetValue2<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString, TESForm*>("GetFormValue2", "JsonUtil", GetValue2<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "GetIntValue2", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "GetFloatValue2", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "GetStringValue2", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "GetFormValue2", VMClassRegistry::kFunctionFlag_NoWait);*/


	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("SetIntValue", "JsonUtil", SetValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, BSFixedString, float>("SetFloatValue", "JsonUtil", SetValue<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, BSFixedString>("SetStringValue", "JsonUtil", SetValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString, TESForm*>("SetFormValue", "JsonUtil", SetValue<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "SetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	/*registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("SetIntValueNoWait", "JsonUtil", SetValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, BSFixedString, float>("SetFloatValueNoWait", "JsonUtil", SetValue<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, BSFixedString>("SetStringValueNoWait", "JsonUtil", SetValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString, TESForm*>("SetFormValueNoWait", "JsonUtil", SetValue<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "SetIntValueNoWait", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetFloatValueNoWait", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetStringValueNoWait", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "SetFormValueNoWait", VMClassRegistry::kFunctionFlag_NoWait);*/

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("GetIntValue", "JsonUtil", GetValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, BSFixedString, float>("GetFloatValue", "JsonUtil", GetValue<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, BSFixedString>("GetStringValue", "JsonUtil", GetValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString, TESForm*>("GetFormValue", "JsonUtil", GetValue<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "GetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "GetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "GetStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "GetFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("AdjustIntValue", "JsonUtil", AdjustValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, BSFixedString, float>("AdjustFloatValue", "JsonUtil", AdjustValue<float>, registry));
	registry->SetFunctionFlags("JsonUtil", "AdjustIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "AdjustFloatValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("UnsetIntValue", "JsonUtil", UnsetValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("UnsetFloatValue", "JsonUtil", UnsetValue<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("UnsetStringValue", "JsonUtil", UnsetValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("UnsetFormValue", "JsonUtil", UnsetValue<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "UnsetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "UnsetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "UnsetStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "UnsetFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("HasIntValue", "JsonUtil", HasValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("HasFloatValue", "JsonUtil", HasValue<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("HasStringValue", "JsonUtil", HasValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("HasFormValue", "JsonUtil", HasValue<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "HasIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "HasFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "HasStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "HasFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	// Global  lists
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32, bool>("IntListAdd", "JsonUtil", ListAdd<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, float, bool>("FloatListAdd", "JsonUtil", ListAdd<float>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, BSFixedString, bool>("StringListAdd", "JsonUtil", ListAdd<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, TESForm*, bool>("FormListAdd", "JsonUtil", ListAdd<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListAdd", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListAdd", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListAdd", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListAdd", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, UInt32>("IntListGet", "JsonUtil", ListGet<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, BSFixedString, UInt32>("FloatListGet", "JsonUtil", ListGet<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, UInt32>("StringListGet", "JsonUtil", ListGet<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString, UInt32>("FormListGet", "JsonUtil", ListGet<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListGet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListGet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListGet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListGet", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, UInt32, SInt32>("IntListAdjust", "JsonUtil", ListAdjust<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, float, BSFixedString, BSFixedString, UInt32, float>("FloatListAdjust", "JsonUtil", ListAdjust<float>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListAdjust", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListAdjust", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, UInt32, SInt32>("IntListSet", "JsonUtil", ListSet<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, float, BSFixedString, BSFixedString, UInt32, float>("FloatListSet", "JsonUtil", ListSet<float>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, UInt32, BSFixedString>("StringListSet", "JsonUtil", ListSet<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString, UInt32, TESForm*>("FormListSet", "JsonUtil", ListSet<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListSet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListSet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListSet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListSet", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, SInt32, bool>("IntListRemove", "JsonUtil", ListRemove<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, float, bool>("FloatListRemove", "JsonUtil", ListRemove<float>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, BSFixedString, bool>("StringListRemove", "JsonUtil", ListRemove<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, TESForm*, bool>("FormListRemove", "JsonUtil", ListRemove<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListRemove", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListRemove", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListRemove", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListRemove", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, UInt32>("IntListRemoveAt", "JsonUtil", ListRemoveAt<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, UInt32>("FloatListRemoveAt", "JsonUtil", ListRemoveAt<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, UInt32>("StringListRemoveAt", "JsonUtil", ListRemoveAt<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, UInt32>("FormListRemoveAt", "JsonUtil", ListRemoveAt<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, bool, BSFixedString, BSFixedString, UInt32, SInt32>("IntListInsertAt", "JsonUtil", ListInsertAt<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, bool, BSFixedString, BSFixedString, UInt32, float>("FloatListInsertAt", "JsonUtil", ListInsertAt<float>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, bool, BSFixedString, BSFixedString, UInt32, BSFixedString>("StringListInsertAt", "JsonUtil", ListInsertAt<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, bool, BSFixedString, BSFixedString, UInt32, TESForm*>("FormListInsertAt", "JsonUtil", ListInsertAt<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListInsertAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListInsertAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListInsertAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListInsertAt", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("IntListClear", "JsonUtil", ListClear<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("FloatListClear", "JsonUtil", ListClear<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("StringListClear", "JsonUtil", ListClear<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("FormListClear", "JsonUtil", ListClear<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListClear", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListClear", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListClear", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListClear", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("IntListCount", "JsonUtil", ListCount<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("FloatListCount", "JsonUtil", ListCount<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("StringListCount", "JsonUtil", ListCount<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("FormListCount", "JsonUtil", ListCount<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListCount", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, SInt32, bool>("IntListCountValue", "JsonUtil", ListCountValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, float, bool>("FloatListCountValue", "JsonUtil", ListCountValue<float>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, BSFixedString, bool>("StringListCountValue", "JsonUtil", ListCountValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, TESForm*, bool>("FormListCountValue", "JsonUtil", ListCountValue<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListCountValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListCountValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListCountValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListCountValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, SInt32>("IntListFind", "JsonUtil", ListFind<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, float>("FloatListFind", "JsonUtil", ListFind<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, BSFixedString>("StringListFind", "JsonUtil", ListFind<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, TESForm*>("FormListFind", "JsonUtil", ListFind<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListFind", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListFind", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListFind", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListFind", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, SInt32>("IntListHas", "JsonUtil", ListHas<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, float>("FloatListHas", "JsonUtil", ListHas<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, BSFixedString>("StringListHas", "JsonUtil", ListHas<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, BSFixedString, TESForm*>("FormListHas", "JsonUtil", ListHas<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListHas", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListHas", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListHas", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListHas", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, BSFixedString, BSFixedString, VMArray<SInt32>, UInt32>("IntListSlice", "JsonUtil", ListSlice<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, BSFixedString, BSFixedString, VMArray<float>, UInt32>("FloatListSlice", "JsonUtil", ListSlice<float>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, BSFixedString, BSFixedString, VMArray<BSFixedString>, UInt32>("StringListSlice", "JsonUtil", ListSlice<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, void, BSFixedString, BSFixedString, VMArray<TESForm*>, UInt32>("FormListSlice", "JsonUtil", ListSlice<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListSlice", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListSlice", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListSlice", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListSlice", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, BSFixedString, BSFixedString, UInt32, SInt32>("IntListResize", "JsonUtil", ListResize<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, BSFixedString, BSFixedString, UInt32, float>("FloatListResize", "JsonUtil", ListResize<float>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, BSFixedString, BSFixedString, UInt32, BSFixedString>("StringListResize", "JsonUtil", ListResize<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, BSFixedString, BSFixedString, UInt32, TESForm*>("FormListResize", "JsonUtil", ListResize<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListResize", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListResize", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListResize", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListResize", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, BSFixedString, BSFixedString, VMArray<SInt32>>("IntListCopy", "JsonUtil", ListCopy<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, BSFixedString, BSFixedString, VMArray<float>>("FloatListCopy", "JsonUtil", ListCopy<float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, BSFixedString, BSFixedString, VMArray<BSFixedString>>("StringListCopy", "JsonUtil", ListCopy<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, BSFixedString, BSFixedString, VMArray<TESForm*>>("FormListCopy", "JsonUtil", ListCopy<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListCopy", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListCopy", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListCopy", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListCopy", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, VMResultArray<SInt32>, BSFixedString, BSFixedString>("IntListToArray", "JsonUtil", ToArray<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, VMResultArray<float>, BSFixedString, BSFixedString>("FloatListToArray", "JsonUtil", ToArray<float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, VMResultArray<BSFixedString>, BSFixedString, BSFixedString>("StringListToArray", "JsonUtil", ToArray<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, VMResultArray<TESForm*>, BSFixedString, BSFixedString>("FormListToArray", "JsonUtil", ToArray<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListToArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListToArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListToArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListToArray", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("CountIntValuePrefix", "JsonUtil", CountValuePrefix<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("CountFloatValuePrefix", "JsonUtil", CountValuePrefix<float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("CountStringValuePrefix", "JsonUtil", CountValuePrefix<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("CountFormValuePrefix", "JsonUtil", CountValuePrefix<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "CountIntValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "CountFloatValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "CountStringValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "CountFormValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("CountIntListPrefix", "JsonUtil", CountListPrefix<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("CountFloatListPrefix", "JsonUtil", CountListPrefix<float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("CountStringListPrefix", "JsonUtil", CountListPrefix<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("CountFormListPrefix", "JsonUtil", CountListPrefix<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "CountIntListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "CountFloatListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "CountStringListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "CountFormListPrefix", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, BSFixedString>("IntListRandom", "JsonUtil", ListRandom<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, float, BSFixedString, BSFixedString>("FloatListRandom", "JsonUtil", ListRandom<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("StringListRandom", "JsonUtil", ListRandom<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, TESForm*, BSFixedString, BSFixedString>("FormListRandom", "JsonUtil", ListRandom<TESForm*>, registry));
	registry->SetFunctionFlags("JsonUtil", "IntListRandom", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FloatListRandom", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "StringListRandom", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("JsonUtil", "FormListRandom", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("CountAllPrefix", "JsonUtil", CountAllPrefix, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, VMResultArray<BSFixedString>, BSFixedString>("JsonInFolder", "JsonUtil", JsonFilesInFolder, registry));

	/*registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ClearIntValuePrefix", "JsonUtil", ClearValuePrefix<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ClearFloatValuePrefix", "JsonUtil", ClearValuePrefix<float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ClearStringValuePrefix", "JsonUtil", ClearValuePrefix<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ClearFormValuePrefix", "JsonUtil", ClearValuePrefix<TESForm*>, registry));

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ClearIntListPrefix", "JsonUtil", ClearListPrefix<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ClearFloatListPrefix", "JsonUtil", ClearListPrefix<float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ClearStringListPrefix", "JsonUtil", ClearListPrefix<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ClearFormListPrefix", "JsonUtil", ClearListPrefix<TESForm*>, registry));

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ClearAllPrefix", "JsonUtil", ClearAllPrefix, registry));*/

}
