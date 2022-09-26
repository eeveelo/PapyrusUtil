#include "StorageUtil.h"

#include "Data.h"
#include "Forms.h"

#include "skse64/GameTypes.h"
#include "skse64/GameForms.h"

#include <boost/algorithm/string.hpp>

namespace StorageUtil {
	using namespace Forms;

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

	// Flat key=>value storage
	template <typename T, typename S>
	T SetValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value) {
		Data::Values<T, S>* Data = Data::GetValues<T, S>();
		if (!Data || !IsValidKey(key)) return Empty<T>();
		else Data->SetValue(GetFormKey(obj), key.data, value);
		return value;
	}

	template <typename T, typename S>
	T GetValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T missing) {
		Data::Values<T, S>* Data = Data::GetValues<T, S>();
		if (!Data || !IsValidKey(key)) return missing;
		else return Data->GetValue(GetFormKey(obj), key.data, missing);
	}

	template <typename T, typename S>
	T AdjustValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value) {
		Data::Values<T, S>* Data = Data::GetValues<T, S>();
		if (!Data || !IsValidKey(key)) return Empty<T>();
		else return Data->AdjustValue(GetFormKey(obj), key.data, value);
	}

	template <typename T, typename S>
	T PluckValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T missing) {
		Data::Values<T, S>* Data = Data::GetValues<T, S>();
		if (!Data || !IsValidKey(key)) return missing;
		else return Data->PluckValue(GetFormKey(obj), key.data, missing);
	}

	template <typename T, typename S>
	bool UnsetValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key) {
		Data::Values<T, S>* Data = Data::GetValues<T, S>();
		if (!Data || !IsValidKey(key)) return false;
		else return Data->UnsetValue(GetFormKey(obj), key.data);
	}

	template <typename T, typename S>
	bool HasValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key) {
		Data::Values<T, S>* Data = Data::GetValues<T, S>();
		if (!Data || !IsValidKey(key)) return false;
		else return Data->HasValue(GetFormKey(obj), key.data);
	}

	// Lists key=>vector=>value storage
	template <typename T, typename S>
	SInt32 ListAdd(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value, bool allowDuplicate) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return -1;
		else return Data->ListAdd(GetFormKey(obj), key.data, value, allowDuplicate);
	}

	template <typename T, typename S>
	T ListGet(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return Empty<T>();
		else return Data->ListGet(GetFormKey(obj), key.data, index);
	}

	template <typename T, typename S>
	T ListSet(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index, T value) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return Empty<T>();
		else return Data->ListSet(GetFormKey(obj), key.data, index, value);
	}

	template <typename T, typename S>
	T ListAdjust(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index, T value) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return Empty<T>();
		else return Data->ListAdjust(GetFormKey(obj), key.data, index, value);
	}

	template <typename T, typename S>
	UInt32 ListRemove(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value, bool allInstances) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return 0;
		else return Data->ListRemove(GetFormKey(obj), key.data, value, allInstances);
	}

	template <typename T, typename S>
	bool ListInsertAt(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index, T value) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return false;
		else return Data->ListInsertAt(GetFormKey(obj), key.data, index, value);
	}

	template <typename T, typename S>
	T ListPluck(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index, T value) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key))  return Empty<T>();
		else return Data->ListPluck(GetFormKey(obj), key.data, index, value);
	}

	template <typename T, typename S>
	T ListShift(StaticFunctionTag* base, TESForm* obj, BSFixedString key) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return Empty<T>();
		else return Data->ListShift(GetFormKey(obj), key.data);
	}

	template <typename T, typename S>
	T ListPop(StaticFunctionTag* base, TESForm* obj, BSFixedString key) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return Empty<T>();
		else return Data->ListPop(GetFormKey(obj), key.data);
	}

	template <typename T, typename S>
	bool ListRemoveAt(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return false;
		else return Data->ListRemoveAt(GetFormKey(obj), key.data, index);
	}

	template <typename T, typename S>
	UInt32 ListClear(StaticFunctionTag* base, TESForm* obj, BSFixedString key) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return 0;
		else return Data->ListClear(GetFormKey(obj), key.data);
	}

	template <typename T, typename S>
	UInt32 ListCount(StaticFunctionTag* base, TESForm* obj, BSFixedString key) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return 0;
		else return Data->ListCount(GetFormKey(obj), key.data);
	}

	template <typename T, typename S>
	UInt32 ListCountValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value, bool exclude) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return 0;
		else return Data->ListCountValue(GetFormKey(obj), key.data, value, exclude);
	}

	template <typename T, typename S>
	SInt32 ListFind(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return -1;
		else return Data->ListFind(GetFormKey(obj), key.data, value);
	}

	template <typename T, typename S>
	bool ListHas(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return false;
		else return Data->ListHas(GetFormKey(obj), key.data, value);
	}

	template <typename T, typename S>
	void ListSort(StaticFunctionTag* base, TESForm* obj, BSFixedString key) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (Data && IsValidKey(key))
			Data->ListSort(GetFormKey(obj), key.data);
	}

	template <typename T, typename S>
	void ListSlice(StaticFunctionTag* base, TESForm* obj, BSFixedString key, VMArray<T> Output, UInt32 startIndex) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (Data && Output.arr && Output.Length() > 0 && startIndex >= 0 && IsValidKey(key))
			Data->ListSlice(GetFormKey(obj), key.data, Output, startIndex);
	}

	template <typename T, typename S>
	SInt32 ListResize(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 length, T filler) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key) || length > 500) return 0;
		else return Data->ListResize(GetFormKey(obj), key.data, length, filler);
	}

	template <typename T, typename S>
	bool ListCopy(StaticFunctionTag* base, TESForm* obj, BSFixedString key, VMArray<T> Input) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key) || !Input.arr || Input.Length() < 1) return false;
		else return Data->ListCopy(GetFormKey(obj), key.data, Input);
	}

	template <typename T, typename S>
	VMResultArray<T> ToArray(StaticFunctionTag* base, TESForm* obj, BSFixedString key) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		VMResultArray<T> arr;
		if (Data && IsValidKey(key))
			arr = Data->ToArray(GetFormKey(obj), key.data);
		return arr;
	}

	// Debug functions
	UInt32 Cleanup(StaticFunctionTag* base) {
		int removed = 0;
		// Values
		removed += Data::GetValues<SInt32, SInt32>()->Cleanup();
		removed += Data::GetValues<float, float>()->Cleanup();
		removed += Data::GetValues<BSFixedString, std::string>()->Cleanup();
		removed += Data::GetValues<TESForm*, UInt32>()->Cleanup();
		// Lists
		removed += Data::GetLists<SInt32, SInt32>()->Cleanup();
		removed += Data::GetLists<float, float>()->Cleanup();
		removed += Data::GetLists<BSFixedString, std::string>()->Cleanup();
		removed += Data::GetLists<TESForm*, UInt32>()->Cleanup();
		return removed;
	}

	void DeleteValues(StaticFunctionTag* base, TESForm* obj) {
		UInt64 key = GetFormKey(obj);
		// Values
		Data::GetValues<SInt32, SInt32>()->RemoveForm(key);
		Data::GetValues<float, float>()->RemoveForm(key);
		Data::GetValues<BSFixedString, std::string>()->RemoveForm(key);
		Data::GetValues<TESForm*, UInt32>()->RemoveForm(key);
		// Lists
		Data::GetLists<SInt32, SInt32>()->RemoveForm(key);
		Data::GetLists<float, float>()->RemoveForm(key);
		Data::GetLists<BSFixedString, std::string>()->RemoveForm(key);
		Data::GetLists<TESForm*, UInt32>()->RemoveForm(key);
	}

	void DeleteAllValues(StaticFunctionTag* base) {
		// Values
		Data::GetValues<SInt32, SInt32>()->Revert();
		Data::GetValues<float, float>()->Revert();
		Data::GetValues<BSFixedString, std::string>()->Revert();
		Data::GetValues<TESForm*, UInt32>()->Revert();
		// Lists
		Data::GetLists<SInt32, SInt32>()->Revert();
		Data::GetLists<float, float>()->Revert();
		Data::GetLists<BSFixedString, std::string>()->Revert();
		Data::GetLists<TESForm*, UInt32>()->Revert();
	}


	template <typename T, typename S>
	UInt32 GetObjectCount(StaticFunctionTag* base) {
		return Data::GetValues<T, S>()->GetObjCount();
	}

	template <typename T, typename S>
	UInt32 GetListsObjectCount(StaticFunctionTag* base) {
		return Data::GetLists<T, S>()->GetObjCount();
	}

	template <typename T, typename S>
	UInt32 GetKeyCount(StaticFunctionTag* base, TESForm* obj) {
		return Data::GetValues<T, S>()->GetKeyCount(GetFormKey(obj));
	}

	template <typename T, typename S>
	UInt32 GetListsKeyCount(StaticFunctionTag* base, TESForm* obj) {
		return Data::GetLists<T, S>()->GetKeyCount(GetFormKey(obj));
	}

	template <typename T, typename S>
	TESForm* GetNthObj(StaticFunctionTag* base, UInt32 i) {
		return Data::GetValues<T, S>()->GetNthObj(i);
	}

	template <typename T, typename S>
	TESForm* GetListsNthObj(StaticFunctionTag* base, UInt32 i) {
		return Data::GetLists<T, S>()->GetNthObj(i);
	}

	template <typename T, typename S>
	BSFixedString GetNthKey(StaticFunctionTag* base, TESForm* obj, UInt32 i) {
		const std::string key = Data::GetValues<T, S>()->GetNthKey(GetFormKey(obj), i);
		return BSFixedString(key.c_str());
	}

	template <typename T, typename S>
	BSFixedString GetListsNthKey(StaticFunctionTag* base, TESForm* obj, UInt32 i) {
		const std::string key = Data::GetLists<T, S>()->GetNthKey(GetFormKey(obj), i);
		return BSFixedString(key.c_str());
	}

	template <typename T, typename S>
	VMResultArray<TESForm*> GetListsAllObj(StaticFunctionTag* base) {
		return Data::GetLists<T, S>()->GetAllObj();
	}

	template <typename T, typename S>
	VMResultArray<TESForm*> GetValuesAllObj(StaticFunctionTag* base) {
		return Data::GetValues<T, S>()->GetAllObj();
	}

	template <typename T, typename S>
	VMResultArray<BSFixedString> GetListsObjKeys(StaticFunctionTag* base, TESForm* obj) {
		return Data::GetLists<T, S>()->GetAllObjKeys(GetFormKey(obj));
	}

	template <typename T, typename S>
	VMResultArray<BSFixedString> GetValuesObjKeys(StaticFunctionTag* base, TESForm* obj) {
		return Data::GetValues<T, S>()->GetAllObjKeys(GetFormKey(obj));
	}

	template <typename T, typename S>
	UInt32 CountValuesPrefixKey(StaticFunctionTag* base, BSFixedString prefix) {
		return Data::GetValues<T, S>()->CountPrefixKey(prefix.data);
	}

	template <typename T, typename S>
	UInt32 CountListsPrefixKey(StaticFunctionTag* base, BSFixedString prefix) {
		return Data::GetLists<T, S>()->CountPrefixKey(prefix.data);
	}

	UInt32 CountAllPrefix(StaticFunctionTag* base, BSFixedString prefix) {
		UInt32 count = 0;
		std::string key = prefix.data;
		if (!key.empty()) {
			count += Data::GetValues<SInt32, SInt32>()->CountPrefixKey(key);
			count += Data::GetValues<float, float>()->CountPrefixKey(key);
			count += Data::GetValues<BSFixedString, std::string>()->CountPrefixKey(key);
			count += Data::GetValues<TESForm*, UInt32>()->CountPrefixKey(key);
			// Lists
			count += Data::GetLists<SInt32, SInt32>()->CountPrefixKey(key);
			count += Data::GetLists<float, float>()->CountPrefixKey(key);
			count += Data::GetLists<BSFixedString, std::string>()->CountPrefixKey(key);
			count += Data::GetLists<TESForm*, UInt32>()->CountPrefixKey(key);
		}
		return count;
	}

	template <typename T, typename S>
	UInt32 ObjCountValuesPrefixKey(StaticFunctionTag* base, TESForm* obj, BSFixedString prefix) {
		return Data::GetValues<T, S>()->CountPrefixKey(GetFormKey(obj), prefix.data);
	}

	template <typename T, typename S>
	UInt32 ObjCountListsPrefixKey(StaticFunctionTag* base, TESForm* obj, BSFixedString prefix) {
		return Data::GetLists<T, S>()->CountPrefixKey(GetFormKey(obj), prefix.data);
	}
	
	UInt32 ObjCountAllPrefix(StaticFunctionTag* base, TESForm* obj, BSFixedString prefix) {
		UInt32 count  = 0;
		UInt64 objkey = GetFormKey(obj);
		std::string key = prefix.data;
		if (!key.empty()) {
			count += Data::GetValues<SInt32, SInt32>()->CountPrefixKey(objkey, key);
			count += Data::GetValues<float, float>()->CountPrefixKey(objkey, key);
			count += Data::GetValues<BSFixedString, std::string>()->CountPrefixKey(objkey, key);
			count += Data::GetValues<TESForm*, UInt32>()->CountPrefixKey(objkey, key);
			// Lists
			count += Data::GetLists<SInt32, SInt32>()->CountPrefixKey(objkey, key);
			count += Data::GetLists<float, float>()->CountPrefixKey(objkey, key);
			count += Data::GetLists<BSFixedString, std::string>()->CountPrefixKey(objkey, key);
			count += Data::GetLists<TESForm*, UInt32>()->CountPrefixKey(objkey, key);
		}
		return count;
	}

	template <typename T, typename S>
	UInt32 ClearValuesPrefixKey(StaticFunctionTag* base, BSFixedString prefix) {
		return Data::GetValues<T, S>()->ClearPrefixKey(prefix.data);
	}

	template <typename T, typename S>
	UInt32 ClearListsPrefixKey(StaticFunctionTag* base, BSFixedString prefix) {
		return Data::GetLists<T, S>()->ClearPrefixKey(prefix.data);
	}

	UInt32 ClearAllPrefix(StaticFunctionTag* base, BSFixedString prefix) {
		UInt32 count = 0;
		std::string key = prefix.data;
		if (!key.empty()) {
			count += Data::GetValues<SInt32, SInt32>()->ClearPrefixKey(key);
			count += Data::GetValues<float, float>()->ClearPrefixKey(key);
			count += Data::GetValues<BSFixedString, std::string>()->ClearPrefixKey(key);
			count += Data::GetValues<TESForm*, UInt32>()->ClearPrefixKey(key);
			// Lists
			count += Data::GetLists<SInt32, SInt32>()->ClearPrefixKey(key);
			count += Data::GetLists<float, float>()->ClearPrefixKey(key);
			count += Data::GetLists<BSFixedString, std::string>()->ClearPrefixKey(key);
			count += Data::GetLists<TESForm*, UInt32>()->ClearPrefixKey(key);
		}
		return count;
	}

	template <typename T, typename S>
	UInt32 ObjClearValuesPrefixKey(StaticFunctionTag* base, TESForm* obj, BSFixedString prefix) {
		return Data::GetValues<T, S>()->ClearPrefixKey(GetFormKey(obj), prefix.data);
	}

	template <typename T, typename S>
	UInt32 ObjClearListsPrefixKey(StaticFunctionTag* base, TESForm* obj, BSFixedString prefix) {
		return Data::GetLists<T, S>()->ClearPrefixKey(GetFormKey(obj), prefix.data);
	}

	UInt32 ObjClearAllPrefix(StaticFunctionTag* base, TESForm* obj, BSFixedString prefix) {
		UInt32 count = 0;
		UInt64 objkey = GetFormKey(obj);
		std::string key = prefix.data;
		if (!key.empty()) {
			count += Data::GetValues<SInt32, SInt32>()->ClearPrefixKey(objkey, key);
			count += Data::GetValues<float, float>()->ClearPrefixKey(objkey, key);
			count += Data::GetValues<BSFixedString, std::string>()->ClearPrefixKey(objkey, key);
			count += Data::GetValues<TESForm*, UInt32>()->ClearPrefixKey(objkey, key);
			// Lists
			count += Data::GetLists<SInt32, SInt32>()->ClearPrefixKey(objkey, key);
			count += Data::GetLists<float, float>()->ClearPrefixKey(objkey, key);
			count += Data::GetLists<BSFixedString, std::string>()->ClearPrefixKey(objkey, key);
			count += Data::GetLists<TESForm*, UInt32>()->ClearPrefixKey(objkey, key);
		}
		return count;
	}

	VMResultArray<TESForm*> FilterByTypes(StaticFunctionTag* base, TESForm* obj, BSFixedString key, VMArray<UInt32> types, bool matching) {
		Data::Lists<TESForm*, UInt32>* Data = Data::GetLists<TESForm*, UInt32>();
		if (!Data || !IsValidKey(key)) { VMResultArray<TESForm*> arr;  return arr; }
		else return Data->FilterByTypes(GetFormKey(obj), key.data, types, matching);
	}

	template <typename T, typename S>
	T ListRandom(StaticFunctionTag* base, TESForm* obj, BSFixedString key) {
		Data::Lists<T, S>* Data = Data::GetLists<T, S>();
		if (!Data || !IsValidKey(key)) return Empty<T>();
		else return Data->ListRandom(GetFormKey(obj), key.data);
	}




} // StorageUtil

