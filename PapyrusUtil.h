#pragma once

//struct StaticFunctionTag;
//class VMClassRegistry;

#include "skse64/GameData.h"
#include "skse64/PapyrusArgs.h"

namespace PapyrusUtil {
	void RegisterFuncs(VMClassRegistry* registry);

	template <typename T> VMResultArray<T> ResizeArray(StaticFunctionTag*, VMArray<T> arr, UInt32 size, T filler);
	template <typename T> VMResultArray<T> PushArray(StaticFunctionTag*, VMArray<T> arr, T push);
	template<typename T> UInt32 CountValues(StaticFunctionTag*, VMArray<T> arr, T find);
	template <typename T> VMResultArray<T> ClearArray(StaticFunctionTag*, VMArray<T> arr, T remove);
	template <typename T> VMResultArray<T> MergeArray(StaticFunctionTag*, VMArray<T> arr1, VMArray<T> arr2, bool removeDupe);
	template <typename T> VMResultArray<T> SliceArray(StaticFunctionTag*, VMArray<T> Input, UInt32 idx, SInt32 end_idx);
	template<typename T> T AddValues(StaticFunctionTag*, VMArray<T> Values);
	template<typename T> T ClampValue(StaticFunctionTag*, T var, T min, T max);
	template<typename T> T WrapValue(StaticFunctionTag*, T var, T end, T start);
	template<typename T> T SignValue(StaticFunctionTag*, bool sign, T var);
	VMResultArray<BSFixedString> StringSplit(StaticFunctionTag*, BSFixedString argstring, BSFixedString delimiter);
	BSFixedString StringJoin(StaticFunctionTag*, VMArray<BSFixedString> args, BSFixedString delimiter);
}
