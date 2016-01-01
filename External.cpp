#include "External.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <streambuf>

//#include <direct.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>

#include "skse/PapyrusVM.h"
#include "skse/PapyrusNativeFunctions.h"

namespace External {
	namespace fs = boost::filesystem;

	static ICriticalSection* s_loadLock = NULL;
	static FileVector* s_Files = NULL;


	// Make value for storage
	inline Value ExternalFile::make(SInt32 v) const {
		return Value::Int(v);
	}
	inline Value ExternalFile::make(float v) const {
		return Value(v);
	}
	inline Value ExternalFile::make(BSFixedString v) const {
		return Value(v.data);
	}
	inline Value ExternalFile::make(TESForm* v) const {
		return Value(Forms::GetFormString(v));
	}

	// Parse json value for output
	inline SInt32 ExternalFile::parse(Value value, SInt32 missing) const {
		return (value.isInt() || (!value.isNull() && value.isConvertibleTo(Json::intValue))) ? value.asInt() : missing;
	}
	inline float ExternalFile::parse(Value value, float missing)  const {
		return (value.isDouble() || (!value.isNull() && value.isConvertibleTo(Json::realValue))) ? value.asFloat() : missing;
	}
	inline TESForm* ExternalFile::parse(Value value, TESForm* missing) const {
		return value.isString() ? Forms::ParseFormString(value.asString()) : missing;
	}
	inline BSFixedString ExternalFile::parse(Value value, BSFixedString missing) const {
		return value.isString() ? BSFixedString(value.asCString()) : (!value.isNull() && value.isConvertibleTo(Json::stringValue)) ? BSFixedString(value.asString().c_str()) : missing;
	}


	/*
	//
	// GLOBAL KEY => VALUE STORAGE
	//
	*/


	template <typename T>
	void ExternalFile::SetValue(std::string key, T value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		root[Type<T>()][key] = make(value);
		isModified = true;
		s_dataLock.Leave();
	}
	template void ExternalFile::SetValue<SInt32>(std::string key, SInt32 value);
	template void ExternalFile::SetValue<float>(std::string key, float value);
	template void ExternalFile::SetValue<BSFixedString>(std::string key, BSFixedString value);
	template void ExternalFile::SetValue<TESForm*>(std::string key, TESForm* value);

	template <typename T>
	T ExternalFile::GetValue(std::string key, T value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		if (HasKey<T>(key))
			value = parse(root[Type<T>()][key], value);
		s_dataLock.Leave();
		return value;
	}
	template SInt32 ExternalFile::GetValue<SInt32>(std::string key, SInt32 value);
	template float ExternalFile::GetValue<float>(std::string key, float value);
	template BSFixedString ExternalFile::GetValue<BSFixedString>(std::string key, BSFixedString value);
	template TESForm* ExternalFile::GetValue<TESForm*>(std::string key, TESForm* value);

	void ExternalFile::SetValue(std::string type, std::string key, Value value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		root[type][key] = value;
		isModified = true;
		s_dataLock.Leave();
	}

	Value ExternalFile::GetValue(std::string type, std::string key, Value value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		if (HasKey(type, key))
			value = root[type][key];
		s_dataLock.Leave();
		return value;
	}

	Value ExternalFile::AdjustValue(std::string type, std::string key, Value value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		if (HasKey(type, key)){
			if (value.isInt()) value = Value::Int(value.asInt() + root[type][key].asInt());
			else value = Value::Int(value.asFloat() + root[type][key].asFloat());
		}
		root[type][key] = value;
		isModified = true;
		s_dataLock.Leave();
		return value;
	}

	bool ExternalFile::UnsetValue(std::string type, std::string key){
		bool removed = false;
		s_dataLock.Enter();

		boost::to_lower(key);
		if (HasKey(type, key)){
			isModified = true;
			removed = true;
			root[type].removeMember(key);
			//if (root[type].empty())
			//	root.removeMember(type);
		}

		s_dataLock.Leave();
		return removed;
	}

