#include "PapyrusUtil.h"

#include <sstream>
#include <vector>

//#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
/*#include <boost/algorithm/string/compare.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>*/

//#include "skse64/GameAPI.h"
//#include "skse64/GameTypes.h"
//#include "skse64/GameForms.h"

#include "Forms.h"

namespace PapyrusUtil {

	template <typename T>
	VMResultArray<T> CreateArray(StaticFunctionTag*, UInt32 size, T filler) {
		VMResultArray<T> arr;
		arr.resize(size, filler);
		return arr;
	}

	template <typename T>
	VMResultArray<T> ResizeArray(StaticFunctionTag*, VMArray<T> arr, UInt32 size, T filler) {
		VMResultArray<T> Output;
		try{
			Output.resize(size, filler);
			UInt32 length = min(arr.Length(), size);
			for (UInt32 idx = 0; idx < length; ++idx) {
				T value;
				arr.Get(&value, idx);
				Output[idx] = value;
			}
		}
		catch (std::exception &e){
			_MESSAGE("Error Exception: %s", e.what());
		}
		return Output;
	}

	template <> VMResultArray<BSFixedString> ResizeArray(StaticFunctionTag*, VMArray<BSFixedString> arr, UInt32 size, BSFixedString filler) {
		VMResultArray<BSFixedString> Output;
		try {
			Output.resize(size, filler);
			UInt32 length = min(arr.Length(), size);
			for (UInt32 idx = 0; idx < length; ++idx) {
				BSFixedString value;
				arr.Get(&value, idx);
				if (value.data) Output[idx] = value;
				else value = BSFixedString("");
			}
		}
		catch (std::exception &e) {
			_MESSAGE("Error Exception: %s", e.what());
		}
		return Output;
	}


	/*template <> VMResultArray<bool>ResizeArray<bool>(StaticFunctionTag*, VMArray<bool> arr, UInt32 size, bool filler) {
		VMResultArray<bool> Output;
		VMResultArray<bool>::iterator itr = Output.begin();
		for (UInt32 idx = 0; itr != Output.end() && idx < arr.Length(); ++itr, ++idx){
			bool var = false;
			arr.Get(&var, idx);
			if (var) Output.push_back(true);
			else Output.push_back(false);
		}
		if (size > Output.size()) Output.resize(size, filler);
		return Output;
	}*/

	template <typename T>
	VMResultArray<T> PushArray(StaticFunctionTag*, VMArray<T> arr, T push) {
		return ResizeArray(NULL, arr, (arr.Length() + 1), push);
	}


	template<typename T>
	UInt32 CountValues(StaticFunctionTag*, VMArray<T> arr, T find){
		UInt32 count(0), length(arr.Length());
		if (length > 0){
			for (UInt32 idx = 0; idx < length; ++idx){
				T var;
				arr.Get(&var, idx);
				if (var == find) count += 1;
			}
		}
		return count;
	}

	template <> UInt32 CountValues(StaticFunctionTag*, VMArray<BSFixedString> arr, BSFixedString find) {
		UInt32 count(0), length(arr.Length());
		if (length > 0) {
			const char* str = find.data;
			for (UInt32 idx = 0; idx < length; ++idx) {
				BSFixedString var;
				arr.Get(&var, idx);
				if (!var.data) var = BSFixedString("");
				if (boost::iequals(var.data, str)) count += 1;
			}
		}
		return count;
	}

	template <typename T>
	VMResultArray<T> ClearArray(StaticFunctionTag*, VMArray<T> arr, T remove){
		VMResultArray<T> Output;
		if (arr.Length() > 0) {
			UInt32 length(arr.Length()), count(CountValues(NULL, arr, remove));
			if (count != length && (length - count) > 0) {
				Output.reserve((length - count));
				for (UInt32 idx = 0; idx < length; ++idx) {
					T var;
					arr.Get(&var, idx);
					if (var != remove)
						Output.push_back(var);
				}
			}
		}
		return Output;
	}

