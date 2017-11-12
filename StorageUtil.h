#pragma once

#include "skse64/GameData.h"
#include "skse64/PapyrusArgs.h"

struct StaticFunctionTag;

namespace StorageUtil {
	void RegisterFuncs(VMClassRegistry* registry);

	// Flat key=>value storage
	template <typename T, typename S> T SetValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value);
	template <typename T, typename S> T GetValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T missing);
	template <typename T, typename S> T AdjustValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value);
	template <typename T, typename S> bool UnsetValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key);
	template <typename T, typename S> bool HasValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key);

	// Lists key=>vector=>value storage
	template <typename T, typename S> SInt32 ListAdd(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value, bool allowDuplicate);
	template <typename T, typename S> T ListGet(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index);
	template <typename T, typename S> T ListSet(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index, T value);
	template <typename T, typename S> T ListAdjust(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index, T value);
	template <typename T, typename S> UInt32 ListRemove(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value, bool allInstances);
	template <typename T, typename S> bool ListInsertAt(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index, T value);
	template <typename T, typename S> bool ListRemoveAt(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 index);
	template <typename T, typename S> UInt32 ListClear(StaticFunctionTag* base, TESForm* obj, BSFixedString key);
	template <typename T, typename S> UInt32 ListCount(StaticFunctionTag* base, TESForm* obj, BSFixedString key);
	template <typename T, typename S> UInt32 ListCountValue(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value, bool exclude);
	template <typename T, typename S> SInt32 ListFind(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value);
	template <typename T, typename S> bool ListHas(StaticFunctionTag* base, TESForm* obj, BSFixedString key, T value);
	template <typename T, typename S> void ListSort(StaticFunctionTag* base, TESForm* obj, BSFixedString key);
	template <typename T, typename S> void ListSlice(StaticFunctionTag* base, TESForm* obj, BSFixedString key, VMArray<T> Output, UInt32 startIndex);
	template <typename T, typename S> SInt32 ListResize(StaticFunctionTag* base, TESForm* obj, BSFixedString key, UInt32 length, T filler);
	template <typename T, typename S> bool ListCopy(StaticFunctionTag* base, TESForm* obj, BSFixedString key, VMArray<T> Input);
	template <typename T, typename S> VMResultArray<T> ToArray(StaticFunctionTag* base, TESForm* obj, BSFixedString key);

	// Debug functions
	UInt32 Cleanup(StaticFunctionTag* base);
	void DeleteAllValues(StaticFunctionTag* base);
	void DeleteValues(StaticFunctionTag* base, TESForm* obj);

	template <typename T, typename S> UInt32 GetObjectCount(StaticFunctionTag* base);
	template <typename T, typename S> UInt32 GetListsObjectCount(StaticFunctionTag* base);
	template <typename T, typename S> UInt32 GetKeyCount(StaticFunctionTag* base, TESForm* obj);
	template <typename T, typename S> UInt32 GetListsKeyCount(StaticFunctionTag* base, TESForm* obj);
	template <typename T, typename S> TESForm* GetNthObj(StaticFunctionTag* base, UInt32 i);
	template <typename T, typename S> TESForm* GetListsNthObj(StaticFunctionTag* base, UInt32 i);
	template <typename T, typename S> BSFixedString GetNthKey(StaticFunctionTag* base, TESForm* obj, UInt32 i);
	template <typename T, typename S> BSFixedString GetListsNthKey(StaticFunctionTag* base, TESForm* obj, UInt32 i);

#ifdef _GLOBAL_EXTERNAL
	// Global external StorageUtil.json file
	template <typename T> T FileSetValue(StaticFunctionTag* base, BSFixedString key, T value);
	template <typename T> T FileGetValue(StaticFunctionTag* base, BSFixedString key, T missing);
	template <typename T> T FileAdjustValue(StaticFunctionTag* base, BSFixedString key, T value);
	template <typename T> bool FileUnsetValue(StaticFunctionTag* base, BSFixedString key);
	template <typename T> bool FileHasValue(StaticFunctionTag* base, BSFixedString key);
	template <typename T> SInt32 FileListAdd(StaticFunctionTag* base, BSFixedString key, T value, bool allowDuplicate);
	template <typename T> T FileListGet(StaticFunctionTag* base, BSFixedString key, UInt32 index);
	template <typename T> T FileListSet(StaticFunctionTag* base, BSFixedString key, UInt32 index, T value);
	template <typename T> T FileListAdjust(StaticFunctionTag* base, BSFixedString key, UInt32 index, T adjustBy);
	template <typename T> UInt32 FileListRemove(StaticFunctionTag* base, BSFixedString key, T value, bool allInstances);
	template <typename T> bool FileListRemoveAt(StaticFunctionTag* base, BSFixedString key, UInt32 index);
	template <typename T> bool FileListInsertAt(StaticFunctionTag* base, BSFixedString key, UInt32 index, T value);
	template <typename T> UInt32 FileListClear(StaticFunctionTag* base, BSFixedString key);
	template <typename T> UInt32 FileListCount(StaticFunctionTag* base, BSFixedString key);
	template <typename T> SInt32 FileListFind(StaticFunctionTag* base, BSFixedString key, T value);
	template <typename T> bool FileListHas(StaticFunctionTag* base, BSFixedString key, T value);
	template <typename T> SInt32 FileListResize(StaticFunctionTag* base, BSFixedString key, UInt32 length, T filler);
	template <typename T> void FileListSlice(StaticFunctionTag* base, BSFixedString key, VMArray<T> Output, UInt32 startIndex);
	template <typename T> bool FileListCopy(StaticFunctionTag* base, BSFixedString key, VMArray<T> Input);
	void SaveExternalFile(StaticFunctionTag* base);
#endif


}