	bool ExternalFile::HasValue(std::string type, std::string key){
		bool has = false;
		s_dataLock.Enter();
		boost::to_lower(key);
		has = root.isMember(type) && root[type].isMember(key);
		s_dataLock.Leave();
		return has;
	}

	// Global key=>value=>list

	int ExternalFile::ListAdd(std::string type, std::string key, Value value, bool allowDuplicate){
		if (!allowDuplicate && ListFind(type, key, value) != -1) return -1;
		s_dataLock.Enter();
		boost::to_lower(key);
		value = root[type][key].append(value);
		isModified = true;
		s_dataLock.Leave();
		return (root[type][key].size() - 1);
	}

	Value ExternalFile::ListGet(std::string type, std::string key, int index){
		s_dataLock.Enter();
		boost::to_lower(key);
		Value value;
		if (HasKey(type, key) && root[type][key].isValidIndex(index))
			value = root[type][key].get(index, Value::null);
		s_dataLock.Leave();
		return value;
	}

	Value ExternalFile::ListSet(std::string type, std::string key, int index, Value value){
		s_dataLock.Enter();
		boost::to_lower(key);
		if (HasKey(type, key) && root[type][key].isValidIndex(index)){
			root[type][key][index] = value;
			isModified = true;
		}
		s_dataLock.Leave();
		return value;
	}

	int ExternalFile::ListRemove(std::string type, std::string key, Value removing, bool allInstances){
		int removed = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		if (HasKey(type, key)){
			Value list = Value(Json::arrayValue);
			for (Value::iterator itr = root[type][key].begin(); itr != root[type][key].end(); ++itr){
				if (!allInstances && removed > 0) list.append((*itr));
				else if (removing != (*itr)) list.append((*itr));
				else removed += 1;
			}
			root[type][key].swap(list);
			/*if (root[type][key].size() == 0){
				root[type].removeMember(key);
				if (root[type].size() == 0)
					root.removeMember(type);
			}*/
			isModified = true;
		}

		s_dataLock.Leave();
		return removed;
	}

	bool ExternalFile::ListRemoveAt(std::string type, std::string key, int index){
		bool removed = false;
		s_dataLock.Enter();

		boost::to_lower(key);
		if (HasKey(type, key) && root[type][key].isValidIndex(index)){
			Value list = Value(Json::arrayValue);
			Value::iterator itr = root[type][key].begin();
			for (itr = root[type][key].begin(); itr != root[type][key].end(); ++itr){
				if (itr.key().asInt() == index)	removed = true;
				else list.append((*itr));
			}
			root[type][key] = list;
			/*if (root[type][key].size() == 0){
				root[type].removeMember(key);
				if (root[type].size() == 0)
					root.removeMember(type);
			}*/
			isModified = true;
		}
		s_dataLock.Leave();
		return removed;
	}

	bool ExternalFile::ListInsertAt(std::string type, std::string key, int index, Value value){
		bool inserted = false;
		s_dataLock.Enter();

		boost::to_lower(key);
		if (HasKey(type, key) && root[type][key].isValidIndex(index)){
			Value list = Value(Json::arrayValue);
			Value::iterator itr = root[type][key].begin();
			for (itr = root[type][key].begin(); itr != root[type][key].end(); ++itr){
				if (itr.key().asInt() == index){
					list.append(value);
					inserted = true;
				}
				list.append((*itr));
			}
			root[type][key] = list;
			isModified = true;
		}

		s_dataLock.Leave();
		return inserted;
	}

	int ExternalFile::ListClear(std::string type, std::string key){
		int cleared = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		if (HasKey(type, key)){
			isModified = true;
			cleared    = root[type][key].size();
			root[type].removeMember(key);
			//if (root[type].size() == 0)
			//	root.removeMember(type);
		}

		s_dataLock.Leave();
		return cleared;
	}

	int ExternalFile::ListCount(std::string type, std::string key){
		int count = 0;
		s_dataLock.Enter();
		boost::to_lower(key);
		if (HasKey(type, key)) count = root[type][key].size();
		s_dataLock.Leave();
		return count;
	}