	//FIXME: returns none instead of empty array when removing all elements
	template <> VMResultArray<BSFixedString> ClearArray(StaticFunctionTag*, VMArray<BSFixedString> arr, BSFixedString remove) {
		VMResultArray<BSFixedString> Output;
		if (arr.Length() > 0) {
			UInt32 length(arr.Length()), count(CountValues(NULL, arr, remove));
			if (count != length && (length - count) > 0) {
				Output.reserve((length - count));
				const char* str = remove.data;
				int size = Output.size();
				for (UInt32 idx = 0; idx < length; ++idx) {
					BSFixedString var;
					arr.Get(&var, idx);
					if (!var.data) var = BSFixedString("");
					if (!boost::iequals(var.data, str))
						Output.push_back(var);
				}
			}
		}
		return Output;
	}
	

	/*template <typename T>
	VMResultArray<T> SortArray(StaticFunctionTag*, VMArray<T> arr, bool descending){
		if (arr.Length() < 1)	return;

		VMResultArray<T> Output;
		try{
			Output.resize(arr.Length());
			for (UInt32 idx = 0; idx < arr.Length(); ++idx) {
				T value;
				arr.Get(&value, idx);
				Output[idx] = value;
			}
			if (descending) std::sort(Output.rbegin(), Output.rend());
			else std::sort(Output.begin(), Output.end());
		}
		catch (std::exception &e){
			_MESSAGE("Error Exception: %s", e.what());
		}
		return Output;
	}

	bool compare(BSFixedString& c1, BSFixedString& c2){
		return strcmp(c1.data, c2.data) < 0;
	}
	template <> VMResultArray<BSFixedString> SortArray(StaticFunctionTag*, VMArray<BSFixedString> arr, bool descending) {
		if (arr.Length() < 1)	return;

		VMResultArray<BSFixedString> Output;
		//std::vector<std::string> vec;
		try{
			Output.resize(arr.Length());
			//vec.resize(arr.Length());
			for (UInt32 idx = 0; idx < arr.Length(); ++idx) {
				BSFixedString var;
				arr.Get(&var, idx);
				if (!var.data) var = BSFixedString("");
				Output[idx] = var.data;
			}
			if (descending) std::sort(Output.rbegin(), Output.rend(), compare);
			else std::sort(Output.begin(), Output.end(), compare);

		}
		catch (std::exception &e){
			_MESSAGE("Error Exception: %s", e.what());
		}
		return Output;
	}*/


	template <typename T>
	VMResultArray<T> MergeArray(StaticFunctionTag*, VMArray<T> arr1, VMArray<T> arr2, bool removeDupe) {
		VMResultArray<T> Output;
		UInt32 length1(arr1.Length()), length2(arr2.Length());
		Output.reserve((length1 + length2));
		for (UInt32 idx = 0; idx < length1; ++idx){
			T var;
			arr1.Get(&var, idx);
			if (!removeDupe || std::find(Output.begin(), Output.end(), var) == Output.end())
				Output.push_back(var);
		}
		for (UInt32 idx = 0; idx < length2; ++idx){
			T var;
			arr2.Get(&var, idx);
			if (!removeDupe || std::find(Output.begin(), Output.end(), var) == Output.end())
				Output.push_back(var);
		}
		if(removeDupe) Output.shrink_to_fit();
		return Output;
	}

	template <typename T>
	VMResultArray<T> RemoveDupe(StaticFunctionTag*, VMArray<T> arr1) {
		VMResultArray<T> Output;
		UInt32 length1(arr1.Length());
		Output.reserve(length1);
		for (UInt32 idx = 0; idx < length1; ++idx) {
			T var;
			arr1.Get(&var, idx);
			if (std::find(Output.begin(), Output.end(), var) == Output.end()) {
				Output.push_back(var);
				//length2 += 1;
			}
		}
		Output.shrink_to_fit();
		return Output;
	}

	template <>	VMResultArray<BSFixedString> RemoveDupe(StaticFunctionTag*, VMArray<BSFixedString> arr1) {
		VMResultArray<BSFixedString> Output;
		UInt32 length1(arr1.Length());
		Output.reserve(length1);
		for (UInt32 idx = 0; idx < length1; ++idx) {
			BSFixedString var;
			arr1.Get(&var, idx);
			if (!var.data) var = BSFixedString("");
			if (std::find(Output.begin(), Output.end(), var) == Output.end()) {
				Output.push_back(var);
				//length2 += 1;
			}
		}
		Output.shrink_to_fit();
		return Output;
	}


