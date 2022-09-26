#pragma once

#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>
//#include <boost/algorithm/string/replace.hpp>

#include "common/ICriticalSection.h"

#include "skse64/PluginAPI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameData.h"
#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusArgs.h"

#include "Forms.h"


inline SInt32 cast(SInt32 &v) { return v; }
inline float cast(float &v) { return v; }
inline std::string cast(BSFixedString &v) { return v.data; }
inline BSFixedString cast(std::string &v) { return BSFixedString(v.c_str()); }
inline UInt32 cast(TESForm* v) { return v == NULL ? 0 : v->formID; }
inline TESForm* cast(UInt32 &v) { return v == 0 ? NULL : LookupFormByID(v); }

namespace Data
{
	void InitLists();

	template <typename S> inline void EncodeValue(S &v) {}
	inline void EncodeValue(std::string &v) {
		if (v.empty()) v += (char)0x1B;
		else std::replace(
			v.begin(),
			v.end(),
			(char)' ',
			(char)0x7
		);
	}
	//inline void EncodeValue(std::string &v) { if (v.empty()) v = (char)0x1B; else boost::replace_all(v, ' ', (char)0x7); }

	template <typename S> inline void DecodeValue(S &v) {}
	inline void DecodeValue(UInt32 &v){ v = Forms::ResolveFormID(v); }
	inline void DecodeValue(std::string &v) {
		if (v.size() == 1 && v[0] == (char)0x1B) v.clear();
		else std::replace(
			v.begin(),
			v.end(),
			(char)0x7,
			(char)' '
		);
	}
	//inline void DecodeValue(std::string &v) { if (v.size() == 1 && v[0] == (char)0x1B) v.clear(); else boost::replace_all(v, (char)0x7, ' '); }


	/*
	* Decalare classes methods
	*/

	template <typename T, typename S>
	class Values {
	public:
		ICriticalSection s_dataLock;

		typedef boost::container::flat_map<std::string, S> Obj;
		typedef boost::container::flat_map<UInt64, Obj> Map;
		Map Data;

		void SetValue(UInt64 obj, std::string key, T value);
		T GetValue(UInt64 obj, std::string key, T value);
		T AdjustValue(UInt64 obj, std::string key, T value);
		T PluckValue(UInt64 obj, std::string key, T value);
		bool UnsetValue(UInt64 obj, std::string key);
		bool HasValue(UInt64 obj, std::string key);
		int ClearPrefixKey(std::string prefix);
		int CountPrefixKey(std::string prefix);
		int ClearPrefixKey(UInt64 obj, std::string prefix);
		int CountPrefixKey(UInt64 obj, std::string prefix);

		// Serialization
		void Revert();
		void LoadStream(std::stringstream &ss);
		void SaveStream(std::stringstream &ss);

		// Debug
		inline int GetObjCount() { return Data.size(); }
		inline int GetKeyCount(UInt64 obj) { return Data.find(obj) != Data.end() ? Data[obj].size() : 0; }
		TESForm* GetNthObj(UInt32 i);
		const std::string GetNthKey(UInt64 obj, UInt32 i);
		VMResultArray<TESForm*> GetAllObj();
		VMResultArray<BSFixedString> GetAllObjKeys(UInt64 obj);
		void RemoveForm(UInt64 &obj);
		int Cleanup();
	};

	template <typename T, typename S>
	class Lists {
	public:
		ICriticalSection s_dataLock;

		typedef std::vector<S> List;
		typedef boost::container::flat_map<std::string, List> Obj;
		typedef boost::container::flat_map<UInt64, Obj> Map;
		Map Data;

		int ListAdd(UInt64 obj, std::string key, T value, bool allowDuplicate);
		T ListGet(UInt64 obj, std::string key, UInt32 index);
		T ListSet(UInt64 obj, std::string key, UInt32 index, T value);
		T ListAdjust(UInt64 obj, std::string key, UInt32 index, T value);
		T ListPluck(UInt64 obj, std::string key, UInt32 index, T value);
		T ListShift(UInt64 obj, std::string key);
		T ListPop(UInt64 obj, std::string key);