	int ExternalFile::ListCountValue(std::string type, std::string key, Value value, bool exclude){
		int count = 0;
		s_dataLock.Enter();
		boost::to_lower(key);
		if (HasKey(type, key)){
			for (Value::iterator itr = root[type][key].begin(); itr != root[type][key].end(); ++itr) {
				if (value == (*itr) || (value.isString() && boost::iequals((*itr).asString(), value.asString())))
					count += 1;
			}
			if (exclude) count = (root[type][key].size() - count);
		}
		s_dataLock.Leave();
		return count;
	}

	int ExternalFile::ListFind(std::string type, std::string key, Value value){
		int index = -1;
		s_dataLock.Enter();
		boost::to_lower(key);
		if (HasKey(type, key)){
			for (Value::iterator itr = root[type][key].begin(); itr != root[type][key].end(); ++itr) {
				if (value == (*itr) || (value.isString() && boost::iequals((*itr).asString(), value.asString()))){
					index = itr.key().asInt();
					//_MESSAGE("\t[%d] %s == %s", index, (*itr).asCString(), value.asCString());
					break;
				}
			}
		}
		s_dataLock.Leave();
		return index;
	}

	int ExternalFile::ListResize(std::string type, std::string key, int length, Value filler) {
		if (length == 0) return ListClear(type, key) * -1;
		s_dataLock.Enter();

		int start = 0;
		boost::to_lower(key);
		if (HasKey(type, key)){
			start = root[type][key].size();
			if (length < start)
				root[type][key].resize(length);
			else {
				for (int i = 0; length > root[type][key].size() && i < 500; ++i)
					root[type][key].append(filler);
			}
		}
		else {
			for (int i = 0; length > root[type][key].size() && i < 500; ++i)
				root[type][key].append(filler);
		}
		isModified = true;

		s_dataLock.Leave();
		return root[type][key].size() - start;
	}

