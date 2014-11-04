#include "External.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
//#include <direct.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "skse/PapyrusVM.h"
#include "skse/PapyrusNativeFunctions.h"

namespace External {
	namespace fs = boost::filesystem;
	ICriticalSection* s_loadLock;

	FileVector* s_Files;
	ExternalFile* GetFile(std::string name) {
		if (s_loadLock == NULL)
			s_loadLock = new ICriticalSection();
		s_loadLock->Enter();

		if (s_Files == NULL)
			s_Files = new FileVector();
		
		if (name.find(".json") == std::string::npos)
			name += ".json";
		
		for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr){
			if (boost::iequals(name, (*itr)->name)){ s_loadLock->Leave(); return (*itr); }
		}

		ExternalFile* File = new ExternalFile(name);
		s_Files->push_back(File);

		s_loadLock->Leave();
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

	bool ExternalFile::ImportOnForm(TESForm* FormRef) {
		return false;
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
		setlocale(LC_NUMERIC, "POSIX");
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
			doc.open(Path, fs::ifstream::in | std::ios_base::binary); //
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

	Value ExternalFile::SetValue(std::string type, std::string key, Value value) {
		s_dataLock.Enter();
		boost::to_lower(key);
		root[type][key] = value;
		isModified = true;
		s_dataLock.Leave();
		return value;
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
			if (root[type].empty())
				root.removeMember(type);
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
			Value::iterator itr = root[type][key].begin();
			for (itr = root[type][key].begin(); itr != root[type][key].end(); ++itr){
				if (!allInstances && removed > 0) list.append((*itr));
				else if (removing != (*itr)) list.append((*itr));
				else removed += 1;
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
			if (root[type].size() == 0)
				root.removeMember(type);
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

	int ExternalFile::ListFind(std::string type, std::string key, Value value){
		int index = -1;
		s_dataLock.Enter();

		boost::to_lower(key);
		if (HasKey(type, key)){
			Value::iterator itr = root[type][key].begin();
			for (itr = root[type][key].begin(); itr != root[type][key].end(); ++itr) {
				if (value == (*itr)) { index = itr.key().asInt(); break;	}
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
		if (Output.Length() < 1) return;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		UInt32 length = Output.Length();
		if (HasKey(type, key)){
			Value::iterator itr = root[type][key].begin();
			std::advance(itr, startIndex);
			for (int index = 0; index < length && itr != root[type][key].end(); ++itr, ++index){
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
	bool ExternalFile::ListCopy(std::string key, VMArray<T> Input) {
		if (Input.Length() < 1) return false;
		s_dataLock.Enter();

		T var;
		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key)) root[type][key].clear();
		for (UInt32 i = 0; i < Input.Length(); ++i) {
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
	
}