		int ListRemove(UInt64 obj, std::string key, T value, bool allInstances);
		bool ListInsertAt(UInt64 obj, std::string key, UInt32 index, T value);
		bool ListRemoveAt(UInt64 obj, std::string key, UInt32 index);
		int ListClear(UInt64 obj, std::string key);
		int ListCount(UInt64 obj, std::string key);
		int ListCountValue(UInt64 obj, std::string key, T value, bool exclude);
		int ListFind(UInt64 obj, std::string key, T value);
		bool ListHas(UInt64 obj, std::string key, T value);
		void ListSort(UInt64 obj, std::string key);
		void ListSlice(UInt64 obj, std::string key, VMArray<T> Output, UInt32 startIndex);
		int ListResize(UInt64 obj, std::string key, UInt32 length, T filler);
		bool ListCopy(UInt64 obj, std::string key, VMArray<T> Input);
		VMResultArray<T> ToArray(UInt64 obj, std::string key);
		VMResultArray<T> FilterByTypes(UInt64 obj, std::string key, VMArray<UInt32> types, bool matching);
		int ClearPrefixKey(std::string prefix);
		int CountPrefixKey(std::string prefix);
		int ClearPrefixKey(UInt64 obj, std::string prefix);
		int CountPrefixKey(UInt64 obj, std::string prefix);

		T ListRandom(UInt64 obj, std::string key);
		VMResultArray<T> ListRandomArray(UInt64 obj, std::string key, UInt32 count, bool duplicates);


		List* GetVector(UInt64 &obj, std::string &key){
			Map::iterator itr = Data.find(obj);
			if (itr == Data.end())
				return NULL;
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 == itr->second.end())
				return NULL;
			return &itr2->second;
		}

		// Serialization
		void Revert();
		void LoadStream(std::stringstream &ss);
		void SaveStream(std::stringstream &ss);

		// Debug
		inline int GetObjCount() { return Data.size(); }
		inline int GetKeyCount(UInt64 obj) { return Data.find(obj) != Data.end() ? Data[obj].size() : 0; }
		TESForm* GetNthObj(UInt32 i);
		const std::string GetNthKey(UInt64 obj, UInt32 i);
		VMResultArray<TESForm*> GetAllObj();
		VMResultArray<BSFixedString> GetAllObjKeys(UInt64 obj);
		void RemoveForm(UInt64 &obj);
		int Cleanup();
	};

	/*
	* Data storage holders
	*/
	// TODO: Change storage method
	typedef Values<TESForm*, std::pair<UInt32, std::string>> formv;
	typedef Lists<TESForm*, std::pair<UInt32, std::string>> forml;

	typedef Values<SInt32, SInt32> intv;
	typedef Values<float, float> flov;
	typedef Values<BSFixedString, std::string> strv;
	typedef Values<TESForm*, UInt32> forv;

	template <typename T, typename S> Values<T, S>* GetValues();
	template <> intv* GetValues<SInt32, SInt32>();
	template <> flov* GetValues<float, float>();
	template <> strv* GetValues<BSFixedString, std::string>();
	template <> forv* GetValues<TESForm*, UInt32>();

	typedef Lists<SInt32, SInt32> intl;
	typedef Lists<float, float> flol;
	typedef Lists<BSFixedString, std::string> strl;
	typedef Lists<TESForm*, UInt32> forl;

	template <typename T, typename S> Lists<T, S>* GetLists();
	template <> intl* GetLists<SInt32, SInt32>();
	template <> flol* GetLists<float, float>();
	template <> strl* GetLists<BSFixedString, std::string>();
	template <> forl* GetLists<TESForm*, UInt32>();

	//forl* GetPackages();
	//aniv* GetAnimations();
}