	template <typename T>
	T ExternalFile::ListAdjust(std::string key, int index, T adjustBy){
		s_dataLock.Enter();
		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key) && root[type][key].isValidIndex(index)){
			adjustBy = adjustBy + ParseValue<T>(root[type][key][index], T());
			root[type][key][index] = MakeValue<T>(adjustBy);
			isModified = true;
		}
		s_dataLock.Leave();
		return adjustBy;
	}
	template SInt32 ExternalFile::ListAdjust<SInt32>(std::string key, int index, SInt32 adjustBy);
	template float ExternalFile::ListAdjust<float>(std::string key, int index, float adjustBy);


	template <typename T>
	void ExternalFile::ListSlice(std::string key, VMArray<T> Output, int startIndex){
		if (Output.Length() > 0) {
			UInt32 length(Output.Length());
			s_dataLock.Enter();

			boost::to_lower(key);
			std::string type = List<T>();
			if (HasKey(type, key)) {
				Value::iterator itr = root[type][key].begin();
				std::advance(itr, startIndex);
				for (int index = 0; index < length && itr != root[type][key].end(); ++itr, ++index) {
					T value = ParseValue<T>(root[type][key].get(itr.index(), Value::null));
					Output.Set(&value, index);
				}
			}
			s_dataLock.Leave();
		}
	}
	template void ExternalFile::ListSlice<SInt32>(std::string key, VMArray<SInt32> Output, int startIndex);
	template void ExternalFile::ListSlice<float>(std::string key, VMArray<float> Output, int startIndex);
	template void ExternalFile::ListSlice<BSFixedString>(std::string key, VMArray<BSFixedString> Output, int startIndex);
	template void ExternalFile::ListSlice<TESForm*>(std::string key, VMArray<TESForm*> Output, int startIndex);

	template <typename T>
	bool ExternalFile::ListCopy(std::string key, VMArray<T> Input) {
		if (Input.Length() > 0) {
			UInt32 length(Input.Length());
			s_dataLock.Enter();

			T var;
			boost::to_lower(key);
			std::string type = List<T>();
			if (HasKey(type, key)) root[type][key].clear();
			for (UInt32 i = 0; i < length; ++i) {
				Input.Get(&var, i);
				root[type][key].append(MakeValue<T>(var));
			}
			isModified = true;

			s_dataLock.Leave();
			return true;
		}
		return false;
	}
	template bool ExternalFile::ListCopy<SInt32>(std::string key, VMArray<SInt32> Input);
	template bool ExternalFile::ListCopy<float>(std::string key, VMArray<float> Input);
	template bool ExternalFile::ListCopy<BSFixedString>(std::string key, VMArray<BSFixedString> Input);
	template bool ExternalFile::ListCopy<TESForm*>(std::string key, VMArray<TESForm*> Input);


	template <typename T>
	VMResultArray<T> ExternalFile::ToArray(std::string key) {
		VMResultArray<T> arr;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key)){
			arr.reserve(root[type][key].size());
			for (Value::iterator itr = root[type][key].begin(); itr != root[type][key].end(); ++itr){
				T var = ParseValue<T>(*itr);
				arr.push_back(var);
			}
		}

		s_dataLock.Leave();
		return arr;
	}
	template VMResultArray<SInt32> ExternalFile::ToArray<SInt32>(std::string key);
	template VMResultArray<float> ExternalFile::ToArray<float>(std::string key);
	template VMResultArray<BSFixedString> ExternalFile::ToArray<BSFixedString>(std::string key);
	template VMResultArray<TESForm*> ExternalFile::ToArray<TESForm*>(std::string key);

	int ExternalFile::CountPrefix(std::string type, std::string prefix) {
		int count = 0;
		if (root.isMember(type) && !root[type].empty()) {
			s_dataLock.Enter();
			boost::to_lower(prefix);
			Value::Members members = root[type].getMemberNames();
			for (Value::Members::iterator itr = members.begin(); itr != members.end(); ++itr) {
				if (boost::starts_with((*itr), prefix)) count++;
			}
			s_dataLock.Leave();
		}
		return count;
	}

	/*int ExternalFile::ClearPrefix(std::string type, std::string prefix) {
		int count = 0;
		if (root.isMember(type) && !root[type].empty()) {
			s_dataLock.Enter();
			boost::to_lower(prefix);
			Value list = Value(Json::objectValue);
			for (Value::iterator itr = root[type].begin(); itr != root[type].end(); ++itr) {
				if (boost::starts_with((*itr), prefix)) count++;
				else list.append(*itr);
			}
			root[type] = list;
			if (count > 0) isModified = true;
			s_dataLock.Leave();
		}
		return count;
	}*/








	/*
	//
	// RESOLVE PATH HANDLING
	//
	*/




	template <typename T> void ExternalFile::SetPathValue(std::string path, Value value) {
		if (!path.empty()) {
			s_dataLock.Enter();
			Path pathto(path.front() != '.' ? '.' + path : path);
			pathto.make(root).swap(value);
			isModified = true;
			s_dataLock.Leave();
		}
	}
	template void ExternalFile::SetPathValue<SInt32>(std::string path, Value value);
	template void ExternalFile::SetPathValue<float>(std::string path, Value value);
	template void ExternalFile::SetPathValue<BSFixedString>(std::string path, Value value);
	template void ExternalFile::SetPathValue<TESForm*>(std::string path, Value value);

	template <typename T> T ExternalFile::GetPathValue(std::string path, T defaultValue) {
		if (path.empty()) return defaultValue;
		s_dataLock.Enter();
		Value value = Resolve(path, MakeValue<T>(defaultValue));
		defaultValue = ParseValue<T>(value, defaultValue);
		s_dataLock.Leave();
		return defaultValue;
	}
	//template SInt32 ExternalFile::GetPathValue<SInt32>(std::string path, SInt32 defaultValue);
	//template float ExternalFile::GetPathValue<float>(std::string path, float defaultValue);
	//template BSFixedString ExternalFile::GetPathValue<BSFixedString>(std::string path, BSFixedString defaultValue);
	//template TESForm* ExternalFile::GetPathValue<TESForm*>(std::string path, TESForm* defaultValue);

	template <> SInt32 ExternalFile::GetPathValue(std::string path, SInt32 defaultValue) {
		if (path.empty()) return defaultValue;
		s_dataLock.Enter();

		Value value = Resolve(path, Value::Int(defaultValue));
		try
		{
			if (value.isInt() || value.isConvertibleTo(Json::intValue))
				defaultValue = value.asInt();
		}
		catch (Json::LogicError &msg)
		{
			_MESSAGE("Json::LogicError: %s", msg.what());
		}


		s_dataLock.Leave();
		return defaultValue;
	}

	template <> float ExternalFile::GetPathValue(std::string path, float defaultValue) {
		if (path.empty()) return defaultValue;
		s_dataLock.Enter();

		Value value = Resolve(path, Value(defaultValue));
		try
		{
			if (value.isInt() || value.isConvertibleTo(Json::realValue))
				defaultValue = value.asFloat();
		}
		catch (Json::LogicError &msg)
		{
			_MESSAGE("Json::LogicError: %s", msg.what());
		}


		s_dataLock.Leave();
		return defaultValue;
	}

	template <> std::string ExternalFile::GetPathValue(std::string path, std::string defaultValue) {
		if (path.empty()) return defaultValue;
		s_dataLock.Enter();

		Value value = Resolve(path, Value(defaultValue));
		try
		{
			if (value.isString() || value.isConvertibleTo(Json::stringValue))
				defaultValue = value.asString();
			//else if (value.isArray())
			//	defaultValue = "(array)";
			//else if (value.isObject())
			//	defaultValue = "(object)";

		}
		catch (Json::LogicError &msg)
		{
			_MESSAGE("Json::LogicError: %s", msg.what());
		}
		

		s_dataLock.Leave();
		return defaultValue;
	}

	template <> TESForm* ExternalFile::GetPathValue(std::string path, TESForm* defaultValue) {
		if (path.empty()) return defaultValue;
		s_dataLock.Enter();

		Value value = Resolve(path, MakeValue(defaultValue));
		try
		{
			if (value.isString()) defaultValue = Forms::ParseFormString(value.asString());
		}
		catch (Json::LogicError &msg)
		{
			_MESSAGE("Json::LogicError: %s", msg.what());
		}


		s_dataLock.Leave();
		return defaultValue;
	}



	template <typename T> VMResultArray<T> ExternalFile::PathElements(std::string path, T invalidType) {
		VMResultArray<T> arr;
		if (path.empty()) return arr;
		s_dataLock.Enter();
		Value value = Resolve(path);
		if (!value.isNull() && value.isArray() && value.size() > 0) {
			arr.reserve(value.size());
			for (Value::iterator itr = value.begin(); itr != value.end(); ++itr) {
				arr.push_back(ParseValue<T>(*itr, invalidType));
			}
		}
		s_dataLock.Leave();
		return arr;
	}
	/*template <> VMResultArray<BSFixedString> ExternalFile::PathElements(std::string path, BSFixedString invalidType) {
		VMResultArray<BSFixedString> arr;
		if (path.empty()) return arr;
		s_dataLock.Enter();
		Value value = Resolve(path);
		if (!value.isNull() && value.isArray() && value.size() > 0) {
			arr.reserve(value.size());
			for (Value::iterator itr = value.begin(); itr != value.end(); ++itr) {
				Value& var = *itr;
				if (var.isString())
					arr.push_back(BSFixedString(var.asCString()));
				else if (var.isConvertibleTo(Json::stringValue))
					arr.push_back(BSFixedString(var.asString().c_str()));
				else
					arr.push_back(invalidType);
			}
		}
		s_dataLock.Leave();
		return arr;
	}*/
	template VMResultArray<SInt32> ExternalFile::PathElements<SInt32>(std::string path, SInt32 invalidType);
	template VMResultArray<float> ExternalFile::PathElements<float>(std::string path, float invalidType);
	template VMResultArray<BSFixedString> ExternalFile::PathElements<BSFixedString>(std::string path, BSFixedString invalidType);
	template VMResultArray<TESForm*> ExternalFile::PathElements<TESForm*>(std::string path, TESForm* invalidType);


	VMResultArray<BSFixedString> ExternalFile::PathMembers(std::string path) {
		VMResultArray<BSFixedString> arr;
		if (path.empty()) return arr;
		s_dataLock.Enter();
		Value value = Resolve(path);
		if (value.isObject() && !value.empty()) {
			arr.reserve(value.size());
			Value::Members members = value.getMemberNames();
			for (Value::Members::iterator itr = members.begin(); itr != members.end(); ++itr) {
				arr.push_back(BSFixedString((*itr).c_str()));
			}
		}
		s_dataLock.Leave();
		return arr;
	}

	int ExternalFile::PathCount(std::string path) {
		if (path.empty()) return -1;
		s_dataLock.Enter();
		Value value = Resolve(path);
		int count = (value.isArray() || value.isObject()) ? value.size() : -1;
		s_dataLock.Leave();
		return count;
	}


	bool ExternalFile::CanResolve(std::string path) {
		return !Resolve(path).isNull();
	}
	bool ExternalFile::IsObject(std::string path) {
		return Resolve(path).isObject();
	}
	bool ExternalFile::IsArray(std::string path) {
		return Resolve(path).isArray();
	}
	bool ExternalFile::IsString(std::string path) {
		return Resolve(path).isString();
	}
	bool ExternalFile::IsNumber(std::string path) {
		return Resolve(path).isNumeric();
	}
	bool ExternalFile::IsBool(std::string path) {
		return Resolve(path).isBool();
	}
	bool ExternalFile::IsForm(std::string path) {
		Value value = Resolve(path);
		return value.isString() && Forms::IsFormString(value.asString());
	}
	
	



	/*
	//
	// FILE HANDLING
	//
	*/


	ExternalFile* GetFile(std::string name) {
		if (s_loadLock == NULL) s_loadLock = new ICriticalSection();
		s_loadLock->Enter();

		if (s_Files == NULL)
			s_Files = new FileVector();

		if (name.empty()) name = "noname.json";
		else if (name.find(".json") == std::string::npos)
			name += ".json";

		ExternalFile* File = NULL;
		for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr) {
			if (boost::iequals(name, (*itr)->name)) {
				File = (*itr);
				break;
			}
		}

		if (!File) {
			File = new ExternalFile(name);
			s_Files->push_back(File);
		}

		s_loadLock->Leave();
		return File;
	}

	bool RevertFile(std::string name, bool savechanges, bool minify) {
		if (s_Files == NULL) return false; // nothing to unload
		s_loadLock->Enter();

		if (name.empty()) name = "noname.json";
		else if (name.find(".json") == std::string::npos)
			name += ".json";

		bool saved = false;
		ExternalFile* File = NULL;
		for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr) {
			if (boost::iequals(name, (*itr)->name)) {
				File = (*itr);
				_MESSAGE("JSON Reverted %s", File->name.c_str());
				if (savechanges && File->isModified) saved = File->SaveFile(minify);
				s_Files->erase(itr);
				break;
			}
		}

		s_loadLock->Leave();
		return saved;
	}

	bool ChangesPending(std::string name) {
		if (s_Files == NULL) return false; // nothing to check
		s_loadLock->Enter();

		if (name.empty()) name = "noname.json";
		else if (name.find(".json") == std::string::npos)
			name += ".json";

		bool pending = false;
		ExternalFile* File = NULL;
		for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr) {
			if (boost::iequals(name, (*itr)->name)) {
				pending = (*itr)->isModified;
				break;
			}
		}

		s_loadLock->Leave();
		return pending;
	}

	void SaveFiles() {
		if (s_loadLock == NULL) s_loadLock = new ICriticalSection();
		s_loadLock->Enter();

		if (s_Files != NULL && !s_Files->empty()) {
			_MESSAGE("JSON Files: %d", (int)s_Files->size());
			for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr) {
				if ((*itr)->isModified) { _MESSAGE("\tSaving: %s", (*itr)->name.c_str()); (*itr)->SaveFile(); }
				//else { _MESSAGE("\tUnloading: %s", (*itr)->name.c_str()); (*itr)->RevertFile(); }
			}
			//s_Files->clear();
		}
		_MESSAGE("JSON SaveFiles - Done");
		s_loadLock->Leave();
	}

	void RevertFiles() {
		if (s_loadLock == NULL) s_loadLock = new ICriticalSection();
		s_loadLock->Enter();

		if (s_Files != NULL && !s_Files->empty()) {
			for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr) {
				(*itr)->RevertFile();
			}
			s_Files->clear();
		}

		s_loadLock->Leave();
	}





	/*bool ExternalFile::ImportOnForm(TESForm* FormRef) {
	return false;
	}*/

	/*/
	/// Class: ExternalFile
	/*/

	// read/write

	bool ExternalFile::LoadFile() {
		Json::Reader reader;
		//setlocale(LC_NUMERIC, "POSIX");
		// Path to file
		fs::path Path = fs::path(docpath);

		_MESSAGE("Loading: %s", Path.generic_string().c_str());
		// Check if file exists
		if (!fs::exists(Path)) {
			reader.parse("{}", root, false);
			return false; // File doesn't exists, why bother?
		}

		// Attempt to read and load the file into root
		bool parsed = false;
		s_dataLock.Enter();

		fs::ifstream doc;
		try
		{
			doc.open(Path, std::ios::in | std::ios::binary);
			if (!doc.fail()) {
				std::stringstream ss;
				ss << doc.rdbuf();
				std::string str = ss.str();
				parsed = reader.parse(str, root, false);
			}
		}
		catch (std::exception&)
		{
			_MESSAGE("Failed to load/read file...");
			parsed = false;
		}
		if (doc.is_open())
			doc.close();

		// Failed to parse file properly, init empty root
		if (!parsed || !root.isObject())
			reader.parse("{}", root, false);

		s_dataLock.Leave();
		return parsed;
	}

	bool ExternalFile::SaveFile() {
		_MESSAGE("Papyrus Saving File: %s", name.c_str());
		// Check if anything even needs saving
		if (!isModified || !root.isObject())
			return false;
		// Path to file
		fs::path Path = fs::path(docpath);
		try
		{
			if (!fs::exists(Path.parent_path()))
				fs::create_directories(Path.parent_path());
		}
		catch (const fs::filesystem_error& ex)
		{
			_MESSAGE("Failed to check or create path to file: %s", ex.what());
			return false;
		}

		// Attempt to read and load the file into root
		s_dataLock.Enter();

		fs::ofstream doc;
		try
		{
			doc.open(Path, fs::ofstream::out | fs::ofstream::trunc);
			if (!doc.fail()) {
				Json::StyledStreamWriter writer;
				writer.write(doc, root);
				isModified = false;
			}
		}
		catch (const std::exception& ex)
		{
			_MESSAGE("Failed to load/read file: %s", ex.what());
		}

		// Close file if it's still open
		if (doc.is_open()) doc.close();

		s_dataLock.Leave();
		return true;
	}


	bool ExternalFile::SaveFile(bool styled) {
		minify = styled;
		return SaveFile();
	}

	void ExternalFile::ClearAll() {
		s_dataLock.Enter();
		root.clear();
		isModified = true;
		s_dataLock.Leave();
	}

	void ExternalFile::RevertFile() {
		s_dataLock.Enter();
		isModified = false;
		root.clear();
		//LoadFile();
		s_dataLock.Leave();
	}

	void ExternalFile::CopyFileTo(std::string copyTo) {
		s_dataLock.Enter();
		if (s_Files == NULL)
			s_Files = new FileVector();

		if (copyTo.find(".json") == std::string::npos)
			copyTo += ".json";

		s_dataLock.Leave();
	}



}