	template<typename T>
	bool HasValue(VMArray<T> arr, T find) {
		UInt32 length(arr.Length());
		if (length > 0) {
			for (UInt32 idx = 0; idx < length; ++idx) {
				T var;
				arr.Get(&var, idx);
				if (var == find) return true;
			}
		}
		return false;
	}

	template <>	bool HasValue(VMArray<BSFixedString> arr, BSFixedString find) {
		UInt32 length(arr.Length());
		if (length > 0) {
			const char* str = find.data;
			for (UInt32 idx = 0; idx < length; ++idx) {
				BSFixedString var;
				arr.Get(&var, idx);
				if (!var.data) var = BSFixedString("");
				if (boost::iequals(var.data, str)) return true;
			}
		}
		return false;
	}

	template <typename T>
	VMResultArray<T> GetDiff(StaticFunctionTag*, VMArray<T> arr1, VMArray<T> arr2, bool checkBoth, bool allowDupe) {
		VMResultArray<T> Output;
		UInt32 length1(arr1.Length()), length2(arr2.Length());
		Output.reserve((length1 + length2));

		for (UInt32 idx = 0; idx < length1; ++idx) {
			T var;
			arr1.Get(&var, idx);
			if ( (!HasValue<T>(arr2, var)) && (allowDupe || std::find(Output.begin(), Output.end(), var) == Output.end()) ) {
				Output.push_back(var);
			}
		}

		if (checkBoth) {
			for (UInt32 idx = 0; idx < length2; ++idx) {
				T var;
				arr2.Get(&var, idx);
				if ((!HasValue<T>(arr1, var)) && (allowDupe || std::find(Output.begin(), Output.end(), var) == Output.end())) {
					Output.push_back(var);
				}
			}
		}
		
		Output.shrink_to_fit();
		return Output;
	}

	template <>	VMResultArray<BSFixedString> GetDiff(StaticFunctionTag*, VMArray<BSFixedString> arr1, VMArray<BSFixedString> arr2, bool checkBoth, bool allowDupe) {
		VMResultArray<BSFixedString> Output;
		UInt32 length1(arr1.Length()), length2(arr2.Length());
		Output.reserve((length1 + length2));

		for (UInt32 idx = 0; idx < length1; ++idx) {
			BSFixedString var;
			arr1.Get(&var, idx);
			if (!var.data) var = BSFixedString("");
			if ((!HasValue<BSFixedString>(arr2, var)) && (allowDupe || std::find(Output.begin(), Output.end(), var) == Output.end())) {
				Output.push_back(var);
			}
		}

		if (checkBoth) {
			for (UInt32 idx = 0; idx < length2; ++idx) {
				BSFixedString var;
				arr2.Get(&var, idx);
				if (!var.data) var = BSFixedString("");
				if ((!HasValue<BSFixedString>(arr1, var)) && (allowDupe || std::find(Output.begin(), Output.end(), var) == Output.end())) {
					Output.push_back(var);
				}
			}
		}

		Output.shrink_to_fit();
		return Output;
	}

	template <typename T>
	VMResultArray<T> GetMatching(StaticFunctionTag*, VMArray<T> arr1, VMArray<T> arr2) {
		VMResultArray<T> Output;
		UInt32 length1(arr1.Length()), length2(arr2.Length());
		Output.reserve((length1 + length2));

		for (UInt32 idx = 0; idx < length1; ++idx) {
			T var;
			arr1.Get(&var, idx);
			if ((HasValue<T>(arr2, var)) && std::find(Output.begin(), Output.end(), var) == Output.end()) {
				Output.push_back(var);
			}
		}

		Output.shrink_to_fit();
		return Output;
	}

