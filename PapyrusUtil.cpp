#include "PapyrusUtil.h"

#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

//#include "skse/GameAPI.h"
//#include "skse/GameTypes.h"
//#include "skse/GameForms.h"

namespace PapyrusUtil {
	
	UInt32 ArgStringCount(StaticFunctionTag*, BSFixedString argstring, BSFixedString delimiter){
		if (!argstring.data || !delimiter.data || boost::iequals(argstring.data, delimiter.data)) return 0;
		std::vector<std::string> args;
		std::string delim = delimiter.data;
		std::string range  = argstring.data;
		boost::to_lower(delim);
		boost::ireplace_all(range, delim, delim);
		boost::iter_split(args, range, boost::first_finder(delim));
		return args.size();
	}

	BSFixedString ArgStringNth(StaticFunctionTag*, UInt32 nth, BSFixedString argstring, BSFixedString delimiter){
		if (!argstring.data || !delimiter.data || boost::iequals(argstring.data, delimiter.data)) return 0;
		std::vector<std::string> args;
		std::string delim = delimiter.data;
		std::string range = argstring.data;
		boost::to_lower(delim);
		boost::ireplace_all(range, delim, delim);
		boost::iter_split(args, range, boost::first_finder(delim));
		if (args.empty() || nth >= args.size()) return BSFixedString("");
		else return BSFixedString(args.at(nth).c_str());
	}

	void ArgStringLoad(StaticFunctionTag*, VMArray<BSFixedString> Output, BSFixedString argstring, BSFixedString delimiter){
		if (!Output.arr || !argstring.data || !delimiter.data || boost::iequals(argstring.data, delimiter.data)) return;
		std::vector<std::string> args;
		std::string delim = delimiter.data;
		std::string range = argstring.data;
		boost::to_lower(delim);
		boost::ireplace_all(range, delim, delim);
		boost::iter_split(args, range, boost::first_finder(delim));
		std::vector<std::string>::iterator itr = args.begin();
		for (UInt32 index = 0; index < Output.Length() && itr != args.end(); ++itr, ++index){
			BSFixedString val = BSFixedString((*itr).c_str());
			Output.Set(&val, index);
		}
	}

	template<typename T>
	T AddValues(StaticFunctionTag*, VMArray<T> Values){
		T out = 0;
		for (int i = 0; i < Values.Length(); ++i){
			T var;
			Values.Get(&var, i);
			out += var;
		}
		return out;
	}

	template<typename T>
	UInt32 CountValues(StaticFunctionTag*, VMArray<T> Values, T find){
		UInt32 out = 0;
		for (int i = 0; i < Values.Length(); ++i){
			T var;
			Values.Get(&var, i);
			if (var == find) out += 1;
		}
		return out;
	}

	template<typename T>
	T ClampValue(StaticFunctionTag*, T var, T min, T max){
		if (var >= max) return max;
		else if (var <= min) return min;
		else return var;
	}

	template<typename T>
	T WrapValue(StaticFunctionTag*, T var, T end, T start){
		if (var > end) return start;
		else if (var < start) return end;
		else return var;
	}

	template<typename T>
	T SignValue(StaticFunctionTag*, bool sign, T var){
		return ((sign && var > 0) || (!sign && var < 0)) ? var * -1 : var;
	}


	template<typename T> inline bool IsSet(T var){ return var != T(); }
	template <> inline bool IsSet<BSFixedString>(BSFixedString var){ return var.data && var.data[0] != 0; }
	template <> inline bool IsSet<TESForm*>(TESForm* var){ return var != NULL; }

	template<typename T>
	void ArrayCopyTo(StaticFunctionTag*, VMArray<T> Append, VMArray<T> Output, SInt32 OutputIndex, SInt32 AppendEnd, bool SkipEmpty){
		if (!Output.arr || !Append.arr)
			return;

		int OutputEnd = Output.Length();
		if (OutputIndex < 0 || OutputIndex >= OutputEnd)
			OutputIndex = 0;

		if (AppendEnd < 0 || AppendEnd >= (Append.Length() - 1))
			AppendEnd = Append.Length();

		//if (end < 0 || (OutputEnd - idx) < end) end = OutputEnd;
		//else if () end += 1

		UInt32 i(0), n(OutputIndex);
		if (SkipEmpty){
			for (; i < AppendEnd && n < OutputEnd; ++i){
				T var;
				Append.Get(&var, i);
				if (IsSet(var)){
					Output.Set(&var, n);
					++n;
				}
			}
		}
		else {
			for (; i < AppendEnd && n < OutputEnd; ++i, ++n){
				T var;
				Append.Get(&var, i);
				Output.Set(&var, n);
			}
		}
	}

}



