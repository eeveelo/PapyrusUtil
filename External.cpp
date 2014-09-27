#include "External.h"

#include "skse/PapyrusVM.h"
#include "skse/PapyrusNativeFunctions.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
//#include <direct.h>

namespace External {
	namespace fs = boost::filesystem;

	FileVector* s_Files;
	ExternalFile* GetFile(std::string name) {
		if (s_Files == NULL)
			s_Files = new FileVector();

		if (name.find(".json") == std::string::npos)
			name += ".json";
		
		for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr)
			if (boost::iequals(name, (*itr)->name))	return (*itr);

		ExternalFile* File = new ExternalFile(name);
		s_Files->push_back(File);

		return File;
	}

	ExternalFile* s_Global;
	ExternalFile* GetSingleton(){
		if (s_Global == NULL)
			s_Global = new ExternalFile("..\\StorageUtil.json");
		return s_Global;
	}

	bool LoadGlobalFile() {
		if (s_Global == NULL)
			s_Global = new ExternalFile("..\\StorageUtil.json");
		return s_Global != NULL;
	}

	bool SaveGlobalFile() {
		if (s_Global == NULL)
			return false;
		return s_Global->SaveFile();
	}

	void SaveFiles() {
		if (s_Global != NULL && s_Global->isModified)
			s_Global->SaveFile(false);
		if (s_Files != NULL && !s_Files->empty()){
			for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr) {
				if ((*itr)->isModified)
					(*itr)->SaveFile();
			}
		}
	}

	void RevertFiles() {
		if (s_Global != NULL && s_Global->isModified)
			s_Global->RevertFile();
		if (s_Files != NULL && !s_Files->empty()){
			for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr) {
				if ((*itr)->isModified)
					(*itr)->RevertFile();
			}
		}
	}

	/*/
	/// Class: ExternalFile
	/*/

	// read/write
	inline fs::path GetPath(std::string &name){
		std::string docpath = "Data\\SKSE\\Plugins\\StorageUtilData\\" + name;
		return fs::path(docpath);
	}

	bool ExternalFile::LoadFile(){
		_MESSAGE("Papyrus Loading File: %s", name.c_str());

		// Path to file
		fs::path Path = GetPath(name);

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
			doc.open(Path, fs::ifstream::in); //| std::ios_base::binary
			if (!doc.fail()) parsed = reader.parse(doc, root, true);
		}
		catch (std::exception&)
		{
			_MESSAGE("Failed to load/read file...");
			parsed = false;
		}
		if (doc.is_open()) doc.close();

		// Failed to parse file properly, init empty root
		if (!parsed || !root.isObject()) reader.parse("{}", root, false);

		s_dataLock.Leave();
		return parsed;
	}

	bool ExternalFile::SaveFile(){
		_MESSAGE("Papyrus Saving File: %s", name.c_str());

		// Check if anything even needs saving
		if (!isModified || !root.isObject()) return false;

		// Path to file
		fs::path Path = GetPath(name);
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
		styledWrite = styled;
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
		LoadFile();
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

	// Global key=>value
	template <typename T>
	T ExternalFile::SetValue(std::string key, T value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		root[Type<T>()][key] = MakeValue<T>(value);
		isModified = true;
		s_dataLock.Leave();
		return value;
	}
	template SInt32 ExternalFile::SetValue<SInt32>(std::string key, SInt32 value);
	template float ExternalFile::SetValue<float>(std::string key, float value);
	template BSFixedString ExternalFile::SetValue<BSFixedString>(std::string key, BSFixedString value);
	template TESForm* ExternalFile::SetValue<TESForm*>(std::string key, TESForm* value);

	template <typename T>
	T ExternalFile::GetValue(std::string key, T missing) {
		s_dataLock.Enter();
		boost::to_lower(key);
		Value value = Value::null;
		if (HasKey(Type<T>(), key))
			value = root[Type<T>()][key];
		s_dataLock.Leave();
		return ParseValue<T>(value, missing);
	}
	template SInt32 ExternalFile::GetValue<SInt32>(std::string key, SInt32 missing);
	template float ExternalFile::GetValue<float>(std::string key, float missing);
	template BSFixedString ExternalFile::GetValue<BSFixedString>(std::string key, BSFixedString missing);
	template TESForm* ExternalFile::GetValue<TESForm*>(std::string key, TESForm* missing);

	template <typename T>
	T ExternalFile::AdjustValue(std::string key, T value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		std::string type = Type<T>();
		if (HasKey(type, key))
			value = value + ParseValue(root[type][key], T());
		root[type][key] = MakeValue<T>(value);
		isModified = true;
		s_dataLock.Leave();
		return value;
	}
	template SInt32 ExternalFile::AdjustValue<SInt32>(std::string key, SInt32 value);
	template float ExternalFile::AdjustValue<float>(std::string key, float value);

	bool ExternalFile::UnsetValue(std::string type, std::string &key){
		bool removed = false;
		s_dataLock.Enter();

		boost::to_lower(key);
		if (HasKey(type, key)){
			isModified = true;
			removed = true;
			root[type].removeMember(key);
			if (root[type].empty())
				root.removeMember(type);
		}

		s_dataLock.Leave();
		return removed;
	}

	bool ExternalFile::HasValue(std::string type, std::string &key){
		bool has = false;
		s_dataLock.Enter();
		boost::to_lower(key);
		has = root.isMember(type) && root[type].isMember(key);
		s_dataLock.Leave();
		return has;
	}

	// Global key=>value=>list
	template <typename T>
	int ExternalFile::ListAdd(std::string key, T value, bool allowDuplicate){
		if (!allowDuplicate && ListHas(key, value))
			return -1;
		s_dataLock.Enter();

		std::string type = List<T>();
		if (root.get(type, Value::null).isNull())
			root[type] = Value(Json::objectValue);

		boost::to_lower(key);
		if (root[type].get(key, Value::null).isNull())
			root[type][key] = Value(Json::arrayValue);

		int index = root[type][key].size();
		Value &v = root[type][key].append(MakeValue<T>(value));
		isModified = true;

		s_dataLock.Leave();
		return index;
	}
	template int ExternalFile::ListAdd<SInt32>(std::string key, SInt32 value, bool allowDuplicate);
	template int ExternalFile::ListAdd<float>(std::string key, float value, bool allowDuplicate);
	template int ExternalFile::ListAdd<BSFixedString>(std::string key, BSFixedString value, bool allowDuplicate);
	template int ExternalFile::ListAdd<TESForm*>(std::string key, TESForm* value, bool allowDuplicate);

	template <typename T>
	T ExternalFile::ListGet(std::string key, int index){
		s_dataLock.Enter();
		boost::to_lower(key);
		std::string type = List<T>();
		Value value;
		if (HasKey(type, key) && root[type][key].isValidIndex(index))
			value = root[type][key].get(index, Value::null);
		s_dataLock.Leave();
		return ParseValue<T>(value);
	}
	template SInt32 ExternalFile::ListGet<SInt32>(std::string key, int index);
	template float ExternalFile::ListGet<float>(std::string key, int index);
	template BSFixedString ExternalFile::ListGet<BSFixedString>(std::string key, int index);
	template TESForm* ExternalFile::ListGet<TESForm*>(std::string key, int index);

	template <typename T>
	T ExternalFile::ListSet(std::string key, int index, T value){
		s_dataLock.Enter();
		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key) && root[type][key].isValidIndex(index)){
			root[type][key][index] = MakeValue<T>(value);
			isModified = true;
		}
		s_dataLock.Leave();
		return value;
	}
	template SInt32 ExternalFile::ListSet<SInt32>(std::string key, int index, SInt32 value);
	template float ExternalFile::ListSet<float>(std::string key, int index, float value);
	template BSFixedString ExternalFile::ListSet<BSFixedString>(std::string key, int index, BSFixedString value);
	template TESForm* ExternalFile::ListSet<TESForm*>(std::string key, int index, TESForm* value);

	template <typename T>
	T ExternalFile::ListAdjust(std::string key, int index, T value){
		s_dataLock.Enter();
		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key) && root[type][key].isValidIndex(index)){
			value = value + ParseValue(root[type][key][index], T());
			root[type][key][index] = MakeValue<T>(value);
			isModified = true;
		}
		s_dataLock.Leave();
		return value;
	}
	template SInt32 ExternalFile::ListAdjust<SInt32>(std::string key, int index, SInt32 value);
	template float ExternalFile::ListAdjust<float>(std::string key, int index, float value);

	template <typename T>
	int ExternalFile::ListRemove(std::string key, T value, bool allInstances){
		int removed = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key)){
			Value list = Value(Json::arrayValue);
			Value removing = MakeValue<T>(value);
			Value::iterator itr = root[type][key].begin();
			if (allInstances) {
				for (itr = root[type][key].begin(); itr != root[type][key].end(); itr++){
					if (removing != (*itr))
						list.append((*itr));
				}
				removed = root[type][key].size() - list.size();
			}
			else {
				for (itr = root[type][key].begin(); itr != root[type][key].end(); itr++){
					if (removed < 1 && removing == (*itr))
						removed = 1;
					else
						list.append((*itr));
				}
			}
			root[type][key] = list;
			if (root[type][key].size() == 0){
				root[type].removeMember(key);
				if (root[type].size() == 0)
					root.removeMember(type);
			}
			isModified = true;
		}

		s_dataLock.Leave();
		return removed;
	}
	template int ExternalFile::ListRemove<SInt32>(std::string key, SInt32 value, bool allInstances);
	template int ExternalFile::ListRemove<float>(std::string key, float value, bool allInstances);
	template int ExternalFile::ListRemove<TESForm*>(std::string key, TESForm* value, bool allInstances);

	template <typename T>
	bool ExternalFile::ListRemoveAt(std::string key, int index){
		bool removed = false;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key) && root[type][key].isValidIndex(index)){
			Value list = Value(Json::arrayValue);
			Value::iterator itr = root[type][key].begin();
			for (itr = root[type][key].begin(); itr != root[type][key].end(); itr++){
				if (itr.key().asInt() == index)
					removed = true;
				else
					list.append((*itr));
			}
			root[type][key] = list;
			if (root[type][key].size() == 0){
				root[type].removeMember(key);
				if (root[type].size() == 0)
					root.removeMember(type);
			}
			isModified = true;
		}
		s_dataLock.Leave();
		return removed;
	}
	template bool ExternalFile::ListRemoveAt<SInt32>(std::string key, int index);
	template bool ExternalFile::ListRemoveAt<float>(std::string key, int index);
	template bool ExternalFile::ListRemoveAt<BSFixedString>(std::string key, int index);
	template bool ExternalFile::ListRemoveAt<TESForm*>(std::string key, int index);

	template <typename T>
	bool ExternalFile::ListInsertAt(std::string key, int index, T value){
		bool inserted = false;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key) && root[type][key].isValidIndex(index)){
			Value find = MakeValue<T>(value);
			Value list = Value(Json::arrayValue);
			Value::iterator itr = root[type][key].begin();
			for (itr = root[type][key].begin(); itr != root[type][key].end(); itr++){
				if (itr.key().asInt() == index){
					list.append(MakeValue<T>(value));
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
	template bool ExternalFile::ListInsertAt<SInt32>(std::string key, int index, SInt32 value);
	template bool ExternalFile::ListInsertAt<float>(std::string key, int index, float value);
	template bool ExternalFile::ListInsertAt<BSFixedString>(std::string key, int index, BSFixedString value);
	template bool ExternalFile::ListInsertAt<TESForm*>(std::string key, int index, TESForm* value);

	template <typename T>
	int ExternalFile::ListClear(std::string key){
		int cleared = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key)){
			cleared = root[type][key].size();
			root[type].removeMember(key);
			if (root[type].size() == 0)
				root.removeMember(type);
			isModified = true;
		}

		s_dataLock.Leave();
		return cleared;
	}
	template int ExternalFile::ListClear<SInt32>(std::string key);
	template int ExternalFile::ListClear<float>(std::string key);
	template int ExternalFile::ListClear<BSFixedString>(std::string key);
	template int ExternalFile::ListClear<TESForm*>(std::string key);

	template <typename T>
	int ExternalFile::ListCount(std::string key){
		int count = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key))
			count = root[type][key].size();

		s_dataLock.Leave();
		return count;
	}
	template int ExternalFile::ListCount<SInt32>(std::string key);
	template int ExternalFile::ListCount<float>(std::string key);
	template int ExternalFile::ListCount<BSFixedString>(std::string key);
	template int ExternalFile::ListCount<TESForm*>(std::string key);

	template <typename T>
	int ExternalFile::ListFind(std::string key, T value){
		int index = -1;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key)){
			Value::iterator itr = std::find(root[type][key].begin(), root[type][key].end(), MakeValue<T>(value));
			if (itr != root[type][key].end())
				index = itr.key().asInt();
		}

		s_dataLock.Leave();
		return index;
	}
	template int ExternalFile::ListFind<SInt32>(std::string key, SInt32 value);
	template int ExternalFile::ListFind<float>(std::string key, float value);
	template int ExternalFile::ListFind<TESForm*>(std::string key, TESForm* value);

	template <typename T>
	bool ExternalFile::ListHas(std::string key, T value){
		bool has = false;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key))
			has = (std::find(root[type][key].begin(), root[type][key].end(), MakeValue<T>(value)) != root[type][key].end());

		s_dataLock.Leave();
		return has;
	}
	template bool ExternalFile::ListHas<SInt32>(std::string key, SInt32 value);
	template bool ExternalFile::ListHas<float>(std::string key, float value);
	template bool ExternalFile::ListHas<TESForm*>(std::string key, TESForm* value);

	template <typename T>
	void ExternalFile::ListSlice(std::string key, VMArray<T> Output, int startIndex){
		if (Output.Length() < 1)
			return;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key) && startIndex < root[type][key].size()){
			Value::iterator itr = root[type][key].begin();
			std::advance(itr, startIndex);
			for (int index = 0; index < Output.Length() && itr != root[type][key].end(); ++itr, ++index){
				T value = ParseValue<T>(root[type][key].get(itr.index(), Value::null));
				Output.Set(&value, index);
			}
		}

		s_dataLock.Leave();
	}
	template void ExternalFile::ListSlice<SInt32>(std::string key, VMArray<SInt32> Output, int startIndex);
	template void ExternalFile::ListSlice<float>(std::string key, VMArray<float> Output, int startIndex);
	template void ExternalFile::ListSlice<BSFixedString>(std::string key, VMArray<BSFixedString> Output, int startIndex);
	template void ExternalFile::ListSlice<TESForm*>(std::string key, VMArray<TESForm*> Output, int startIndex);

	template <typename T>
	int ExternalFile::ListResize(std::string key, int length, T filler) {
		if (length == 0)
			return ListClear<T>(key) * -1;
		s_dataLock.Enter();

		int start = 0;
		Value value = MakeValue<T>(filler);
		std::string type = List<T>();
		boost::to_lower(key);
		if (HasKey(type, key)){
			start = root[type][key].size();
			if (length < start)
				root[type][key].resize(length);
			else {
				for (int i = 0; length > root[type][key].size() && i < 500; ++i)
					root[type][key].append(value);
			}
		}
		else {
			for (int i = 0; length > root[type][key].size() && i < 500; ++i)
				root[type][key].append(value);
		}
		isModified = true;

		s_dataLock.Leave();
		return root[type][key].size() - start;
	}
	template int ExternalFile::ListResize<SInt32>(std::string key, int length, SInt32 filler);
	template int ExternalFile::ListResize<float>(std::string key, int length, float filler);
	template int ExternalFile::ListResize<BSFixedString>(std::string key, int length, BSFixedString filler);
	template int ExternalFile::ListResize<TESForm*>(std::string key, int length, TESForm* filler);

	template <typename T>
	bool ExternalFile::ListCopy(std::string key, VMArray<T> Input) {
		if (Input.Length() < 1)
			return false;
		s_dataLock.Enter();

		std::string type = List<T>();
		boost::to_lower(key);
		if (HasKey(type, key))
			root[type][key].clear();
		for (UInt32 i = 0; i < Input.Length(); ++i) {
			T var;
			Input.Get(&var, i);
			root[type][key].append(MakeValue<T>(var));
		}
		isModified = true;

		s_dataLock.Leave();
		return true;
	}
	template bool ExternalFile::ListCopy<SInt32>(std::string key, VMArray<SInt32> Input);
	template bool ExternalFile::ListCopy<float>(std::string key, VMArray<float> Input);
	template bool ExternalFile::ListCopy<BSFixedString>(std::string key, VMArray<BSFixedString> Input);
	template bool ExternalFile::ListCopy<TESForm*>(std::string key, VMArray<TESForm*> Input);

	// special case for strings
	int ExternalFile::ListFind(std::string key, BSFixedString value){
		int index = -1;
		s_dataLock.Enter();
		boost::to_lower(key);
		std::string type = "stringList";
		if (HasKey(type, key)){
			std::string var = value.data;
			for (Value::iterator itr = root[type][key].begin(); itr != root[type][key].end(); ++itr){
				std::string str = root[type][key].get(itr.index(), Value::null).asString();
				if (boost::iequals(var, str)) {
					index = itr.key().asInt();
					break;
				}
			}
		}
		s_dataLock.Leave();
		return index;
	}

	bool ExternalFile::ListHas(std::string key, BSFixedString value){
		return ListFind(key, value) != -1;
	}

	/*int ExternalFile::ListAdd(std::string key, BSFixedString value, bool allowDuplicate){
	if (!allowDuplicate && ListFind(key, value) != -1);
	return -1;
	s_dataLock.Enter();

	boost::to_lower(key);
	std::string type = "stringList";
	int index = root[type][key].size();
	Value &v = root[type][key].append(MakeValue<BSFixedString>(value));
	isModified = true;

	s_dataLock.Leave();
	return index;
	}*/

	int ExternalFile::ListRemove(std::string key, BSFixedString value, bool allInstances){
		int removed = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = "stringList";
		if (HasKey(type, key)){
			std::string var = value.data;
			Value list = Value(Json::arrayValue);
			Value removing = MakeValue<BSFixedString>(value);
			Value::iterator itr = root[type][key].begin();
			for (itr = root[type][key].begin(); itr != root[type][key].end(); ++itr){
				std::string str = (*itr).asString();
				if (!boost::iequals(var, str))
					list.append((*itr));
			}
			removed = root[type][key].size() - list.size();
			root[type][key] = list;
			if (root[type][key].size() == 0){
				root[type].removeMember(key);
				if (root[type].size() == 0)
					root.removeMember(type);
			}
			isModified = true;
		}

		s_dataLock.Leave();
		return removed;
	}
	//template int ExternalFile::ListFind(std::string key, BSFixedString value);
	//template bool ExternalFile::ListHas(std::string key, BSFixedString value);
	//template int ExternalFile::ListRemove(std::string key, BSFixedString value, bool allInstances);
	//template int ExternalFile::ListAdd(std::string key, BSFixedString value, bool allowDuplicate);

}