	template <>	VMResultArray<BSFixedString> GetMatching(StaticFunctionTag*, VMArray<BSFixedString> arr1, VMArray<BSFixedString> arr2) {
		VMResultArray<BSFixedString> Output;
		UInt32 length1(arr1.Length()), length2(arr2.Length());
		Output.reserve((length1 + length2));

		for (UInt32 idx = 0; idx < length1; ++idx) {
			BSFixedString var;
			arr1.Get(&var, idx);
			if (!var.data) var = BSFixedString("");
			if ((HasValue<BSFixedString>(arr2, var)) && std::find(Output.begin(), Output.end(), var) == Output.end()) {
				Output.push_back(var);
			}
		}

		Output.shrink_to_fit();
		return Output;
	}


	template <typename T>
	VMResultArray<T> SliceArray(StaticFunctionTag*, VMArray<T> Input, UInt32 idx, SInt32 end_idx){
		VMResultArray<T> Output;
		if (end_idx < 0 || end_idx >= Input.Length())
			end_idx = Input.Length() - 1;
		if (!Input.arr || idx >= Input.Length())
			return Output;

		Output.reserve(((end_idx - idx) + 1));
		for (; idx <= end_idx; ++idx){
			T var;
			Input.Get(&var, idx);
			Output.push_back(var);
		}

		return Output;
	}

	template<typename T>
	void SortArray(StaticFunctionTag*, VMArray<T> Input, bool descending) {
		if (Input.Length() < 1)	return;

		UInt32 length = Input.Length();
		std::vector<T> arr;
		arr.resize(length);
		for (UInt32 idx = 0; idx < length; ++idx) {
			T value;
			Input.Get(&value, idx);
			arr[idx] = value;
		}

		std::sort(arr.begin(), arr.end());
		if (descending) std::reverse(arr.begin(), arr.end());

		for (UInt32 idx = 0; idx < length; ++idx) {
			Input.Set(&arr[idx], idx);
		}

	}

	template <> void SortArray(StaticFunctionTag*, VMArray<BSFixedString> Input, bool descending) {
		if (Input.Length() < 1)	return;

		UInt32 length = Input.Length();
		std::vector<std::string> arr;
		arr.resize(length);
		for (UInt32 idx = 0; idx < length; ++idx) {
			BSFixedString value;
			Input.Get(&value, idx);
			if (value.data && value.data[0] != '\0') arr[idx] = value.data;
			else arr[idx] = "";
			
		}

		std::sort(arr.begin(), arr.end());
		if (descending) std::reverse(arr.begin(), arr.end());

		for (UInt32 idx = 0; idx < length; ++idx) {
			BSFixedString value = BSFixedString(arr[idx].c_str());
			Input.Set(&value, idx);
		}
		
	}