#include "skse/PapyrusNativeFunctions.h"

void PapyrusUtil::RegisterFuncs(VMClassRegistry* registry) {
	// Argstrings
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ArgStringCount", "PapyrusUtil", ArgStringCount, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ArgStringCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, UInt32, BSFixedString, BSFixedString>("ArgStringNth", "PapyrusUtil", ArgStringNth, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ArgStringNth", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, VMArray<BSFixedString>, BSFixedString, BSFixedString>("ArgStringLoad", "PapyrusUtil", ArgStringLoad, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ArgStringLoad", VMClassRegistry::kFunctionFlag_NoWait);

	// Count value
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<bool>, bool>("CountBool", "PapyrusUtil", CountValues<bool>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountBool", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<float>, float>("CountFloat", "PapyrusUtil", CountValues<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<SInt32>, SInt32>("CountInt", "PapyrusUtil", CountValues<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountInt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<BSFixedString>, BSFixedString>("CountString", "PapyrusUtil", CountValues<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountString", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<TESForm*>, TESForm*>("CountForm", "PapyrusUtil", CountValues<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountForm", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<Actor*>, Actor*>("CountActor", "PapyrusUtil", CountValues<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountActor", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<TESObjectREFR*>, TESObjectREFR*>("CountObjectReference", "PapyrusUtil", CountValues<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountObjectReference", VMClassRegistry::kFunctionFlag_NoWait);

	// Array Copy To
	registry->RegisterFunction(new NativeFunction5<StaticFunctionTag, void, VMArray<bool>, VMArray<bool>, SInt32, SInt32, bool>("BoolCopyTo", "PapyrusUtil", ArrayCopyTo<bool>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "BoolCopyTo", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction5<StaticFunctionTag, void, VMArray<float>, VMArray<float>, SInt32, SInt32, bool>("FloatCopyTo", "PapyrusUtil", ArrayCopyTo<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "FloatCopyTo", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction5<StaticFunctionTag, void, VMArray<SInt32>, VMArray<SInt32>, SInt32, SInt32, bool>("IntCopyTo", "PapyrusUtil", ArrayCopyTo<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "IntCopyTo", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction5<StaticFunctionTag, void, VMArray<BSFixedString>, VMArray<BSFixedString>, SInt32, SInt32, bool>("StringCopyTo", "PapyrusUtil", ArrayCopyTo<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "StringCopyTo", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction5<StaticFunctionTag, void, VMArray<TESForm*>, VMArray<TESForm*>, SInt32, SInt32, bool>("FormCopyTo", "PapyrusUtil", ArrayCopyTo<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "FormCopyTo", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction5<StaticFunctionTag, void, VMArray<Actor*>, VMArray<Actor*>, SInt32, SInt32, bool>("ActorCopyTo", "PapyrusUtil", ArrayCopyTo<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ActorCopyTo", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction5<StaticFunctionTag, void, VMArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, SInt32, SInt32, bool>("ObjectReferenceCopyTo", "PapyrusUtil", ArrayCopyTo<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ObjectReferenceCopyTo", VMClassRegistry::kFunctionFlag_NoWait);


	// Misc float/int 
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, float, VMArray<float>>("AddFloatValues", "PapyrusUtil", AddValues<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "AddFloatValues", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, SInt32, VMArray<SInt32>>("AddIntValues", "PapyrusUtil", AddValues<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "AddIntValues", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, float, float, float>("ClampFloat", "PapyrusUtil", ClampValue<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ClampFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, SInt32, SInt32, SInt32>("ClampInt", "PapyrusUtil", ClampValue<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ClampInt", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, float, float, float>("WrapFloat", "PapyrusUtil", WrapValue<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "WrapFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, SInt32, SInt32, SInt32>("WrapInt", "PapyrusUtil", WrapValue<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "WrapInt", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, float, bool, float>("SignFloat", "PapyrusUtil", SignValue<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SignFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, bool, SInt32>("SignInt", "PapyrusUtil", SignValue<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SignInt", VMClassRegistry::kFunctionFlag_NoWait);

}
