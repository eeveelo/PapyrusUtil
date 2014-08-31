#include "External.h"

#include "skse/PapyrusVM.h"
#include "skse/PapyrusNativeFunctions.h"

#include <boost/algorithm/string.hpp>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <direct.h>

/*
TODO: idot. You overwrote this file with the wrong paste. It's not currently reverted to v2.5.

Filepath needs to be rewritten to 2.6 level
Re-add adjust functions.

*/
namespace External {

	FileVector* s_Files;
	ExternalFile* GetFile(std::string name) {
		if (s_Files == NULL)
			s_Files = new FileVector();
		if (name.find(".") == std::string::npos)
			name += ".json";

		for (FileVector::iterator itr = s_Files->begin(); itr != s_Files->end(); ++itr){
			if (boost::iequals(name, (*itr)->name)){
				return (*itr);
			}
		}
		ExternalFile* file = new ExternalFile(name);
		s_Files->push_back(file);
		_MESSAGE("Loaded file: %s", name.c_str());
		return file;
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



	// read/write
	bool ExternalFile::LoadFile(){
		_MESSAGE("Papyrus Loading File: %s", name.c_str());
		s_dataLock.Enter();
		_mkdir("Data\\SKSE\\Plugins\\StorageUtilData");
		std::string docpath = "Data\\SKSE\\Plugins\\StorageUtilData\\" + name;
		std::ifstream doc;
		try {
			doc.open(docpath.c_str(), std::ifstream::in | std::ifstream::binary);
		}
		catch (std::exception&) {
			_MESSAGE("Catch");
			reader.parse("{}", root, true);
			doc.close();
			return false;
		}
		bool parsed = false;
		if (doc.fail()){
			_MESSAGE("Fail");
			parsed = reader.parse("{}", root, true);
		}
		else {
			_MESSAGE("Read");
			parsed = reader.parse(doc, root, true);
		}
		doc.close();
		if (!parsed || root.size() == 0){
			_MESSAGE("Empty");
			parsed = reader.parse("{}", root, true);
		}


		s_dataLock.Leave();
		return parsed;
	}

	bool ExternalFile::SaveFile(){
		_MESSAGE("Papyrus Saving File: %s", name.c_str());
		s_dataLock.Enter();
		// stringify json
		std::string out = "";
		if (styledWrite){
			Json::StyledWriter style;
			out = style.write(root);
		}
		else{
			Json::FastWriter fast;
			out = fast.write(root);
		}
		const char * cstr = out.c_str();
		// Open file
		std::string docpath = "Data\\SKSE\\Plugins\\StorageUtilData\\" + name;
		std::ofstream doc;
		try {
			doc.open(docpath.c_str());
		}
		catch (std::exception*) {
			return false;
		}
		if (doc.fail())
			return false;
		// Write to file
		doc.write(cstr, strlen(cstr));
		doc.close();
		isModified = false;
		s_dataLock.Leave();
		return true;
	}

	bool ExternalFile::SaveFile(bool styled) {
		styledWrite = styled;
		return SaveFile();
	}

	void ExternalFile::RevertFile() {
		s_dataLock.Enter();
		isModified = false;
		root.clear();
		LoadFile();
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
		Json::Value value = Json::Value::null;
		if (HasKey(Type<T>(), key))
			value = root[Type<T>()][key];
		s_dataLock.Leave();
		return ParseValue<T>(value, missing);
	}
	template SInt32 ExternalFile::GetValue<SInt32>(std::string key, SInt32 missing);
	template float ExternalFile::GetValue<float>(std::string key, float missing);
	template BSFixedString ExternalFile::GetValue<BSFixedString>(std::string key, BSFixedString missing);
	template TESForm* ExternalFile::GetValue<TESForm*>(std::string key, TESForm* missing);

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
	template <typename T>
	int ExternalFile::ListAdd(std::string key, T value, bool allowDuplicate){
		if (!allowDuplicate && ListHas(key, value))
			return -1;
		s_dataLock.Enter();

		std::string type = List<T>();
		if (root.get(type, Json::Value::null).isNull())
			root[type] = Json::Value(Json::objectValue);

		boost::to_lower(key);
		if (root[type].get(key, Json::Value::null).isNull())
			root[type][key] = Json::Value(Json::arrayValue);

		int index = root[type][key].size();
		Json::Value &v = root[type][key].append(MakeValue<T>(value));
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
		Json::Value value;
		if (HasKey(type, key) && root[type][key].isValidIndex(index))
			value = root[type][key].get(index, Json::Value::null);
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
	int ExternalFile::ListRemove(std::string key, T value, bool allInstances){
		int removed = 0;
		s_dataLock.Enter();

		boost::to_lower(key);
		std::string type = List<T>();
		if (HasKey(type, key)){
			Json::Value list = Json::Value(Json::arrayValue);
			Json::Value removing = MakeValue<T>(value);
			Json::Value::iterator itr = root[type][key].begin();
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
			Json::Value list = Json::Value(Json::arrayValue);
			Json::Value::iterator itr = root[type][key].begin();
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
			Json::Value find = MakeValue<T>(value);
			Json::Value list = Json::Value(Json::arrayValue);
			Json::Value::iterator itr = root[type][key].begin();
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
			Json::Value::iterator itr = std::find(root[type][key].begin(), root[type][key].end(), MakeValue<T>(value));
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
			Json::Value::iterator itr = root[type][key].begin();
			std::advance(itr, startIndex);
			for (int index = 0; index < Output.Length() && itr != root[type][key].end(); ++itr, ++index){
				T value = ParseValue<T>(root[type][key].get(itr.index(), Json::Value::null));
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
		Json::Value value = MakeValue<T>(filler);
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
			for (Json::Value::iterator itr = root[type][key].begin(); itr != root[type][key].end(); ++itr){
				std::string str = root[type][key].get(itr.index(), Json::Value::null).asString();
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
	Json::Value &v = root[type][key].append(MakeValue<BSFixedString>(value));
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
			Json::Value list = Json::Value(Json::arrayValue);
			Json::Value removing = MakeValue<BSFixedString>(value);
			Json::Value::iterator itr = root[type][key].begin();
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