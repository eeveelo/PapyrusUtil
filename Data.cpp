#include "Data.h"
#include "External.h"

#include "skse64/GameForms.h"

#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>

#include <boost/algorithm/string.hpp>
#include <string>

namespace Data {

	// Flat obj=>key=>value storage
	intv* intValues = NULL;
	flov* floatValues = NULL;
	strv* stringValues = NULL;
	forv* formValues = NULL;

	// Vector obj=>key=>vector[i] storage
	intl* intLists = NULL;
	flol* floatLists = NULL;
	strl* stringLists = NULL;
	forl* formLists = NULL;

	template <> intv* GetValues<SInt32, SInt32>() { if (!intValues) intValues = new intv(); return intValues; }
	template <> flov* GetValues<float, float>() { if (!floatValues) floatValues = new flov(); return floatValues; }
	template <> strv* GetValues<BSFixedString, std::string>() { if (!stringValues) stringValues = new strv(); return stringValues; }
	template <> forv* GetValues<TESForm*, UInt32>() { if (!formValues) formValues = new forv(); return formValues; }

	template <> intl* GetLists<SInt32, SInt32>() { if (!intLists) intLists = new intl(); return intLists; }
	template <> flol* GetLists<float, float>() { if (!floatLists) floatLists = new flol(); return floatLists; }
	template <> strl* GetLists<BSFixedString, std::string>() { if (!stringLists) stringLists = new strl(); return stringLists; }
	template <> forl* GetLists<TESForm*, UInt32>() { if (!formLists) formLists = new forl(); return formLists; }

	void InitLists() {
		if (intValues == NULL) intValues = new intv();
		if (floatValues == NULL) floatValues = new flov();
		if (stringValues == NULL) stringValues = new strv();
		if (formValues == NULL) formValues = new forv();

		if (intLists == NULL) intLists = new intl();
		if (floatLists == NULL) floatLists = new flol();
		if (stringLists == NULL) stringLists = new strl();
		if (formLists == NULL) formLists = new forl();
	}


	/*
	* Define methods
	*/

