#pragma once

//struct StaticFunctionTag;
//class VMClassRegistry;

#include "skse/GameData.h"
#include "skse/PapyrusArgs.h"

namespace PapyrusUtil {
	void RegisterFuncs(VMClassRegistry* registry);

	UInt32 ArgStringCount(StaticFunctionTag*, BSFixedString argstring, BSFixedString delimiter);
	BSFixedString ArgStringNth(StaticFunctionTag*, UInt32 nth, BSFixedString argstring, BSFixedString delimiter);
	void ArgStringLoad(StaticFunctionTag*, VMArray<BSFixedString> Output, BSFixedString argstring, BSFixedString delimiter);

	template <typename T> T AddValues(StaticFunctionTag*, VMArray<T> Values);
	template <typename T> UInt32 CountValues(StaticFunctionTag*, VMArray<T> Values, T find);
	template <typename T> void ArrayCopyTo(StaticFunctionTag*, VMArray<T> Source, VMArray<T> Output, UInt32 StartIndex, UInt32 EndIndex, bool AllowEmpty);

	template<typename T> T ClampValue(StaticFunctionTag*, T var, T min, T max);
	template<typename T> T WrapValue(StaticFunctionTag*, T var, T end, T start);
	template<typename T> T SignValue(StaticFunctionTag*, bool sign, T var);

	template<typename T> void ArrayCopyTo(StaticFunctionTag*, VMArray<T> Append, VMArray<T> Output, SInt32 OutputIndex, SInt32 AppendEnd, bool SkipEmpty);
}