	template<typename T>
	T AddValues(StaticFunctionTag*, VMArray<T> Values){
		T out = 0;
		UInt32 length(Values.Length());
		for (int idx = 0; idx < length; ++idx){
			T var;
			Values.Get(&var, idx);
			out += var;
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

	//trim whitespace
	/*static inline std::string &trim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}*/

	VMResultArray<BSFixedString> StringSplit(StaticFunctionTag*, BSFixedString argstring, BSFixedString delimiter){
		VMResultArray<BSFixedString> Output;
		if (!argstring.data || !delimiter.data || boost::iequals(argstring.data, delimiter.data))
			return Output;
		// Get non BS string vector
		std::vector<std::string> args;
		std::string delim = delimiter.data;
		std::string range = argstring.data;
		boost::ireplace_all(range, delim, delim);
		boost::iter_split(args, range, boost::first_finder(delim));
		// Init to size
		Output.reserve(args.size());
		// Fill BSFixedString array
		for (std::vector<std::string>::iterator itr = args.begin(); itr != args.end(); ++itr){
			boost::trim((*itr));
			BSFixedString str((*itr).c_str());
			Output.push_back(str);
		}
		return Output;
	}

	BSFixedString StringJoin(StaticFunctionTag*, VMArray<BSFixedString> args, BSFixedString delimiter){
		std::stringstream ss;
		UInt32 length(args.Length());
		const char* delim = delimiter.data;
		for (UInt32 idx = 0; idx < length;){
			BSFixedString arg;
			args.Get(&arg, idx);
			if (!arg.data) arg = BSFixedString("");
			ss << arg.data;
			++idx;
			if (idx < length)
				ss << delim;
		}
		const std::string &tmp = ss.str();
		const char* str = tmp.c_str();
		return BSFixedString(str);
	}

	bool TestForm(StaticFunctionTag*, TESForm* ref) {
		if (!ref) { return false; }
		UInt64 fkey = Forms::GetFormKey(ref);
		_MESSAGE("TestForm(0x%X) / %lu", ref->formID, ref->formID);
		_MESSAGE("\tFormKey: (0x%X) / %llu", fkey, fkey);


		UInt64 key = ref->formID;
		UInt64 testi = ((UInt64)ref->formType) << 32;
		_MESSAGE("\tFormType: %d", (int)ref->formType);
		_MESSAGE("\tShift: (0x%X) / %llu", (int)testi, testi);
		key |= testi;
		_MESSAGE("\tShifted: (0x%X) / %llu", (int)key, key);


		UInt32 type = (UInt32)(key >> 32);
		UInt32 id = (UInt32)(key);
		_MESSAGE("\tShifted Type: %lu", type);
		_MESSAGE("\tShifted ID: (0x%X) / %lu", (int)id, id);


		UInt32 baseID = id & 0x00FFFFFF;
		_MESSAGE("\tbaseID: 0x%X", (int)baseID);

		UInt8 oldMod = (UInt8)(id >> 24);

		_MESSAGE("\toldMod: 0x%X / %d", (int)oldMod, (int)oldMod);



		//UInt32 id2 = Forms::ResolveFormID((UInt32)(key));
		//_MESSAGE("\tResolved: 0x%X / %lu", (int)id2, id2);

		return true;
	}

}



#include "skse64/PapyrusNativeFunctions.h"

void PapyrusUtil::RegisterFuncs(VMClassRegistry* registry) {

	//registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESForm*>("TestForm", "PapyrusUtil", TestForm, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "TestForm", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, VMArray<SInt32>, bool>("SortIntArray", "PapyrusUtil", SortArray<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SortIntArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, VMArray<float>, bool>("SortFloatArray", "PapyrusUtil", SortArray<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SortFloatArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, VMArray<BSFixedString>, bool>("SortStringArray", "PapyrusUtil", SortArray<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SortStringArray", VMClassRegistry::kFunctionFlag_NoWait);

	/*registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, VMArray<TESForm*>, UInt32, TESForm*>("_SetFormValue", "PapyrusUtil", SetArrayValue<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "_SetFormValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, VMArray<BGSBaseAlias*>, UInt32, BGSBaseAlias*>("_SetAliasValue", "PapyrusUtil", SetArrayValue<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "_SetAliasValue", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, VMArray<SInt32>, UInt32, SInt32>("_SetIntValue", "PapyrusUtil", SetArrayValue<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "_SetIntValue", VMClassRegistry::kFunctionFlag_NoWait);*/

	// CreateArray
	/*registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<float>, UInt32, float>("FloatArray", "PapyrusUtil", CreateArray<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "FloatArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<SInt32>, UInt32, SInt32>("IntArray", "PapyrusUtil", CreateArray<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "IntArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<bool>, UInt32, bool>("BoolArray", "PapyrusUtil", CreateArray<bool>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "BoolArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BSFixedString>, UInt32, BSFixedString>("StringArray", "PapyrusUtil", CreateArray<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "StringArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<TESForm*>, UInt32, TESForm*>("FormArray", "PapyrusUtil", CreateArray<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "FormArray", VMClassRegistry::kFunctionFlag_NoWait);

	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BGSBaseAlias*>, UInt32, BGSBaseAlias*>("AliasArray", "PapyrusUtil", CreateArray<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "AliasArray", VMClassRegistry::kFunctionFlag_NoWait);*/
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<Actor*>, UInt32, Actor*>("ActorArray", "PapyrusUtil", CreateArray<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ActorArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<TESObjectREFR*>, UInt32, TESObjectREFR*>("ObjRefArray", "PapyrusUtil", CreateArray<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ObjRefArray", VMClassRegistry::kFunctionFlag_NoWait);

	// ResizeArray
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<float>, VMArray<float>, UInt32, float>("ResizeFloatArray", "PapyrusUtil", ResizeArray<float>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "ResizeFloatArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<SInt32>, VMArray<SInt32>, UInt32, SInt32>("ResizeIntArray", "PapyrusUtil", ResizeArray<SInt32>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "ResizeIntArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<bool>, VMArray<bool>, UInt32, bool>("ResizeBoolArray", "PapyrusUtil", ResizeArray<bool>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "ResizeBoolArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<BSFixedString>, VMArray<BSFixedString>, UInt32, BSFixedString>("ResizeStringArray", "PapyrusUtil", ResizeArray<BSFixedString>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "ResizeStringArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<TESForm*>, VMArray<TESForm*>, UInt32, TESForm*>("ResizeFormArray", "PapyrusUtil", ResizeArray<TESForm*>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "ResizeFormArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>, UInt32, BGSBaseAlias*>("ResizeAliasArray", "PapyrusUtil", ResizeArray<BGSBaseAlias*>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "ResizeAliasArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<Actor*>, VMArray<Actor*>, UInt32, Actor*>("ResizeActorArray", "PapyrusUtil", ResizeArray<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ResizeActorArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, UInt32, TESObjectREFR*>("ResizeObjRefArray", "PapyrusUtil", ResizeArray<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ResizeObjRefArray", VMClassRegistry::kFunctionFlag_NoWait);

	// PushArray
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<float>, VMArray<float>, float>("PushFloat", "PapyrusUtil", PushArray<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "PushFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<SInt32>, VMArray<SInt32>, SInt32>("PushInt", "PapyrusUtil", PushArray<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "PushInt", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<bool>, VMArray<bool>, bool>("PushBool", "PapyrusUtil", PushArray<bool>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "PushBool", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BSFixedString>, VMArray<BSFixedString>, BSFixedString>("PushString", "PapyrusUtil", PushArray<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "PushString", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<TESForm*>, VMArray<TESForm*>, TESForm*>("PushForm", "PapyrusUtil", PushArray<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "PushForm", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>, BGSBaseAlias*>("PushAlias", "PapyrusUtil", PushArray<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "PushAlias", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<Actor*>, VMArray<Actor*>, Actor*>("PushActor", "PapyrusUtil", PushArray<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "PushActor", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, TESObjectREFR*>("PushObjRef", "PapyrusUtil", PushArray<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "PushObjRef", VMClassRegistry::kFunctionFlag_NoWait);

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
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<BGSBaseAlias*>, BGSBaseAlias*>("CountAlias", "PapyrusUtil", CountValues<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountAlias", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<Actor*>, Actor*>("CountActor", "PapyrusUtil", CountValues<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountActor", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, VMArray<TESObjectREFR*>, TESObjectREFR*>("CountObjRef", "PapyrusUtil", CountValues<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "CountObjRef", VMClassRegistry::kFunctionFlag_NoWait);

	// ClearArray
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<float>, VMArray<float>, float>("RemoveFloat", "PapyrusUtil", ClearArray<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<SInt32>, VMArray<SInt32>, SInt32>("RemoveInt", "PapyrusUtil", ClearArray<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveInt", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<bool>, VMArray<bool>, bool>("RemoveBool", "PapyrusUtil", ClearArray<bool>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "RemoveBool", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BSFixedString>, VMArray<BSFixedString>, BSFixedString>("RemoveString", "PapyrusUtil", ClearArray<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveString", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<TESForm*>, VMArray<TESForm*>, TESForm*>("RemoveForm", "PapyrusUtil", ClearArray<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveForm", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>, BGSBaseAlias*>("RemoveAlias", "PapyrusUtil", ClearArray<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveAlias", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<Actor*>, VMArray<Actor*>, Actor*>("RemoveActor", "PapyrusUtil", ClearArray<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveActor", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, TESObjectREFR*>("RemoveObjRef", "PapyrusUtil", ClearArray<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveObjRef", VMClassRegistry::kFunctionFlag_NoWait);

	// RemoveDupe
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, VMResultArray<float>, VMArray<float>>("RemoveDupeFloat", "PapyrusUtil", RemoveDupe<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveDupeFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, VMResultArray<SInt32>, VMArray<SInt32>>("RemoveDupeInt", "PapyrusUtil", RemoveDupe<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveDupeInt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, VMResultArray<BSFixedString>, VMArray<BSFixedString>>("RemoveDupeString", "PapyrusUtil", RemoveDupe<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveDupeString", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, VMResultArray<TESForm*>, VMArray<TESForm*>>("RemoveDupeForm", "PapyrusUtil", RemoveDupe<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveDupeForm", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, VMResultArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>>("RemoveDupeAlias", "PapyrusUtil", RemoveDupe<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveDupeAlias", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, VMResultArray<Actor*>, VMArray<Actor*>>("RemoveDupeActor", "PapyrusUtil", RemoveDupe<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveDupeActor", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, VMResultArray<TESObjectREFR*>, VMArray<TESObjectREFR*>>("RemoveDupeObjRef", "PapyrusUtil", RemoveDupe<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "RemoveDupeObjRef", VMClassRegistry::kFunctionFlag_NoWait);

	// GetDiff
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, VMResultArray<float>, VMArray<float>, VMArray<float>, bool, bool>("GetDiffFloat", "PapyrusUtil", GetDiff<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetDiffFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, VMResultArray<SInt32>, VMArray<SInt32>, VMArray<SInt32>, bool, bool>("GetDiffInt", "PapyrusUtil", GetDiff<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetDiffInt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, VMResultArray<BSFixedString>, VMArray<BSFixedString>, VMArray<BSFixedString>, bool, bool>("GetDiffString", "PapyrusUtil", GetDiff<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetDiffString", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, VMResultArray<TESForm*>, VMArray<TESForm*>, VMArray<TESForm*>, bool, bool>("GetDiffForm", "PapyrusUtil", GetDiff<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetDiffForm", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, VMResultArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>, bool, bool>("GetDiffAlias", "PapyrusUtil", GetDiff<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetDiffAlias", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, VMResultArray<Actor*>, VMArray<Actor*>, VMArray<Actor*>, bool, bool>("GetDiffActor", "PapyrusUtil", GetDiff<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetDiffActor", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction4<StaticFunctionTag, VMResultArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, bool, bool>("GetDiffObjRef", "PapyrusUtil", GetDiff<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetDiffObjRef", VMClassRegistry::kFunctionFlag_NoWait);

	// GetMatching
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<float>, VMArray<float>, VMArray<float>>("GetMatchingFloat", "PapyrusUtil", GetMatching<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetMatchingFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<SInt32>, VMArray<SInt32>, VMArray<SInt32>>("GetMatchingInt", "PapyrusUtil", GetMatching<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetMatchingInt", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BSFixedString>, VMArray<BSFixedString>, VMArray<BSFixedString>>("GetMatchingString", "PapyrusUtil", GetMatching<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetMatchingString", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<TESForm*>, VMArray<TESForm*>, VMArray<TESForm*>>("GetMatchingForm", "PapyrusUtil", GetMatching<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetMatchingForm", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>>("GetMatchingAlias", "PapyrusUtil", GetMatching<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetMatchingAlias", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<Actor*>, VMArray<Actor*>, VMArray<Actor*>>("GetMatchingActor", "PapyrusUtil", GetMatching<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetMatchingActor", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, VMArray<TESObjectREFR*>>("GetMatchingObjRef", "PapyrusUtil", GetMatching<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "GetMatchingObjRef", VMClassRegistry::kFunctionFlag_NoWait);

	// MergeArray
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<float>, VMArray<float>, VMArray<float>, bool>("MergeFloatArray", "PapyrusUtil", MergeArray<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "MergeFloatArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<SInt32>, VMArray<SInt32>, VMArray<SInt32>, bool>("MergeIntArray", "PapyrusUtil", MergeArray<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "MergeIntArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<bool>, VMArray<bool>, VMArray<bool>, bool>("MergeBoolArray", "PapyrusUtil", MergeArray<bool>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "MergeBoolArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<BSFixedString>, VMArray<BSFixedString>, VMArray<BSFixedString>, bool>("MergeStringArray", "PapyrusUtil", MergeArray<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "MergeStringArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<TESForm*>, VMArray<TESForm*>, VMArray<TESForm*>, bool>("MergeFormArray", "PapyrusUtil", MergeArray<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "MergeFormArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>, bool>("MergeAliasArray", "PapyrusUtil", MergeArray<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "MergeAliasArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<Actor*>, VMArray<Actor*>, VMArray<Actor*>, bool>("MergeActorArray", "PapyrusUtil", MergeArray<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "MergeActorArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, bool>("MergeObjRefArray", "PapyrusUtil", MergeArray<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "MergeObjRefArray", VMClassRegistry::kFunctionFlag_NoWait);


	// SliceArray
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<float>, VMArray<float>, UInt32, SInt32>("SliceFloatArray", "PapyrusUtil", SliceArray<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SliceFloatArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<SInt32>, VMArray<SInt32>, UInt32, SInt32>("SliceIntArray", "PapyrusUtil", SliceArray<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SliceIntArray", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<bool>, VMArray<bool>, UInt32, SInt32>("SliceBoolArray", "PapyrusUtil", SliceArray<bool>, registry));
	//registry->SetFunctionFlags("PapyrusUtil", "SliceBoolArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<BSFixedString>, VMArray<BSFixedString>, UInt32, SInt32>("SliceStringArray", "PapyrusUtil", SliceArray<BSFixedString>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SliceStringArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<TESForm*>, VMArray<TESForm*>, UInt32, SInt32>("SliceFormArray", "PapyrusUtil", SliceArray<TESForm*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SliceFormArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<BGSBaseAlias*>, VMArray<BGSBaseAlias*>, UInt32, SInt32>("SliceAliasArray", "PapyrusUtil", SliceArray<BGSBaseAlias*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SliceAliasArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<Actor*>, VMArray<Actor*>, UInt32, SInt32>("SliceActorArray", "PapyrusUtil", SliceArray<Actor*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SliceActorArray", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMResultArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, UInt32, SInt32>("SliceObjRefArray", "PapyrusUtil", SliceArray<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SliceObjRefArray", VMClassRegistry::kFunctionFlag_NoWait);


	// Argstrings
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMResultArray<BSFixedString>, BSFixedString, BSFixedString>("StringSplit", "PapyrusUtil", StringSplit, registry));
	registry->SetFunctionFlags("PapyrusUtil", "StringSplit", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, BSFixedString, VMArray<BSFixedString>, BSFixedString>("StringJoin", "PapyrusUtil", StringJoin, registry));
	registry->SetFunctionFlags("PapyrusUtil", "StringJoin", VMClassRegistry::kFunctionFlag_NoWait);

	// PRE SKSE Array
	/*registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("ArgStringCount", "PapyrusUtil", ArgStringCount, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ArgStringCount", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, UInt32, BSFixedString, BSFixedString>("ArgStringNth", "PapyrusUtil", ArgStringNth, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ArgStringNth", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, VMArray<BSFixedString>, BSFixedString, BSFixedString>("ArgStringLoad", "PapyrusUtil", ArgStringLoad, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ArgStringLoad", VMClassRegistry::kFunctionFlag_NoWait);*/



	// Array Copy To
	/*registry->RegisterFunction(new NativeFunction5<StaticFunctionTag, void, VMArray<bool>, VMArray<bool>, SInt32, SInt32, bool>("BoolCopyTo", "PapyrusUtil", ArrayCopyTo<bool>, registry));
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
	registry->RegisterFunction(new NativeFunction5<StaticFunctionTag, void, VMArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, SInt32, SInt32, bool>("ObjectRefCopyTo", "PapyrusUtil", ArrayCopyTo<TESObjectREFR*>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "ObjectRefCopyTo", VMClassRegistry::kFunctionFlag_NoWait);*/


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

	// Overkill
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, float, bool, float>("SignFloat", "PapyrusUtil", SignValue<float>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SignFloat", VMClassRegistry::kFunctionFlag_NoWait);
	registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, bool, SInt32>("SignInt", "PapyrusUtil", SignValue<SInt32>, registry));
	registry->SetFunctionFlags("PapyrusUtil", "SignInt", VMClassRegistry::kFunctionFlag_NoWait);

}
