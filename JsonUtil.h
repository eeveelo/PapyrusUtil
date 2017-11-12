#pragma once

#include "skse64/PapyrusArgs.h"
#include "skse64/PapyrusVM.h"

struct StaticFunctionTag;

//MAKE PATH FUNCTIONS CASE INSENSITIVE

namespace JsonUtil {
	void RegisterFuncs(VMClassRegistry* registry);

	bool SaveJson(StaticFunctionTag* base, BSFixedString name, bool styled);
	bool LoadJson(StaticFunctionTag* base, BSFixedString name);
	void ClearAll(StaticFunctionTag* base, BSFixedString name);

	template <typename T> T SetValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value);
	template <typename T> T GetValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T missing);
	template <typename T> T AdjustValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value);
	template <typename T> bool UnsetValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key);
	template <typename T> bool HasValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key);
	template <typename T> SInt32 ListAdd(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value, bool allowDuplicate);
	template <typename T> T ListGet(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index);
	template <typename T> T ListSet(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index, T value);
	template <typename T> T ListAdjust(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index, T adjustBy);
	template <typename T> UInt32 ListRemove(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value, bool allInstances);
	template <typename T> bool ListRemoveAt(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index);
	template <typename T> bool ListInsertAt(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 index, T value);
	template <typename T> UInt32 ListClear(StaticFunctionTag* base, BSFixedString name, BSFixedString key);
	template <typename T> UInt32 ListCount(StaticFunctionTag* base, BSFixedString name, BSFixedString key);
	template <typename T> UInt32 ListCountValue(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value, bool exclude);
	template <typename T> SInt32 ListFind(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value);
	template <typename T> bool ListHas(StaticFunctionTag* base, BSFixedString name, BSFixedString key, T value);
	template <typename T> SInt32 ListResize(StaticFunctionTag* base, BSFixedString name, BSFixedString key, UInt32 length, T filler);
	template <typename T> void ListSlice(StaticFunctionTag* base, BSFixedString name, BSFixedString key, VMArray<T> Output, UInt32 startIndex);
	template <typename T> bool ListCopy(StaticFunctionTag* base, BSFixedString name, BSFixedString key, VMArray<T> Input);
	template <typename T> VMResultArray<T> ToArray(StaticFunctionTag* base, BSFixedString name, BSFixedString key);

}