	template <typename T, typename S>
	void Values<T, S>::SetValue(UInt64 obj, std::string key, T value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		Data[obj][key] = cast(value);
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	T Values<T, S>::GetValue(UInt64 obj, std::string key, T value) {
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			boost::to_lower(key);
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end())
				value = cast(itr2->second);
		}
		s_dataLock.Leave();
		return value;
	}

	template <typename T, typename S>
	T Values<T, S>::AdjustValue(UInt64 obj, std::string key, T value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end())
				value = (value + cast(itr2->second));
		}
		Data[obj][key] = value;
		s_dataLock.Leave();
		return value;
	}
	template <> BSFixedString Values<BSFixedString, std::string>::AdjustValue(UInt64 obj, std::string key, BSFixedString value) { return value; }
	template <> TESForm* Values<TESForm*, UInt32>::AdjustValue(UInt64 obj, std::string key, TESForm* value) { return value; }

	template <typename T, typename S>
	bool Values<T, S>::UnsetValue(UInt64 obj, std::string key) {
		bool unset = false;
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			boost::to_lower(key);
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end()) {
				unset = true;
				itr->second.erase(itr2);
				if (itr->second.size() == 0)
					Data.erase(itr);
			}
		}
		s_dataLock.Leave();
		return unset;
	}

	template <typename T, typename S>
	T Values<T, S>::PluckValue(UInt64 obj, std::string key, T value) {
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			boost::to_lower(key);
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end()) {
				value = cast(itr2->second);
				itr->second.erase(itr2);
				if (itr->second.size() == 0)
					Data.erase(itr);
			}
		}
		s_dataLock.Leave();
		return value;
	}

	template <typename T, typename S>
	bool Values<T, S>::HasValue(UInt64 obj, std::string key) {
		bool found = false;
		s_dataLock.Enter();

		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			boost::to_lower(key);
			found = itr->second.find(key) != itr->second.end();
		}

		s_dataLock.Leave();
		return found;
	}

	template <typename T, typename S>
	int Lists<T, S>::ListAdd(UInt64 obj, std::string key, T value, bool allowDuplicate) {
		if (!allowDuplicate && ListHas(obj, key, value))
			return -1;
		s_dataLock.Enter();

		boost::to_lower(key);
		int index = Data[obj][key].size();
		Data[obj][key].push_back(cast(value));

		s_dataLock.Leave();
		return index;
	}

	template <typename T, typename S>
	T Lists<T, S>::ListGet(UInt64 obj, std::string key, UInt32 index) {
		S value = S();
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && index >= 0 && index < vector->size())
			value = vector->at(index);

		s_dataLock.Leave();
		return cast(value);
	}


	template <typename T, typename S>
	T Lists<T, S>::ListSet(UInt64 obj, std::string key, UInt32 index, T value) {
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && index >= 0 && index < vector->size())
			vector->at(index) = cast(value);
		else if (index == 0 && (vector == NULL || vector->empty()))
			Data[obj][key].push_back(cast(value));
		else {
			S var = S();
			value = cast(var);
		}

		s_dataLock.Leave();
		return value;
	}

	template <typename T, typename S>
	T Lists<T, S>::ListAdjust(UInt64 obj, std::string key, UInt32 index, T value) {
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && index >= 0 && index < vector->size()) {
			vector->at(index) += cast(value);
			value = cast(vector->at(index));
		}
		else {
			S var = S();
			value = cast(var);
		}

		s_dataLock.Leave();
		return value;
	}
	template <> BSFixedString Lists<BSFixedString, std::string>::ListAdjust(UInt64 obj, std::string key, UInt32 index, BSFixedString value) { return value; }
	template <> TESForm* Lists<TESForm*, UInt32>::ListAdjust(UInt64 obj, std::string key, UInt32 index, TESForm* value) { return value; }

	template <typename T, typename S>
	int Lists<T, S>::ListRemove(UInt64 obj, std::string key, T value, bool allInstances) {
		int removed = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL) {
			// Remove
			int count = vector->size();
			if (allInstances)
				vector->erase(std::remove(vector->begin(), vector->end(), cast(value)), vector->end());
			else {
				List::iterator itr = std::find(vector->begin(), vector->end(), cast(value));
				if (itr != vector->end())
					vector->erase(itr);
			}
			removed = count - vector->size();
			// Cleanup
			if (vector->empty()) {
				Data[obj].erase(key);
				if (Data[obj].size() == 0)
					Data.erase(obj);
			}
		}

		s_dataLock.Leave();
		return removed;
	}

	template <typename T, typename S>
	bool Lists<T, S>::ListInsertAt(UInt64 obj, std::string key, UInt32 index, T value) {
		bool inserted = false;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && !vector->empty() && index < vector->size()) {
			try {
				List::iterator itr = vector->begin();
				std::advance(itr, index);
				vector->insert(itr, cast(value));
				//vector->at(index) = value;
				inserted = true;
			}
			catch (const std::out_of_range&) {
				inserted = false;
			}
		}
		else if (index == 0 && (vector == NULL || vector->empty())) {
			Data[obj][key].push_back(cast(value));
			inserted = true;
		}
		s_dataLock.Leave();
		return inserted;
	}

	template <typename T, typename S>
	bool Lists<T, S>::ListRemoveAt(UInt64 obj, std::string key, UInt32 index) {
		bool removed = false;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && !vector->empty() && index < vector->size()) {
			vector->erase(vector->begin() + index);
			removed = true;
			// Cleanup
			if (vector->empty()) {
				Data[obj].erase(key);
				if (Data[obj].size() == 0)
					Data.erase(obj);
			}
		}
		s_dataLock.Leave();
		return removed;
	}

	template <typename T, typename S>
	T Lists<T, S>::ListPluck(UInt64 obj, std::string key, UInt32 index, T value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && !vector->empty() && index < vector->size()) {
			value = cast(vector->at(index));
			vector->erase(vector->begin() + index);
			// Cleanup
			if (vector->empty()) {
				Data[obj].erase(key);
				if (Data[obj].size() == 0)
					Data.erase(obj);
			}
		}
		s_dataLock.Leave();
		return value;
	}
	template <typename T, typename S>
	T Lists<T, S>::ListShift(UInt64 obj, std::string key) {
		S value = S();
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && !vector->empty()) {
			value = vector->at(0);
			vector->erase(vector->begin());
			// Cleanup
			if (vector->empty()) {
				Data[obj].erase(key);
				if (Data[obj].size() == 0)
					Data.erase(obj);
			}
		}
		s_dataLock.Leave();
		return cast(value);
	}
	template <typename T, typename S>
	T Lists<T, S>::ListPop(UInt64 obj, std::string key) {
		S value = S();
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && !vector->empty()) {
			value = vector->at(vector->size() - 1);
			vector->pop_back();
			// Cleanup
			if (vector->empty()) {
				Data[obj].erase(key);
				if (Data[obj].size() == 0)
					Data.erase(obj);
			}
		}
		s_dataLock.Leave();
		return cast(value);
	}


	template <typename T, typename S>
	int Lists<T, S>::ListClear(UInt64 obj, std::string key) {
		int removed = 0;
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			boost::to_lower(key);
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end()) {
				removed = itr2->second.size();
				itr->second.erase(itr2);
				// Cleanup
				if (itr->second.size() == 0)
					Data.erase(itr);
			}
		}
		s_dataLock.Leave();
		return removed;
	}

	template <typename T, typename S>
	int Lists<T, S>::ListCount(UInt64 obj, std::string key) {
		int count = 0;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL)
			count = vector->size();
		s_dataLock.Leave();
		return count;
	}

	template <typename T, typename S>
	int Lists<T, S>::ListCountValue(UInt64 obj, std::string key, T value, bool exclude) {
		int count = 0;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL) {
			S var = cast(value);
			for (List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
				if (var == *itr)
					count += 1;
			}
			if (exclude) count = (vector->size() - count);
		}
		s_dataLock.Leave();
		return count;
	}

	template <typename T, typename S>
	int Lists<T, S>::ListFind(UInt64 obj, std::string key, T value) {
		int index = -1;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL) {
			List::iterator itr = std::find(vector->begin(), vector->end(), cast(value));
			if (itr != vector->end()) index = itr - vector->begin();
		}
		s_dataLock.Leave();
		return index;
	}

	template <typename T, typename S>
	bool Lists<T, S>::ListHas(UInt64 obj, std::string key, T value) {
		bool found = false;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		found = vector != NULL && std::find(vector->begin(), vector->end(), cast(value)) != vector->end();
		s_dataLock.Leave();
		return found;
	}


	template <typename T, typename S>
	void Lists<T, S>::ListSort(UInt64 obj, std::string key) {
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL)
			std::sort(vector->begin(), vector->end());
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Lists<T, S>::ListSlice(UInt64 obj, std::string key, VMArray<T> Output, UInt32 startIndex) {
		if (Output.Length() > 0) {
			s_dataLock.Enter();
			boost::to_lower(key);
			List* vector = GetVector(obj, key);
			if (vector != NULL && startIndex < vector->size() && startIndex >= 0) {
				List::iterator itr = vector->begin();
				std::advance(itr, startIndex);
				UInt32 length(Output.Length());
				for (UInt32 index = 0; index < length && itr != vector->end(); ++itr, ++index) {
					T val = cast(*itr);
					Output.Set(&val, index);
				}
			}
			s_dataLock.Leave();
		}
	}

	template <typename T, typename S>
	int Lists<T, S>::ListResize(UInt64 obj, std::string key, UInt32 length, T filler) {
		if (length == 0)
			return ListClear(obj, key) * -1;
		s_dataLock.Enter();
		boost::to_lower(key);
		int start = Data[obj][key].size();
		Data[obj][key].resize(length, cast(filler));
		s_dataLock.Leave();
		return Data[obj][key].size() - start;
	}

	template <typename T, typename S>
	bool Lists<T, S>::ListCopy(UInt64 obj, std::string key, VMArray<T> Input) {
		UInt32 length(Input.Length());
		if (length < 1)
			return false;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL)
			vector->clear();
		Data[obj][key].reserve(length);
		for (UInt32 i = 0; i < length; ++i) {
			T var;
			Input.Get(&var, i);
			S value = cast(var);
			Data[obj][key].push_back(value);
		}
		s_dataLock.Leave();
		return true;
	}

	template <typename T, typename S>
	VMResultArray<T> Lists<T, S>::ToArray(UInt64 obj, std::string key) {
		VMResultArray<T> arr;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL) {
			arr.reserve(vector->size());
			for (List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
				T var = cast(*itr);
				arr.push_back(var);
			}
		}
		s_dataLock.Leave();
		return arr;
	}


	template <typename T, typename S>
	int Values<T, S>::CountPrefixKey(std::string prefix) {
		if (prefix.length() < 1) return 0;
		int count = 0;
		s_dataLock.Enter();
		boost::to_lower(prefix);
		for (Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
			Obj &ObjRef = itr->second;
			for (Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end(); ++itr2) {
				if (boost::starts_with(std::string(itr2->first), prefix)) count++;
			}
		}
		s_dataLock.Leave();
		return count;
	}

	template <typename T, typename S>
	int Values<T, S>::CountPrefixKey(UInt64 obj, std::string prefix) {
		if (prefix.length() < 1) return 0;
		int count = 0;
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			boost::to_lower(prefix);
			Obj &ObjRef = itr->second;
			for (Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end(); ++itr2) {
				if (boost::starts_with(std::string(itr2->first), prefix)) count++;
			}
		}
		s_dataLock.Leave();
		return count;
	}

	template <typename T, typename S>
	int Lists<T, S>::CountPrefixKey(std::string prefix) {
		if (prefix.length() < 1) return 0;
		int count = 0;
		s_dataLock.Enter();
		boost::to_lower(prefix);
		for (Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
			Obj &ObjRef = itr->second;
			for (Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end(); ++itr2) {
				if (boost::starts_with(std::string(itr2->first), prefix)) count++;
			}
		}
		s_dataLock.Leave();
		return count;
	}

	template <typename T, typename S>
	int Lists<T, S>::CountPrefixKey(UInt64 obj, std::string prefix) {
		if (prefix.length() < 1) return 0;
		int count = 0;
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			boost::to_lower(prefix);
			Obj &ObjRef = itr->second;
			for (Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end(); ++itr2) {
				if (boost::starts_with(std::string(itr2->first), prefix)) count++;
			}
		}
		s_dataLock.Leave();
		return count;
	}

	template <typename T, typename S>
	int Values<T, S>::ClearPrefixKey(UInt64 obj, std::string prefix) {
		if (prefix.length() < 1) return 0;

		int count  = 0;
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			boost::to_lower(prefix);
			Obj &ObjRef = itr->second;
			for (Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end();) {
				std::string key = itr2->first;
				if (boost::starts_with(key, prefix)) {
					itr2 = ObjRef.erase(itr2);
					count++;
				}
				else ++itr2;
			}
		}
		s_dataLock.Leave();
		return count;
	}

	template <typename T, typename S>
	int Lists<T, S>::ClearPrefixKey(UInt64 obj, std::string prefix) {
		if (prefix.length() < 1) return 0;

		int count = 0;
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			boost::to_lower(prefix);
			Obj &ObjRef = itr->second;
			for (Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end();) {
				std::string key = itr2->first;
				if (boost::starts_with(key, prefix)) {
					itr2 = ObjRef.erase(itr2);
					count++;
				}
				else ++itr2;
			}
		}
		s_dataLock.Leave();
		return count;
	}

	template <typename T, typename S>
	int Values<T, S>::ClearPrefixKey(std::string prefix) {
		if (prefix.length() < 1) return 0;

		int count = 0;
		s_dataLock.Enter();
		boost::to_lower(prefix);
		for (Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
			Obj &ObjRef = itr->second;
			for (Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end();) {
				std::string key = itr2->first;
				if (boost::starts_with(key, prefix)) {
					itr2 = ObjRef.erase(itr2);
					count++;
				}
				else ++itr2;
			}
		}
		s_dataLock.Leave();
		return count;
	}

	template <typename T, typename S>
	int Lists<T, S>::ClearPrefixKey(std::string prefix) {
		if (prefix.length() < 1) return 0;

		int count = 0;
		s_dataLock.Enter();
		boost::to_lower(prefix);
		for (Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
			Obj &ObjRef = itr->second;
			for (Obj::iterator itr2 = ObjRef.begin(); itr2 != ObjRef.end();) {
				std::string key = itr2->first;
				if (boost::starts_with(key, prefix)) {
					itr2 = ObjRef.erase(itr2);
					count++;
				}
				else ++itr2;
			}
		}
		s_dataLock.Leave();
		return count;
	}


	inline bool CheckType(TESForm* ref, const std::vector<UInt8> &validtypes) {
		return std::find(validtypes.begin(), validtypes.end(), ref->formType) != validtypes.end();
	}
	template <>	VMResultArray<TESForm*> Lists<TESForm*, UInt32>::FilterByTypes(UInt64 obj, std::string key, VMArray<UInt32> types, bool matching) {
		VMResultArray<TESForm*> output;
		if (types.Length() < 1) return output;
		// Get easier to search type list.
		std::vector<UInt8> valid;
		valid.reserve(types.Length());
		for (UInt32 idx = 0; idx < types.Length(); ++idx) {
			UInt32 t(0); types.Get(&t, idx);
			valid.push_back((UInt8)t);
		}
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && !vector->empty()) {
			output.reserve(vector->size());
			for (List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
				TESForm* ref = cast(*itr);
				if (ref != NULL && matching == (std::find(valid.begin(), valid.end(), ref->formType) != valid.end())) {
					output.push_back(ref);
				}
			}
			output.shrink_to_fit();
		}
		s_dataLock.Leave();
		return output;
	}

	template <typename T, typename S>
	VMResultArray<T> Lists<T, S>::FilterByTypes(UInt64 obj, std::string key, VMArray<UInt32> types, bool matching){
		VMResultArray<T> output;
		return output;
	}

	// Special case for strings
	template <> int Lists<BSFixedString, std::string>::ListFind(UInt64 obj, std::string key, BSFixedString value) {
		int index = -1;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL) {
			std::string var = value.data;
			for (List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
				if (boost::iequals(var, *itr)) {
					index = itr - vector->begin();
					break;
				}
			}
		}
		s_dataLock.Leave();
		return index;
	}

	template <> bool Lists<BSFixedString, std::string>::ListHas(UInt64 obj, std::string key, BSFixedString value) {
		return ListFind(obj, key, value) != -1;
	}

	template <> int Lists<BSFixedString, std::string>::ListRemove(UInt64 obj, std::string key, BSFixedString value, bool allInstances) {
		int removed = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL) {
			// Remove
			int count = vector->size();
			std::string var = value.data;
			for (List::iterator itr = vector->begin(); itr != vector->end();) {
				if (boost::iequals(var, *itr)) {
					count++;
					vector->erase(itr);
					if (!allInstances) break;
				}
				else ++itr;
			}
			removed = count - vector->size();
			// Cleanup
			if (vector->empty()) {
				Data[obj].erase(key);
				if (Data[obj].size() == 0)
					Data.erase(obj);
			}
		}

		s_dataLock.Leave();
		return removed;
	}

	template <> int Lists<BSFixedString, std::string>::ListCountValue(UInt64 obj, std::string key, BSFixedString value, bool exclude) {
		int count = 0;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL) {
			std::string var = value.data;
			for (List::iterator itr = vector->begin(); itr != vector->end(); ++itr) {
				if (boost::iequals(var, *itr))
					count += 1;
			}
			if (exclude) count = (vector->size() - count);
		}
		s_dataLock.Leave();
		return count;
	}



	//static boost::random_device rd;
	static boost::random::mt19937 rnd;
	static int randomIndex(int size) {
		//rnd.seed(rd());
		return boost::random::uniform_int_distribution<>(0, (size - 1))(rnd);
	}

	template <typename T, typename S>
	T Lists<T, S>::ListRandom(UInt64 obj, std::string key) {
		S value = S();
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && !vector->empty()) {
			int index = randomIndex(vector->size());
			//_MESSAGE("ListRandom - Size(%d) - randomIndex(%d)", root[type][key].size(), index);
			value = vector->at(index);
		}
		s_dataLock.Leave();
		return cast(value);
	}

	/*TODO*/
	template <typename T, typename S>
	VMResultArray<T>  Lists<T, S>::ListRandomArray(UInt64 obj, std::string key, UInt32 count, bool duplicates) {
		VMResultArray<T> output;
		output.reserve(count);
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && !vector->empty()) {

		}

		s_dataLock.Leave();
		return output;
	}



	/*
	* DEBUG methods
	*/

	template <typename T, typename S>
	void Values<T, S>::RemoveForm(UInt64 &obj) {
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()) {
			UInt32 type = (UInt32)(obj >> 32);
			UInt32 id = (UInt32)(obj);
			_MESSAGE("Removed VALUE handle[%llu][%lu] with id: %lu", obj, type, id);
			Data.erase(itr);
		}
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Lists<T, S>::RemoveForm(UInt64 &obj) {
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()){
			UInt32 type = (UInt32)(obj >> 32);
			UInt32 id = (UInt32)(obj);
			_MESSAGE("Removed LIST handle[%llu][%lu] with id: %lu", obj, type, id);
			Data.erase(itr);
		}
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	int Values<T, S>::Cleanup() {
		int removed = 0;
		s_dataLock.Enter();
		for (Map::iterator itr = Data.begin(); itr != Data.end();) {
			if (itr->first != 0) {
				
				// SKSE64 EDIT
				//TESForm* ptr = (TESForm*)Forms::GameGetForm((UInt32)(itr->first & 0xFFFFFFFF));
				// TODO/FIXME SKSE64: not sure if replacing GameGetForm() with LookupFormByID() will work as expected
				UInt32 id = (itr->first & 0xFFFFFFFF);
				TESForm* ptr = id <= 0 ? NULL : LookupFormByID(id);
				// END SKSE64 EDIT

				if (itr->second.empty() || !Forms::IsValidObject(ptr, itr->first)) {
					itr = Data.erase(itr);
					removed++;
					continue;
				}
			}
			itr++;
		}
		s_dataLock.Leave();
		return removed;
	}

	template <typename T, typename S>
	int Lists<T, S>::Cleanup() {
		int removed = 0;
		s_dataLock.Enter();
		for (Map::iterator itr = Data.begin(); itr != Data.end();) {
			if (itr->first != 0) {
				
				// SKSE64 EDIT
				//TESForm* ptr = (TESForm*)Forms::GameGetForm((UInt32)(itr->first & 0xFFFFFFFF));
				// TODO/FIXME SKSE64: not sure if replacing GameGetForm() with LookupFormByID() will work as expected
				UInt32 id = (itr->first & 0xFFFFFFFF);
				TESForm* ptr = id <= 0 ? NULL : LookupFormByID(id);
				// END SKSE64 EDIT

				if (itr->second.empty() || !Forms::IsValidObject(ptr, itr->first)) {
					itr = Data.erase(itr);
					removed++;
					continue;
				}
			}
			itr++;
		}
		s_dataLock.Leave();
		return removed;
	}

	template <typename T, typename S>
	TESForm* Values<T, S>::GetNthObj(UInt32 i) {
		if (Data.size() <= i)
			return NULL;
		Map::iterator itr = Data.begin();
		std::advance(itr, i);
		return itr != Data.end() ? LookupFormByID((UInt32)(itr->first & 0xFFFFFFFF)) : NULL;
	}

	template <typename T, typename S>
	TESForm* Lists<T, S>::GetNthObj(UInt32 i) {
		if (Data.size() <= i)
			return NULL;
		Map::iterator itr = Data.begin();
		std::advance(itr, i);
		return itr != Data.end() ? LookupFormByID((UInt32)(itr->first & 0xFFFFFFFF)) : NULL;
	}

	template <typename T, typename S>
	const std::string Values<T, S>::GetNthKey(UInt64 obj, UInt32 i) {
		if (Data.find(obj) == Data.end() || Data[obj].size() <= i)
			return "";
		Obj::iterator itr = Data[obj].begin();
		std::advance(itr, i);
		return itr != Data[obj].end() ? itr->first : "";
	}

	template <typename T, typename S>
	const std::string Lists<T, S>::GetNthKey(UInt64 obj, UInt32 i) {
		if (Data.find(obj) == Data.end() || Data[obj].size() <= i)
			return "";
		Obj::iterator itr = Data[obj].begin();
		std::advance(itr, i);
		return itr != Data[obj].end() ? itr->first : "";
	}

	template <typename T, typename S>
	VMResultArray<TESForm*> Values<T, S>::GetAllObj() {
		VMResultArray<TESForm*> arr;
		s_dataLock.Enter();
		arr.reserve(Data.size());
		for (Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
			TESForm* FormRef = LookupFormByID((UInt32)(itr->first & 0xFFFFFFFF));
			arr.push_back(FormRef);
		}
		s_dataLock.Leave();
		return arr;
	}

	template <typename T, typename S>
	VMResultArray<TESForm*> Lists<T, S>::GetAllObj() {
		VMResultArray<TESForm*> arr;
		s_dataLock.Enter();
		arr.reserve(Data.size());
		for (Map::iterator itr = Data.begin(); itr != Data.end(); ++itr) {
			TESForm* FormRef = LookupFormByID((UInt32)(itr->first & 0xFFFFFFFF));
			arr.push_back(FormRef);
		}
		s_dataLock.Leave();
		return arr;
	}

	template <typename T, typename S>
	VMResultArray<BSFixedString> Values<T, S>::GetAllObjKeys(UInt64 obj) {
		VMResultArray<BSFixedString> arr;
		s_dataLock.Enter();
		if (Data.size() > 0 && Data.find(obj) != Data.end()) {
			for (Obj::iterator itr = Data[obj].begin(); itr != Data[obj].end(); ++itr) {
				arr.push_back(BSFixedString(itr->first.c_str()));
			}
		}
		s_dataLock.Leave();
		return arr;
	}

	template <typename T, typename S>
	VMResultArray<BSFixedString> Lists<T, S>::GetAllObjKeys(UInt64 obj) {
		VMResultArray<BSFixedString> arr;
		s_dataLock.Enter();
		if (Data.size() > 0 && Data.find(obj) != Data.end()) {
			for (Obj::iterator itr = Data[obj].begin(); itr != Data[obj].end(); ++itr) {
				arr.push_back(BSFixedString(itr->first.c_str()));
			}
		}
		s_dataLock.Leave();
		return arr;
	}




	// LEGACY style LOADING in single stream
	template <typename T, typename S>
	void Values<T, S>::LoadStream(std::stringstream &ss){
		int count;
		ss >> count;
		//_MESSAGE("- count: %d", count);
		if (count < 1) return;
		s_dataLock.Enter();
		Data.reserve(count);
		for (int i = 0; i < count; i++) {
			UInt64 objKey;
			ss >> objKey;
			int count2;
			ss >> count2;
			//_MESSAGE("\t - Object: %llu (%d)", objKey, count2);

			// Check if objKey still exists and prevent it falling into global 0 key
			if (objKey != 0) {
				TESForm *ObjRef = Forms::ResolveFormKey(objKey);
				objKey = Forms::GetFormKey(ObjRef);
				if (ObjRef == NULL || objKey == 0) {
					// Object no longer exists - don't load it's content
					_MESSAGE("\tDiscarding Empty Form");
					for (int n = 0; n < count2; n++) {
						std::string key;
						S value;
						ss >> key;
						ss >> value;
						//_MESSAGE("\t -- Key: %s", key.c_str());
					}
					continue;
				}
			}

			// Valid object or global key
			Data[objKey].reserve(count2);
			for (int n = 0; n < count2; n++) {
				// Key
				std::string key;
				ss >> key;
				DecodeValue(key);
				//_MESSAGE("\t -- Key: %s", key.c_str());
				//boost::to_lower(key);
				// Value
				S value;
				ss >> value;
				DecodeValue(value);
				// Save to data store
				Data[objKey][key] = value;
			}
		}
		Data.shrink_to_fit();
		if (count != Data.size()){
			int size = Data.size();
			_MESSAGE("\tData Shrink: %d -> %d", count, size);
		}
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Lists<T, S>::LoadStream(std::stringstream &ss){
		int count;
		ss >> count;
		//_MESSAGE("- count: %d", count);
		if (count < 1) return;
		s_dataLock.Enter();
		Data.reserve(count);
		for (int i = 0; i < count; ++i) {
			UInt64 objKey;
			ss >> objKey;
			int count2;
			ss >> count2;
			//_MESSAGE("\t - Object: %llu (%d)", objKey, count2);

			// Check if objKey still exists and prevent it falling into global 0 key
			if (objKey != 0) {
				TESForm *ObjRef = Forms::ResolveFormKey(objKey);
				objKey = Forms::GetFormKey(ObjRef);
				if (ObjRef == NULL || objKey == 0) {
					// Object no longer exists - don't load it's content
					_MESSAGE("\tDiscarding Empty Form");
					for (int n = 0; n < count2; ++n) {
						std::string key;
						int count3;
						ss >> key;
						ss >> count3;
						for (int k = 0; k < count3; ++k){
							S value;
							ss >> value;
						}
					}
					continue;
				}
			}

			// Valid object or global key
			Data[objKey].reserve(count2);
			for (int n = 0; n < count2; ++n) {
				// Key
				std::string key;
				ss >> key;
				DecodeValue(key);
				boost::to_lower(key);
				// Vector
				int count3;
				ss >> count3;
				Data[objKey][key].reserve(count3);
				for (int k = 0; k < count3; ++k){
					S value;
					ss >> value;
					DecodeValue(value);
					Data[objKey][key].push_back(value);
				}

			}
		}
		Data.shrink_to_fit();
		if (count != Data.size()){
			int size = Data.size();
			_MESSAGE("\t--Data Shrink: %d -> %d", count, size);
		}
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Values<T, S>::SaveStream(std::stringstream &ss) {
		//_MESSAGE("- count: %d", (int)Data.size());
		if (Data.size() < 1) return;
		s_dataLock.Enter();
		ss << (int)Data.size();
		for (Map::iterator i = Data.begin(); i != Data.end(); ++i) {
			int size = i->second.size();
			if (size > 0) {
				ss << ' ' << i->first;
				ss << ' ' << size;
				//_MESSAGE("\t - Object: %llu (%d)", i->first, size);
				for (Obj::iterator n = i->second.begin(); n != i->second.end(); ++n) {
					// Key
					std::string key = n->first;
					//_MESSAGE("\t -- Key: %s", key.c_str());
					EncodeValue(key);
					ss << ' ' << key;
					// Value
					S val = n->second;
					EncodeValue(val);
					ss << ' ' << val;
				}
			}
			else {
				_MESSAGE("\t Discarding empty: %llu", i->first);
			}
			
		}
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Lists<T, S>::SaveStream(std::stringstream &ss) {
		//_MESSAGE("- count: %d", (int)Data.size());
		if (Data.size() < 1) return;
		s_dataLock.Enter();
		ss << (int)Data.size();
			for (Map::iterator i = Data.begin(); i != Data.end(); ++i) {
				int size = i->second.size();
				if (size > 0) {
					ss << ' ' << i->first;
					ss << ' ' << size;
					//_MESSAGE("\t - Object: %llu (%d)", i->first, size);
					for (Obj::iterator n = i->second.begin(); n != i->second.end(); ++n) {
						// Key
						std::string key = n->first;
						EncodeValue(key);
						//_MESSAGE("\t -- %s", key.c_str());
						ss << ' ' << key;
						// Vector
						ss << ' ' << (int)n->second.size();
						for (List::iterator itr = n->second.begin(); itr != n->second.end(); ++itr) {
							S value = *itr;
							EncodeValue(value);
							ss << ' ' << value;
						}
					}
				}
				else {
					_MESSAGE("\t Discarding empty: %llu", i->first);
				}
				
			}
		s_dataLock.Leave();
	}


	template <typename T, typename S>
	void Values<T, S>::Revert(){
		s_dataLock.Enter();
		Data.clear();
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Lists<T, S>::Revert(){
		s_dataLock.Enter();
		Data.clear();
		s_dataLock.Leave();
	}

	template class Values<SInt32, SInt32>;
	template class Values<float, float>;
	template class Values<BSFixedString, std::string>;
	template class Values<TESForm*, UInt32>;
	template class Lists<SInt32, SInt32>;
	template class Lists<float, float>;
	template class Lists<BSFixedString, std::string>;
	template class Lists<TESForm*, UInt32>;

}




 