#ifdef _GLOBAL_EXTERNAL
#include "External.h"
using namespace External;

namespace StorageUtil {
	// Global external StorageUtil.json file
	template <typename T>
	T FileSetValue(StaticFunctionTag* base, BSFixedString key, T value) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->SetValue(List<T>(), key.data, MakeValue<T>(value)));
	}

	template <typename T>
	T FileGetValue(StaticFunctionTag* base, BSFixedString key, T missing) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->GetValue(List<T>(), key.data, MakeValue<T>(missing)));
	}

	template <typename T>
	T FileAdjustValue(StaticFunctionTag* base, BSFixedString key, T value) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->AdjustValue(List<T>(), key.data, MakeValue<T>(value)));
	}

	template <typename T>
	bool FileUnsetValue(StaticFunctionTag* base, BSFixedString key) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return false;
		else return File->UnsetValue(List<T>(), key.data);
	}

	template <typename T>
	bool FileHasValue(StaticFunctionTag* base, BSFixedString key) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return false;
		else return File->HasValue(List<T>(), key.data);
	}

	template <typename T>
	SInt32 FileListAdd(StaticFunctionTag* base, BSFixedString key, T value, bool allowDuplicate) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return -1;
		else return File->ListAdd(List<T>(), key.data, MakeValue<T>(value), allowDuplicate);
	}

	template <typename T>
	T FileListGet(StaticFunctionTag* base, BSFixedString key, UInt32 index) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->ListGet(List<T>(), key.data, index));
	}

	template <typename T>
	T FileListSet(StaticFunctionTag* base, BSFixedString key, UInt32 index, T value) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->ListSet(List<T>(), key.data, index, MakeValue<T>(value)));
	}

	template <typename T>
	T FileListAdjust(StaticFunctionTag* base, BSFixedString key, UInt32 index, T adjustBy) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return Empty<T>();
		else return ParseValue<T>(File->ListAdjust(key.data, index, adjustBy));
	}

	template <typename T>
	UInt32 FileListRemove(StaticFunctionTag* base, BSFixedString key, T value, bool allInstances) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return 0;
		else return File->ListRemove(List<T>(), key.data, MakeValue<T>(value), allInstances);
	}

	template <typename T>
	bool FileListRemoveAt(StaticFunctionTag* base, BSFixedString key, UInt32 index) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return false;
		else return File->ListRemoveAt(List<T>(), key.data, index);
	}

	template <typename T>
	bool FileListInsertAt(StaticFunctionTag* base, BSFixedString key, UInt32 index, T value) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return false;
		else return File->ListInsertAt(List<T>(), key.data, index, MakeValue<T>(value));
	}

	template <typename T>
	UInt32 FileListClear(StaticFunctionTag* base, BSFixedString key) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return 0;
		else return File->ListClear(List<T>(), key.data);
	}

	template <typename T>
	UInt32 FileListCount(StaticFunctionTag* base, BSFixedString key) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return 0;
		return File->ListCount(List<T>(), key.data);
	}

	template <typename T>
	SInt32 FileListFind(StaticFunctionTag* base, BSFixedString key, T value) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return -1;
		else return File->ListFind(List<T>(), key.data, MakeValue<T>(value));
	}

	template <typename T>
	bool FileListHas(StaticFunctionTag* base, BSFixedString key, T value) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key)) return false;
		else return File->ListFind(List<T>(), key.data, MakeValue<T>(value)) != -1;
	}

	template <typename T>
	SInt32 FileListResize(StaticFunctionTag* base, BSFixedString key, UInt32 length, T filler) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key) || length > 500) return 0;
		else return File->ListResize(List<T>(), key.data, length, MakeValue<T>(filler));
	}

	template <typename T>
	void FileListSlice(StaticFunctionTag* base, BSFixedString key, VMArray<T> Output, UInt32 startIndex){
		ExternalFile* File = GetSingleton();
		if (File && Output.arr && Output.Length() > 0 && startIndex >= 0 && IsValidKey(key))
			File->ListSlice<T>(key.data, Output, startIndex);
	}

	template <typename T>
	bool FileListCopy(StaticFunctionTag* base, BSFixedString key, VMArray<T> Input) {
		ExternalFile* File = GetSingleton();
		if (!File || !IsValidKey(key) || !Input.arr || Input.Length() < 1) return false;
		else return File->ListCopy<T>(key.data, Input);
	}


	void SaveExternalFile(StaticFunctionTag* base) {
		External::ExternalFile* File = External::GetSingleton();
		if (File) File->SaveFile(false);
	}
} // StorageUtil
#endif


