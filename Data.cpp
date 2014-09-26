#include "Data.h"
#include "External.h"
#include "Forms.h"

#include "skse/GameForms.h"

#include <boost/algorithm/string.hpp>

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

	// Overrides
	forl* packageLists = NULL;
	forl* GetPackages(){ if (!packageLists) packageLists = new forl(); return packageLists; }

	//aniv* animValues = NULL;
	//aniv* GetAnimations(){ return animValues; }

	void InitLists() {
		intValues = new intv();
		floatValues = new flov();
		stringValues = new strv();
		formValues = new forv();

		intLists = new intl();
		floatLists = new flol();
		stringLists = new strl();
		formLists = new forl();

		packageLists = new forl();
	}

	/*
	*
	* Define methods
	*
	*/

	template <typename T, typename S>
	T Values<T, S>::SetValue(UInt64 obj, std::string key, T value){
		s_dataLock.Enter();
		boost::to_lower(key);
		Data[obj][key] = cast(value);
		s_dataLock.Leave();
		return value;
	}

	template <typename T, typename S>
	T Values<T, S>::GetValue(UInt64 obj, std::string key, T value){
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()){
			boost::to_lower(key);
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end())
				value = cast(itr2->second);
		}
		s_dataLock.Leave();
		return value;
	}

	template <typename T, typename S>
	T Values<T, S>::AdjustValue(UInt64 obj, std::string key, T value){
		s_dataLock.Enter();
		boost::to_lower(key);
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()){
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end())
				value = (value + cast(itr2->second));
		}
		Data[obj][key] = value;
		s_dataLock.Leave();
		return value;
	}
	template <> BSFixedString Values<BSFixedString, std::string>::AdjustValue(UInt64 obj, std::string key, BSFixedString value){ return value; }
	template <> TESForm* Values<TESForm*, UInt32>::AdjustValue(UInt64 obj, std::string key, TESForm* value){ return value; }

	template <typename T, typename S>
	bool Values<T, S>::UnsetValue(UInt64 obj, std::string key){
		bool unset = false;
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()){
			boost::to_lower(key);
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end()){
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
	bool Values<T, S>::HasValue(UInt64 obj, std::string key){
		bool found = false;
		s_dataLock.Enter();

		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()){
			boost::to_lower(key);
			found = itr->second.find(key) != itr->second.end();
		}

		s_dataLock.Leave();
		return found;
	}

	template <typename T, typename S>
	int Lists<T, S>::ListAdd(UInt64 obj, std::string key, T value, bool allowDuplicate){
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
	T Lists<T, S>::ListSet(UInt64 obj, std::string key, UInt32 index, T value){
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && index >= 0 && index < vector->size())
			vector->at(index) = cast(value);
		else if (index == 0 && (vector == NULL || vector->size() == 0))
			Data[obj][key].push_back(cast(value));
		else{
			S var = S();
			value = cast(var);
		}

		s_dataLock.Leave();
		return value;
	}

	template <typename T, typename S>
	T Lists<T, S>::ListAdjust(UInt64 obj, std::string key, UInt32 index, T value){
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && index >= 0 && index < vector->size()){
			vector->at(index) += cast(value);
			value = cast(vector->at(index));
		}
		else{
			S var = S();
			value = cast(var);
		}

		s_dataLock.Leave();
		return value;
	}
	template <> BSFixedString Lists<BSFixedString, std::string>::ListAdjust(UInt64 obj, std::string key, UInt32 index, BSFixedString value){ return value; }
	template <> TESForm* Lists<TESForm*, UInt32>::ListAdjust(UInt64 obj, std::string key, UInt32 index, TESForm* value){ return value; }
	
	template <typename T, typename S>
	int Lists<T, S>::ListRemove(UInt64 obj, std::string key, T value, bool allInstances){
		int removed = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL){
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
			if (vector->size() == 0) {
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
		if (vector != NULL && vector->size() > 0 && index < vector->size()) {
			try {
				List::iterator itr = vector->begin();
				std::advance(itr, index);
				vector->insert(itr, cast(value));
				//vector->at(index) = value;
				inserted = true;
			}
			catch (const std::out_of_range&){
				inserted = false;
			}
		}
		else if (index == 0 && (vector == NULL || vector->size() == 0)){
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
		if (vector != NULL && vector->size() > 0 && index < vector->size()) {
			vector->erase(vector->begin() + index);
			removed = true;
			// Cleanup
			if (vector->size() == 0) {
				Data[obj].erase(key);
				if (Data[obj].size() == 0)
					Data.erase(obj);
			}
		}
		s_dataLock.Leave();
		return removed;
	}


	template <typename T, typename S>
	int Lists<T, S>::ListClear(UInt64 obj, std::string key){
		int removed = 0;
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()){
			boost::to_lower(key);
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end()){
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
	int Lists<T, S>::ListCount(UInt64 obj, std::string key){
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
	int Lists<T, S>::ListFind(UInt64 obj, std::string key, T value){
		int index = -1;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL){
			List::iterator itr = std::find(vector->begin(), vector->end(), cast(value));
			if (itr != vector->end())
				index = itr - vector->begin();
		}
		s_dataLock.Leave();
		return index;
	}

	template <typename T, typename S>
	bool Lists<T, S>::ListHas(UInt64 obj, std::string key, T value){
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
	void Lists<T, S>::ListSlice(UInt64 obj, std::string key, VMArray<T> Output, UInt32 startIndex){
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL && startIndex < vector->size() && startIndex >= 0) {
			List::iterator itr = vector->begin();
			std::advance(itr, startIndex);
			for (UInt32 index = 0; index < Output.Length() && itr != vector->end(); ++itr, ++index){
				T val = cast(*itr);
				Output.Set(&val, index);
			}
		}
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	int Lists<T, S>::ListResize(UInt64 obj, std::string key, UInt32 length, T filler){
		if (length == 0)
			return ListClear(obj, key) * -1;
		s_dataLock.Enter();

		int start = 0;
		S value = cast(filler);
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL) {
			start = vector->size();
			if (length < vector->size())
				vector->resize(length);
			else {
				for (UInt32 i = 0; length > vector->size() && i < 500; ++i)
					vector->push_back(value);
			}
		}
		else {
			for (UInt32 i = 0; length > Data[obj][key].size() && i < 500; ++i)
				Data[obj][key].push_back(value);				
		}

		s_dataLock.Leave();
		return Data[obj][key].size() - start;
	}

	template <typename T, typename S>
	bool Lists<T, S>::ListCopy(UInt64 obj, std::string key, VMArray<T> Input) {
		if (Input.Length() < 1)
			return false;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL)
			vector->clear();
		for (UInt32 i = 0; i < Input.Length(); ++i) {
			T var;
			Input.Get(&var, i);
			S value = cast(var);
			Data[obj][key].push_back(value);
		}
		s_dataLock.Leave();
		return true;
	}

	// Special case for strings
	template <> int Lists<BSFixedString, std::string>::ListFind(UInt64 obj, std::string key, BSFixedString value){
		int index = -1;
		s_dataLock.Enter();
		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL){
			std::string var = value.data;
			for (List::iterator itr = vector->begin(); itr != vector->end(); ++itr){
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

	/*template <> int Lists<BSFixedString, std::string>::ListAdd(UInt64 obj, std::string key, BSFixedString value, bool allowDuplicate){
		if (!allowDuplicate && ListFind(obj, key, value) != -1)
			return -1;
		s_dataLock.Enter();

		boost::to_lower(key);
		int index = Data[obj][key].size();
		Data[obj][key].push_back(value.data);
		
		s_dataLock.Leave();
		return index;
	}*/

	template <> int Lists<BSFixedString, std::string>::ListRemove(UInt64 obj, std::string key, BSFixedString value, bool allInstances){
		int removed = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		List* vector = GetVector(obj, key);
		if (vector != NULL){
			// Remove
			int count = vector->size();
			std::string var = value.data;
			for (List::iterator itr = vector->begin(); itr != vector->end();){
				if (boost::iequals(var, *itr)){
					count++;
					vector->erase(itr);
					if (!allInstances) break;
				}
				else ++itr;
			}
			removed = count - vector->size();
			// Cleanup
			if (vector->size() == 0) {
				Data[obj].erase(key);
				if (Data[obj].size() == 0)
					Data.erase(obj);
			}
		}

		s_dataLock.Leave();
		return removed;
	}

	/*
	*
	* DEBUG methods
	*
	*/

	template <typename T, typename S>
	void Values<T, S>::RemoveForm(UInt64 &obj) {
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end())
			Data.erase(itr);
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Lists<T, S>::RemoveForm(UInt64 &obj) {
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end())
			Data.erase(itr);
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	int Values<T, S>::Cleanup() {
		int removed = 0;
		s_dataLock.Enter();
		for (Map::iterator itr = Data.begin(); itr != Data.end();){
			if (itr->first == 0){
				itr++;
				continue;
			}
			TESForm* ptr = (TESForm*)Forms::GameGetForm((UInt32)(itr->first & 0xFFFFFFFF));
			if (!ptr || !Forms::IsValidObject(ptr, itr->first)) {
				removed++;
				itr = Data.erase(itr);
			}
			else
				itr++;
		}
		s_dataLock.Leave();
		return removed;
	}
	
	template <typename T, typename S>
	int Lists<T, S>::Cleanup() {
		int removed = 0;
		s_dataLock.Enter();
		for (Map::iterator itr = Data.begin(); itr != Data.end();){
			if (itr->first == 0){
				itr++;
				continue;
			}
			TESForm* ptr = (TESForm*)Forms::GameGetForm((UInt32)(itr->first & 0xFFFFFFFF));
			if (!ptr || !Forms::IsValidObject(ptr, itr->first)) {
				removed++;
				itr = Data.erase(itr);
			}
			else
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
	std::string Values<T, S>::GetNthKey(UInt64 obj, UInt32 i){
		if (Data.find(obj) == Data.end() || Data[obj].size() <= i)
			return "";
		Obj::iterator itr = Data[obj].begin();
		std::advance(itr, i);
		return itr != Data[obj].end() ? itr->first : "";
	}

	template <typename T, typename S>
	std::string Lists<T, S>::GetNthKey(UInt64 obj, UInt32 i){
		if (Data.find(obj) == Data.end() || Data[obj].size() <= i)
			return "";
		Obj::iterator itr = Data[obj].begin();
		std::advance(itr, i);
		return itr != Data[obj].end() ? itr->first : "";
	}


	template <typename S>
	void EncodeValue(S &v) {}
	void EncodeValue(std::string &v){
		if (v.empty())
			v += (char)0x1B;
		else
			std::replace(v.begin(), v.end(), ' ', (char)0x7);
	}

	template <typename S>
	void DecodeValue(S &v) {}
	void DecodeValue(std::string &v){
		if (v.size() == 1 && v[0] == (char)0x1B)
			v.clear();
		else
			std::replace(v.begin(), v.end(), (char)0x7, ' ');
	}
	void DecodeValue(UInt32 &v){ v = Forms::GetNewId(v); }

	// LEGACY style LOADING in single stream
	template <typename T, typename S>
	void Values<T, S>::LoadStream(std::stringstream &ss){
		int count;
		ss >> count;
		if (count < 1)
			return;
		s_dataLock.Enter();

		for (int i = 0; i < count; i++) {
			UInt64 objKey;
			ss >> objKey;
			int count2;
			ss >> count2;

			// Check if objKey still exists and prevent it falling into global 0 key
			if (objKey != 0) {
				objKey = Forms::GetNewKey(objKey);
				TESForm *ObjRef = Forms::GetFormKey(objKey);
				if (ObjRef == NULL) {
					// Object no longer exists - don't load it's content
					_MESSAGE("-- Discarding Empty Form");
					for (int n = 0; n < count2; n++) {
						std::string key;
						S value;
						ss >> key;
						ss >> value;
						// _MESSAGE("---- Key: %s", key.c_str());
					}
					continue;
				}
			}

			// Valid object or global key
			for (int n = 0; n < count2; n++) {
				// Key
				std::string key;
				ss >> key;
				DecodeValue(key);
				boost::to_lower(key);
				// Value
				S value;
				ss >> value;
				DecodeValue(value);
				// Save to data store
				Data[objKey][key] = value;
			}
		}
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Lists<T, S>::LoadStream(std::stringstream &ss){
		int count;
		ss >> count;
		if (count < 1)
			return;
		s_dataLock.Enter();
		for (int i = 0; i < count; ++i) {
			UInt64 objKey;
			ss >> objKey;
			int count2;
			ss >> count2;

			// Check if objKey still exists and prevent it falling into global 0 key
			if (objKey != 0) {
				objKey = Forms::GetNewKey(objKey);
				TESForm *ObjRef = Forms::GetFormKey(objKey);

				if (ObjRef == NULL) {
					// Object no longer exists - don't load it's content
					_MESSAGE("-- Discarding Empty Form");
					for (int n = 0; n < count2; ++n) {
						std::string key;
						int count3;
						ss >> key;
						ss >> count3;
						_MESSAGE("---- Key: %s", key.c_str());
						for (int k = 0; k < count3; ++k){
							S value;
							ss >> value;
						}
					}
					continue;
				}
			}

			// Valid object or global key
			for (int n = 0; n < count2; ++n) {
				// Key
				std::string key;
				ss >> key;
				DecodeValue(key);
				boost::to_lower(key);
				// Vector
				int count3;
				ss >> count3;
				for (int k = 0; k < count3; ++k){
					S value;
					ss >> value;
					DecodeValue(value);
					Data[objKey][key].push_back(value);
				}

			}
		}
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Values<T, S>::SaveStream(std::stringstream &ss) {
		s_dataLock.Enter();
		ss << (int)Data.size();
		for (Map::iterator i = Data.begin(); i != Data.end(); ++i) {
			ss << ' ' << i->first;
			ss << ' ' << (int)i->second.size();
			for (Obj::iterator n = i->second.begin(); n != i->second.end(); ++n) {
				// Key
				std::string key = n->first;
				EncodeValue(key);
				ss << ' ' << key;
				// Value
				S val = n->second;
				EncodeValue(val);
				ss << ' ' << val;
			}
		}
		s_dataLock.Leave();
	}

	template <typename T, typename S>
	void Lists<T, S>::SaveStream(std::stringstream &ss) {
		s_dataLock.Enter();
		ss << (int)Data.size();
			for (Map::iterator i = Data.begin(); i != Data.end(); ++i) {
				ss << ' ' << i->first;
				ss << ' ' << (int)i->second.size();
				for (Obj::iterator n = i->second.begin(); n != i->second.end(); ++n) {
					// Key
					std::string key = n->first;
					EncodeValue(key);
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


	/*
	// NEW style SAVING with individual streams
	template <typename T, typename S>
	void Values<T, S>::Save(SKSESerializationInterface *intfc, UInt32 type){
		if (intfc->OpenRecord(type, kSerializationDataVersion)) {
			s_dataLock.Enter();
			std::stringstream ss;
			ss << (int)Data.size();
			for (Map::iterator i = Data.begin(); i != Data.end(); ++i) {
				ss << ' ' << i->first;
				ss << ' ' << (int)i->second.size();
				for (Obj::iterator n = i->second.begin(); n != i->second.end(); ++n) {
					// Key
					std::string key = n->first;
					EncodeValue(key);
					ss << ' ' << key;
					// Value
					S val = n->second;
					EncodeValue(val);
					ss << ' ' << val;
				}
			}
			std::string str = ss.str();
			const char *cstr = str.c_str();
			//_MESSAGE(cstr);
			intfc->WriteRecordData(cstr, strlen(cstr));
			s_dataLock.Leave();
		}
	}

	template <typename T, typename S>
	void Lists<T, S>::Save(SKSESerializationInterface *intfc, UInt32 type){
		if (intfc->OpenRecord(type, kSerializationDataVersion)) {
			s_dataLock.Enter();
			std::stringstream ss;
			
			std::string str = ss.str();
			const char *cstr = str.c_str();
			//_MESSAGE(cstr);
			intfc->WriteRecordData(cstr, strlen(cstr));
			s_dataLock.Leave();
		}
	}


	
	// NEW style LOADING with individual streams
	template <typename T, typename S>
	void Values<T, S>::Load(SKSESerializationInterface *intfc, UInt32 &version, UInt32 &length){
		if (version == kSerializationDataVersion) {
			if (length) {
				char *buf = new char[length + 1];
				intfc->ReadRecordData(buf, length);
				buf[length] = 0;
				_MESSAGE("length: %d", strlen(buf));
				std::stringstream ss(buf);
				LoadStream(ss);
				delete[] buf;
			}
		}
	}

	template <typename T, typename S>
	void Lists<T, S>::Load(SKSESerializationInterface *intfc, UInt32 &version, UInt32 &length){
		if (version == kSerializationDataVersion) {
			if (length) {
				char *buf = new char[length + 1];
				intfc->ReadRecordData(buf, length);
				buf[length] = 0;
				_MESSAGE("length: %d", strlen(buf));
				std::stringstream ss(buf);
				LoadStream(ss);
				delete[] buf;
			}
		}
	}
	*/


	template class Values<SInt32, SInt32>;
	template class Values<float, float>;
	template class Values<BSFixedString, std::string>;
	template class Values<TESForm*, UInt32>;
	template class Lists<SInt32, SInt32>;
	template class Lists<float, float>;
	template class Lists<BSFixedString, std::string>;
	template class Lists<TESForm*, UInt32>;

}