#include "skse64/PapyrusNativeFunctions.h"

void StorageUtil::RegisterFuncs(VMClassRegistry* registry) {
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, SInt32>("SetIntValue", "StorageUtil", SetValue<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, float, TESForm*, BSFixedString, float>("SetFloatValue", "StorageUtil", SetValue<float, float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, BSFixedString, TESForm*, BSFixedString, BSFixedString>("SetStringValue", "StorageUtil", SetValue<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, TESForm*, TESForm*, BSFixedString, TESForm*>("SetFormValue", "StorageUtil", SetValue<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "SetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "SetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "SetStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "SetFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, SInt32>("GetIntValue", "StorageUtil", GetValue<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, float, TESForm*, BSFixedString, float>("GetFloatValue", "StorageUtil", GetValue<float, float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, BSFixedString, TESForm*, BSFixedString, BSFixedString>("GetStringValue", "StorageUtil", GetValue<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, TESForm*, TESForm*, BSFixedString, TESForm*>("GetFormValue", "StorageUtil", GetValue<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "GetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "GetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "GetStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "GetFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, SInt32>("AdjustIntValue", "StorageUtil", AdjustValue<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, float, TESForm*, BSFixedString, float>("AdjustFloatValue", "StorageUtil", AdjustValue<float, float>, registry));
	registry->SetFunctionFlags("StorageUtil", "AdjustIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "AdjustFloatValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, SInt32>("PluckIntValue", "StorageUtil", PluckValue<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, float, TESForm*, BSFixedString, float>("PluckFloatValue", "StorageUtil", PluckValue<float, float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, BSFixedString, TESForm*, BSFixedString, BSFixedString>("PluckStringValue", "StorageUtil", PluckValue<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, TESForm*, TESForm*, BSFixedString, TESForm*>("PluckFormValue", "StorageUtil", PluckValue<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "PluckIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "PluckFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "PluckStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "PluckFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, TESForm*, BSFixedString>("UnsetIntValue", "StorageUtil", UnsetValue<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, TESForm*, BSFixedString>("UnsetFloatValue", "StorageUtil", UnsetValue<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, TESForm*, BSFixedString>("UnsetStringValue", "StorageUtil", UnsetValue<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, TESForm*, BSFixedString>("UnsetFormValue", "StorageUtil", UnsetValue<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "UnsetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "UnsetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "UnsetStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "UnsetFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, TESForm*, BSFixedString>("HasIntValue", "StorageUtil", HasValue<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, TESForm*, BSFixedString>("HasFloatValue", "StorageUtil", HasValue<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, TESForm*, BSFixedString>("HasStringValue", "StorageUtil", HasValue<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, TESForm*, BSFixedString>("HasFormValue", "StorageUtil", HasValue<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "HasIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "HasFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "HasStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "HasFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, SInt32, bool>("IntListAdd", "StorageUtil", ListAdd<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, float, bool>("FloatListAdd", "StorageUtil", ListAdd<float, float>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, BSFixedString, bool>("StringListAdd", "StorageUtil", ListAdd<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, TESForm*, bool>("FormListAdd", "StorageUtil", ListAdd<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListAdd", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListAdd", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListAdd", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListAdd", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, UInt32>("IntListGet", "StorageUtil", ListGet<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, float, TESForm*, BSFixedString, UInt32>("FloatListGet", "StorageUtil", ListGet<float, float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, BSFixedString, TESForm*, BSFixedString, UInt32>("StringListGet", "StorageUtil", ListGet<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, TESForm*, TESForm*, BSFixedString, UInt32>("FormListGet", "StorageUtil", ListGet<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListGet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListGet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListGet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListGet", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, UInt32, SInt32>("IntListSet", "StorageUtil", ListSet<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, float, TESForm*, BSFixedString, UInt32, float>("FloatListSet", "StorageUtil", ListSet<float, float>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, BSFixedString, TESForm*, BSFixedString, UInt32, BSFixedString>("StringListSet", "StorageUtil", ListSet<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, TESForm*, TESForm*, BSFixedString, UInt32, TESForm*>("FormListSet", "StorageUtil", ListSet<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListSet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListSet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListSet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListSet", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, UInt32, SInt32>("IntListPluck", "StorageUtil", ListPluck<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, float, TESForm*, BSFixedString, UInt32, float>("FloatListPluck", "StorageUtil", ListPluck<float, float>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, BSFixedString, TESForm*, BSFixedString, UInt32, BSFixedString>("StringListPluck", "StorageUtil", ListPluck<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, TESForm*, TESForm*, BSFixedString, UInt32, TESForm*>("FormListPluck", "StorageUtil", ListPluck<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListPluck", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListPluck", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListPluck", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListPluck", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, SInt32, TESForm*, BSFixedString>("IntListShift", "StorageUtil", ListShift<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, float, TESForm*, BSFixedString>("FloatListShift", "StorageUtil", ListShift<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, BSFixedString>("StringListShift", "StorageUtil", ListShift<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, TESForm*, TESForm*, BSFixedString>("FormListShift", "StorageUtil", ListShift<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListShift", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListShift", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListShift", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListShift", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, SInt32, TESForm*, BSFixedString>("IntListPop", "StorageUtil", ListPop<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, float, TESForm*, BSFixedString>("FloatListPop", "StorageUtil", ListPop<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, BSFixedString>("StringListPop", "StorageUtil", ListPop<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, TESForm*, TESForm*, BSFixedString>("FormListPop", "StorageUtil", ListPop<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListPop", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListPop", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListPop", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListPop", VMClassRegistry::kFunctionFlag_NoWait);



	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, UInt32, SInt32>("IntListAdjust", "StorageUtil", ListAdjust<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, float, TESForm*, BSFixedString, UInt32, float>("FloatListAdjust", "StorageUtil", ListAdjust<float, float>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListAdjust", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListAdjust", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, TESForm*, BSFixedString, SInt32, bool>("IntListRemove", "StorageUtil", ListRemove<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, TESForm*, BSFixedString, float, bool>("FloatListRemove", "StorageUtil", ListRemove<float, float>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, TESForm*, BSFixedString, BSFixedString, bool>("StringListRemove", "StorageUtil", ListRemove<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, TESForm*, BSFixedString, TESForm*, bool>("FormListRemove", "StorageUtil", ListRemove<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListRemove", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListRemove", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListRemove", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListRemove", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, bool, TESForm*, BSFixedString, UInt32, SInt32>("IntListInsert", "StorageUtil", ListInsertAt<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, bool, TESForm*, BSFixedString, UInt32, float>("FloatListInsert", "StorageUtil", ListInsertAt<float, float>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, bool, TESForm*, BSFixedString, UInt32, BSFixedString>("StringListInsert", "StorageUtil", ListInsertAt<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, bool, TESForm*, BSFixedString, UInt32, TESForm*>("FormListInsert", "StorageUtil", ListInsertAt<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListInsert", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListInsert", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListInsert", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListInsert", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, UInt32>("IntListRemoveAt", "StorageUtil", ListRemoveAt<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, UInt32>("FloatListRemoveAt", "StorageUtil", ListRemoveAt<float, float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, UInt32>("StringListRemoveAt", "StorageUtil", ListRemoveAt<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, UInt32>("FormListRemoveAt", "StorageUtil", ListRemoveAt<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("IntListClear", "StorageUtil", ListClear<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("FloatListClear", "StorageUtil", ListClear<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("StringListClear", "StorageUtil", ListClear<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("FormListClear", "StorageUtil", ListClear<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListClear", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListClear", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListClear", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListClear", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("IntListCount", "StorageUtil", ListCount<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("FloatListCount", "StorageUtil", ListCount<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("StringListCount", "StorageUtil", ListCount<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("FormListCount", "StorageUtil", ListCount<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListCount", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, TESForm*, BSFixedString, SInt32, bool>("IntListCountValue", "StorageUtil", ListCountValue<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, TESForm*, BSFixedString, float, bool>("FloatListCountValue", "StorageUtil", ListCountValue<float, float>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, TESForm*, BSFixedString, BSFixedString, bool>("StringListCountValue", "StorageUtil", ListCountValue<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, TESForm*, BSFixedString, TESForm*, bool>("FormListCountValue", "StorageUtil", ListCountValue<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListCountValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListCountValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListCountValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListCountValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, SInt32>("IntListFind", "StorageUtil", ListFind<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, float>("FloatListFind", "StorageUtil", ListFind<float, float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, BSFixedString>("StringListFind", "StorageUtil", ListFind<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, TESForm*>("FormListFind", "StorageUtil", ListFind<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListFind", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListFind", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListFind", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListFind", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, SInt32>("IntListHas", "StorageUtil", ListHas<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, float>("FloatListHas", "StorageUtil", ListHas<float, float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, BSFixedString>("StringListHas", "StorageUtil", ListHas<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, TESForm*>("FormListHas", "StorageUtil", ListHas<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListHas", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListHas", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListHas", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListHas", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, TESForm*, BSFixedString>("IntListSort", "StorageUtil", ListSort<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, TESForm*, BSFixedString>("FloatListSort", "StorageUtil", ListSort<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, TESForm*, BSFixedString>("StringListSort", "StorageUtil", ListSort<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, TESForm*, BSFixedString>("FormListSort", "StorageUtil", ListSort<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListSort", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListSort", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListSort", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListSort", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, void, TESForm*, BSFixedString, VMArray<SInt32>, UInt32>("IntListSlice", "StorageUtil", ListSlice<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, void, TESForm*, BSFixedString, VMArray<float>, UInt32>("FloatListSlice", "StorageUtil", ListSlice<float, float>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, void, TESForm*, BSFixedString, VMArray<BSFixedString>, UInt32>("StringListSlice", "StorageUtil", ListSlice<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, void, TESForm*, BSFixedString, VMArray<TESForm*>, UInt32>("FormListSlice", "StorageUtil", ListSlice<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListSlice", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListSlice", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListSlice", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListSlice", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, UInt32, SInt32>("IntListResize", "StorageUtil", ListResize<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, UInt32, float>("FloatListResize", "StorageUtil", ListResize<float, float>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, UInt32, BSFixedString>("StringListResize", "StorageUtil", ListResize<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, SInt32, TESForm*, BSFixedString, UInt32, TESForm*>("FormListResize", "StorageUtil", ListResize<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListResize", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListResize", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListResize", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListResize", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, VMArray<SInt32>>("IntListCopy", "StorageUtil", ListCopy<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, VMArray<float>>("FloatListCopy", "StorageUtil", ListCopy<float, float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, VMArray<BSFixedString>>("StringListCopy", "StorageUtil", ListCopy<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, TESForm*, BSFixedString, VMArray<TESForm*>>("FormListCopy", "StorageUtil", ListCopy<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListCopy", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListCopy", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListCopy", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListCopy", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, VMResultArray<SInt32>, TESForm*, BSFixedString>("IntListToArray", "StorageUtil", ToArray<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, VMResultArray<float>, TESForm*, BSFixedString>("FloatListToArray", "StorageUtil", ToArray<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, VMResultArray<BSFixedString>, TESForm*, BSFixedString>("StringListToArray", "StorageUtil", ToArray<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, VMResultArray<TESForm*>, TESForm*, BSFixedString>("FormListToArray", "StorageUtil", ToArray<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListToArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListToArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListToArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListToArray", VMClassRegistry::kFunctionFlag_NoWait);


	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("CountIntValuePrefix", "StorageUtil", CountValuesPrefixKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("CountFloatValuePrefix", "StorageUtil", CountValuesPrefixKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("CountStringValuePrefix", "StorageUtil", CountValuesPrefixKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("CountFormValuePrefix", "StorageUtil", CountValuesPrefixKey<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "CountIntValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountFloatValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountStringValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountFormValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("CountIntListPrefix", "StorageUtil", CountListsPrefixKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("CountFloatListPrefix", "StorageUtil", CountListsPrefixKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("CountStringListPrefix", "StorageUtil", CountListsPrefixKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("CountFormListPrefix", "StorageUtil", CountListsPrefixKey<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "CountIntListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountFloatListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountStringListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountFormListPrefix", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("CountAllPrefix", "StorageUtil", CountAllPrefix, registry));

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("ClearIntValuePrefix", "StorageUtil", ClearValuesPrefixKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("ClearFloatValuePrefix", "StorageUtil", ClearValuesPrefixKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("ClearStringValuePrefix", "StorageUtil", ClearValuesPrefixKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("ClearFormValuePrefix", "StorageUtil", ClearValuesPrefixKey<TESForm*, UInt32>, registry));

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("ClearIntListPrefix", "StorageUtil", ClearListsPrefixKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("ClearFloatListPrefix", "StorageUtil", ClearListsPrefixKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("ClearStringListPrefix", "StorageUtil", ClearListsPrefixKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("ClearFormListPrefix", "StorageUtil", ClearListsPrefixKey<TESForm*, UInt32>, registry));

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("ClearAllPrefix", "StorageUtil", ClearAllPrefix, registry));


	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("CountObjIntValuePrefix", "StorageUtil", ObjCountValuesPrefixKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("CountObjFloatValuePrefix", "StorageUtil", ObjCountValuesPrefixKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("CountObjStringValuePrefix", "StorageUtil", ObjCountValuesPrefixKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("CountObjFormValuePrefix", "StorageUtil", ObjCountValuesPrefixKey<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "CountObjIntValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountObjFloatValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountObjStringValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountObjFormValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("CountObjIntListPrefix", "StorageUtil", ObjCountListsPrefixKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("CountObjFloatListPrefix", "StorageUtil", ObjCountListsPrefixKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("CountObjStringListPrefix", "StorageUtil", ObjCountListsPrefixKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("CountObjFormListPrefix", "StorageUtil", ObjCountListsPrefixKey<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "CountObjIntListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountObjFloatListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountObjStringListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "CountObjFormListPrefix", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("CountAllObjPrefix", "StorageUtil", ObjCountAllPrefix, registry));

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("ClearObjIntValuePrefix", "StorageUtil", ObjClearValuesPrefixKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("ClearObjFloatValuePrefix", "StorageUtil", ObjClearValuesPrefixKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("ClearObjStringValuePrefix", "StorageUtil", ObjClearValuesPrefixKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("ClearObjFormValuePrefix", "StorageUtil", ObjClearValuesPrefixKey<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "ClearObjIntValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "ClearObjFloatValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "ClearObjStringValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "ClearObjFormValuePrefix", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("ClearObjIntListPrefix", "StorageUtil", ObjClearListsPrefixKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("ClearObjFloatListPrefix", "StorageUtil", ObjClearListsPrefixKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("ClearObjStringListPrefix", "StorageUtil", ObjClearListsPrefixKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("ClearObjFormListPrefix", "StorageUtil", ObjClearListsPrefixKey<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "ClearObjIntListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "ClearObjFloatListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "ClearObjStringListPrefix", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "ClearObjFormListPrefix", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, UInt32, TESForm*, BSFixedString>("ClearAllObjPrefix", "StorageUtil", ObjClearAllPrefix, registry));

	registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, VMResultArray<TESForm*>, TESForm*, BSFixedString, VMArray<UInt32>, bool>("FormListFilterByTypes", "StorageUtil", FilterByTypes, registry));
	registry->SetFunctionFlags("StorageUtil", "FormListFilterByTypes", VMClassRegistry::kFunctionFlag_NoWait);


	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, SInt32, TESForm*, BSFixedString>("IntListRandom", "StorageUtil", ListRandom<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, float, TESForm*, BSFixedString>("FloatListRandom", "StorageUtil", ListRandom<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, BSFixedString>("StringListRandom", "StorageUtil", ListRandom<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, TESForm*, TESForm*, BSFixedString>("FormListRandom", "StorageUtil", ListRandom<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "IntListRandom", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FloatListRandom", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "StringListRandom", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FormListRandom", VMClassRegistry::kFunctionFlag_NoWait);


	// Debug Functions
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, TESForm*>("debug_DeleteValues", "StorageUtil", DeleteValues, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_DeleteValues", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("debug_DeleteAllValues", "StorageUtil", DeleteAllValues, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_DeleteAllValues", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction0<StaticFunctionTag, UInt32>("debug_Cleanup", "StorageUtil", Cleanup, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_Cleanup", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("debug_GetIntObjectCount", "StorageUtil", GetObjectCount<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("debug_GetFloatObjectCount", "StorageUtil", GetObjectCount<float, float>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("debug_GetStringObjectCount", "StorageUtil", GetObjectCount<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("debug_GetFormObjectCount", "StorageUtil", GetObjectCount<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_GetIntObjectCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFloatObjectCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetStringObjectCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFormObjectCount", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("debug_GetIntListObjectCount", "StorageUtil", GetListsObjectCount<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("debug_GetFloatListObjectCount", "StorageUtil", GetListsObjectCount<float, float>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("debug_GetStringListObjectCount", "StorageUtil", GetListsObjectCount<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("debug_GetFormListObjectCount", "StorageUtil", GetListsObjectCount<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_GetIntListObjectCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFloatListObjectCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetStringListObjectCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFormListObjectCount", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, TESForm*>("debug_GetIntKeysCount", "StorageUtil", GetKeyCount<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, TESForm*>("debug_GetFloatKeysCount", "StorageUtil", GetKeyCount<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, TESForm*>("debug_GetStringKeysCount", "StorageUtil", GetKeyCount<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, TESForm*>("debug_GetFormKeysCount", "StorageUtil", GetKeyCount<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_GetIntKeysCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFloatKeysCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetStringKeysCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFormKeysCount", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, TESForm*>("debug_GetIntListKeysCount", "StorageUtil", GetListsKeyCount<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, TESForm*>("debug_GetFloatListKeysCount", "StorageUtil", GetListsKeyCount<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, TESForm*>("debug_GetStringListKeysCount", "StorageUtil", GetListsKeyCount<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, UInt32, TESForm*>("debug_GetFormListKeysCount", "StorageUtil", GetListsKeyCount<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_GetIntListKeysCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFloatListKeysCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetStringListKeysCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFormListKeysCount", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, TESForm*, UInt32>("debug_GetIntObject", "StorageUtil", GetNthObj<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, TESForm*, UInt32>("debug_GetFloatObject", "StorageUtil", GetNthObj<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, TESForm*, UInt32>("debug_GetStringObject", "StorageUtil", GetNthObj<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, TESForm*, UInt32>("debug_GetFormObject", "StorageUtil", GetNthObj<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_GetIntObject", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFloatObject", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetStringObject", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFormObject", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, TESForm*, UInt32>("debug_GetIntListObject", "StorageUtil", GetListsNthObj<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, TESForm*, UInt32>("debug_GetFloatListObject", "StorageUtil", GetListsNthObj<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, TESForm*, UInt32>("debug_GetStringListObject", "StorageUtil", GetListsNthObj<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, TESForm*, UInt32>("debug_GetFormListObject", "StorageUtil", GetListsNthObj<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_GetIntListObject", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFloatListObject", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetStringListObject", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFormListObject", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, UInt32>("debug_GetIntKey", "StorageUtil", GetNthKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, UInt32>("debug_GetFloatKey", "StorageUtil", GetNthKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, UInt32>("debug_GetStringKey", "StorageUtil", GetNthKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, UInt32>("debug_GetFormKey", "StorageUtil", GetNthKey<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_GetIntKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFloatKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetStringKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFormKey", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, UInt32>("debug_GetIntListKey", "StorageUtil", GetListsNthKey<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, UInt32>("debug_GetFloatListKey", "StorageUtil", GetListsNthKey<float, float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, UInt32>("debug_GetStringListKey", "StorageUtil", GetListsNthKey<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, BSFixedString, TESForm*, UInt32>("debug_GetFormListKey", "StorageUtil", GetListsNthKey<TESForm*, UInt32>, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_GetIntListKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFloatListKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetStringListKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "debug_GetFormListKey", VMClassRegistry::kFunctionFlag_NoWait);


	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, VMResultArray<TESForm*>>("debug_AllIntListObjs", "StorageUtil", GetListsAllObj<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, VMResultArray<TESForm*>>("debug_AllFloatListObjs", "StorageUtil", GetListsAllObj<float, float>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, VMResultArray<TESForm*>>("debug_AllStringListObjs", "StorageUtil", GetListsAllObj<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, VMResultArray<TESForm*>>("debug_AllFormListObjs", "StorageUtil", GetListsAllObj<TESForm*, UInt32>, registry));
	//registry->SetFunctionFlags("StorageUtil", "debug_AllIntListObjs", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllFloatListObjs", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllStringListObjs", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllFormListObjs", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, VMResultArray<TESForm*>>("debug_AllIntObjs", "StorageUtil", GetValuesAllObj<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, VMResultArray<TESForm*>>("debug_AllFloatObjs", "StorageUtil", GetValuesAllObj<float, float>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, VMResultArray<TESForm*>>("debug_AllStringObjs", "StorageUtil", GetValuesAllObj<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, VMResultArray<TESForm*>>("debug_AllFormObjs", "StorageUtil", GetValuesAllObj<TESForm*, UInt32>, registry));
	//registry->SetFunctionFlags("StorageUtil", "debug_AllIntObjs", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllFloatObjs", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllStringObjs", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllFormObjs", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, VMResultArray<BSFixedString>, TESForm*>("debug_AllObjIntListKeys", "StorageUtil", GetListsObjKeys<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, VMResultArray<BSFixedString>, TESForm*>("debug_AllObjFloatListKeys", "StorageUtil", GetListsObjKeys<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, VMResultArray<BSFixedString>, TESForm*>("debug_AllObjStringListKeys", "StorageUtil", GetListsObjKeys<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, VMResultArray<BSFixedString>, TESForm*>("debug_AllObjFormListKeys", "StorageUtil", GetListsObjKeys<TESForm*, UInt32>, registry));
	//registry->SetFunctionFlags("StorageUtil", "debug_AllObjIntListKeys", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllObjFloatListKeys", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllObjStringListKeys", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllObjFormListKeys", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, VMResultArray<BSFixedString>, TESForm*>("debug_AllObjIntKeys", "StorageUtil", GetValuesObjKeys<SInt32, SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, VMResultArray<BSFixedString>, TESForm*>("debug_AllObjFloatKeys", "StorageUtil", GetValuesObjKeys<float, float>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, VMResultArray<BSFixedString>, TESForm*>("debug_AllObjStringKeys", "StorageUtil", GetValuesObjKeys<BSFixedString, std::string>, registry));
	registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, VMResultArray<BSFixedString>, TESForm*>("debug_AllObjFormKeys", "StorageUtil", GetValuesObjKeys<TESForm*, UInt32>, registry));
	//registry->SetFunctionFlags("StorageUtil", "debug_AllObjIntKeys", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllObjFloatKeys", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllObjStringKeys", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("StorageUtil", "debug_AllObjFormKeys", VMClassRegistry::kFunctionFlag_NoWait);


#ifdef _GLOBAL_EXTERNAL
	// Global file values
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, SInt32>("FileSetIntValue", "StorageUtil", FileSetValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, float, BSFixedString, float>("FileSetFloatValue", "StorageUtil", FileSetValue<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("FileSetStringValue", "StorageUtil", FileSetValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, TESForm*, BSFixedString, TESForm*>("FileSetFormValue", "StorageUtil", FileSetValue<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileSetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileSetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileSetStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileSetFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, SInt32>("FileGetIntValue", "StorageUtil", FileGetValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, float, BSFixedString, float>("FileGetFloatValue", "StorageUtil", FileGetValue<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("FileGetStringValue", "StorageUtil", FileGetValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, TESForm*, BSFixedString, TESForm*>("FileGetFormValue", "StorageUtil", FileGetValue<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileGetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileGetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileGetStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileGetFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, SInt32>("FileAdjustIntValue", "StorageUtil", FileAdjustValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, float, BSFixedString, float>("FileAdjustFloatValue", "StorageUtil", FileAdjustValue<float>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileAdjustIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileAdjustFloatValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("FileUnsetIntValue", "StorageUtil", FileUnsetValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("FileUnsetFloatValue", "StorageUtil", FileUnsetValue<float>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("FileUnsetStringValue", "StorageUtil", FileUnsetValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("FileUnsetFormValue", "StorageUtil", FileUnsetValue<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileUnsetIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileUnsetFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileUnsetStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileUnsetFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("FileHasIntValue", "StorageUtil", FileHasValue<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("FileHasFloatValue", "StorageUtil", FileHasValue<float>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("FileHasStringValue", "StorageUtil", FileHasValue<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("FileHasFormValue", "StorageUtil", FileHasValue<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileHasIntValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileHasFloatValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileHasStringValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileHasFormValue", VMClassRegistry::kFunctionFlag_NoWait);

	// Global file lists
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, SInt32, bool>("FileIntListAdd", "StorageUtil", FileListAdd<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, float, bool>("FileFloatListAdd", "StorageUtil", FileListAdd<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, BSFixedString, bool>("FileStringListAdd", "StorageUtil", FileListAdd<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, TESForm*, bool>("FileFormListAdd", "StorageUtil", FileListAdd<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListAdd", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListAdd", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListAdd", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListAdd", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, UInt32>("FileIntListGet", "StorageUtil", FileListGet<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, float, BSFixedString, UInt32>("FileFloatListGet", "StorageUtil", FileListGet<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, BSFixedString, BSFixedString, UInt32>("FileStringListGet", "StorageUtil", FileListGet<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, TESForm*, BSFixedString, UInt32>("FileFormListGet", "StorageUtil", FileListGet<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListGet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListGet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListGet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListGet", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, UInt32, SInt32>("FileIntListSet", "StorageUtil", FileListSet<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, UInt32, float>("FileFloatListSet", "StorageUtil", FileListSet<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, UInt32, BSFixedString>("FileStringListSet", "StorageUtil", FileListSet<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, TESForm*, BSFixedString, UInt32, TESForm*>("FileFormListSet", "StorageUtil", FileListSet<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListSet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListSet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListSet", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListSet", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, BSFixedString, UInt32, SInt32>("FileIntListAdjust", "StorageUtil", FileListAdjust<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, BSFixedString, UInt32, float>("FileFloatListAdjust", "StorageUtil", FileListAdjust<float>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListAdjust", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListAdjust", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, UInt32, BSFixedString, SInt32, bool>("FileIntListRemove", "StorageUtil", FileListRemove<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, UInt32, BSFixedString, float, bool>("FileFloatListRemove", "StorageUtil", FileListRemove<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, bool>("FileStringListRemove", "StorageUtil", FileListRemove<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, UInt32, BSFixedString, TESForm*, bool>("FileFormListRemove", "StorageUtil", FileListRemove<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListRemove", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListRemove", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListRemove", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListRemove", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, UInt32>("FileIntListRemoveAt", "StorageUtil", FileListRemoveAt<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, UInt32>("FileFloatListRemoveAt", "StorageUtil", FileListRemoveAt<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, UInt32>("FileStringListRemoveAt", "StorageUtil", FileListRemoveAt<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, UInt32>("FileFormListRemoveAt", "StorageUtil", FileListRemoveAt<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListRemoveAt", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, UInt32, SInt32>("FileIntListInsert", "StorageUtil", FileListInsertAt<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, UInt32, float>("FileFloatListInsert", "StorageUtil", FileListInsertAt<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, UInt32, BSFixedString>("FileStringListInsert", "StorageUtil", FileListInsertAt<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, BSFixedString, UInt32, TESForm*>("FileFormListInsert", "StorageUtil", FileListInsertAt<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListInsert", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListInsert", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListInsert", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListInsert", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>("FileIntListClear", "StorageUtil", FileListClear<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>("FileFloatListClear", "StorageUtil", FileListClear<float>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>("FileStringListClear", "StorageUtil", FileListClear<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>("FileFormListClear", "StorageUtil", FileListClear<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListClear", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListClear", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListClear", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListClear", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>("FileIntListCount", "StorageUtil", FileListCount<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>("FileFloatListCount", "StorageUtil", FileListCount<float>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>("FileStringListCount", "StorageUtil", FileListCount<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>("FileFormListCount", "StorageUtil", FileListCount<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListCount", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, SInt32>("FileIntListFind", "StorageUtil", FileListFind<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, float>("FileFloatListFind", "StorageUtil", FileListFind<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, BSFixedString>("FileStringListFind", "StorageUtil", FileListFind<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, BSFixedString, TESForm*>("FileFormListFind", "StorageUtil", FileListFind<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListFind", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListFind", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListFind", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListFind", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, SInt32>("FileIntListHas", "StorageUtil", FileListHas<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, float>("FileFloatListHas", "StorageUtil", FileListHas<float>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, BSFixedString>("FileStringListHas", "StorageUtil", FileListHas<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, BSFixedString, TESForm*>("FileFormListHas", "StorageUtil", FileListHas<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListHas", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListHas", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListHas", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListHas", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, VMArray<SInt32>, UInt32>("FileIntListSlice", "StorageUtil", FileListSlice<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, VMArray<float>, UInt32>("FileFloatListSlice", "StorageUtil", FileListSlice<float>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, VMArray<BSFixedString>, UInt32>("FileStringListSlice", "StorageUtil", FileListSlice<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, VMArray<TESForm*>, UInt32>("FileFormListSlice", "StorageUtil", FileListSlice<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListSlice", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListSlice", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListSlice", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListSlice", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, BSFixedString, UInt32, SInt32>("FileIntListResize", "StorageUtil", FileListResize<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, BSFixedString, UInt32, float>("FileFloatListResize", "StorageUtil", FileListResize<float>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, BSFixedString, UInt32, BSFixedString>("FileStringListResize", "StorageUtil", FileListResize<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, SInt32, BSFixedString, UInt32, TESForm*>("FileFormListResize", "StorageUtil", FileListResize<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListResize", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListResize", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListResize", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListResize", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, VMArray<SInt32>>("FileIntListCopy", "StorageUtil", FileListCopy<SInt32>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, VMArray<float>>("FileFloatListCopy", "StorageUtil", FileListCopy<float>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, VMArray<BSFixedString>>("FileStringListCopy", "StorageUtil", FileListCopy<BSFixedString>, registry));
	registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, VMArray<TESForm*>>("FileFormListCopy", "StorageUtil", FileListCopy<TESForm*>, registry));
	registry->SetFunctionFlags("StorageUtil", "FileIntListCopy", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFloatListCopy", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileStringListCopy", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("StorageUtil", "FileFormListCopy", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("debug_SaveFile", "StorageUtil", SaveExternalFile, registry));
	registry->SetFunctionFlags("StorageUtil", "debug_SaveFile", VMClassRegistry::kFunctionFlag_NoWait);
#endif
}