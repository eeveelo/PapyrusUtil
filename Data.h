#pragma once

#include "skse/PluginAPI.h"
//#include "skse/GameAPI.h"
#include "skse/GameTypes.h"
#include "skse/GameData.h"
#include "skse/PapyrusVM.h"
#include "skse/PapyrusArgs.h"
//#include "Forms.h"
#include "common/ICriticalSection.h"

#include <boost/unordered_map.hpp>
#include <sstream>
#include <vector>

inline SInt32 cast(SInt32 &v) { return v; }
inline float cast(float &v) { return v; }
inline std::string cast(BSFixedString &v) { return v.data; }
inline BSFixedString cast(std::string &v) { return BSFixedString(v.c_str()); }
inline UInt32 cast(TESForm* v) { return v == NULL ? 0 : v->formID; }
inline TESForm* cast(UInt32 &v) { return v == 0 ? NULL : LookupFormByID(v); }

namespace Data {

	void InitLists();


	/*
	*
	* Decalare classes methods
	*
	*/

	template <typename T, typename S>
	class Values {
	public:
		ICriticalSection s_dataLock;

		typedef boost::unordered_map<std::string, S> Obj;
		typedef boost::unordered_map<UInt64, Obj> Map;
		Map Data;

		T SetValue(UInt64 obj, std::string key, T value);
		T GetValue(UInt64 obj, std::string key, T value);
		T AdjustValue(UInt64 obj, std::string key, T value);
		bool UnsetValue(UInt64 obj, std::string key);
		bool HasValue(UInt64 obj, std::string key);

		// Serialization
		void Revert();
		void LoadStream(std::stringstream &ss);
		void SaveStream(std::stringstream &ss);

		// Debug
		inline int GetObjCount() { return Data.size(); }
		inline int GetKeyCount(UInt64 obj) { return Data.find(obj) != Data.end() ? Data[obj].size() : 0; }
		TESForm* GetNthObj(UInt32 i);
		std::string GetNthKey(UInt64 obj, UInt32 i);
		void RemoveForm(UInt64 &obj);
		int Cleanup();
	};

	template <typename T, typename S>
	class Lists {
	public:
		ICriticalSection s_dataLock;

		typedef std::vector<S> List;
		typedef boost::unordered_map<std::string, List> Obj;
		typedef boost::unordered_map<UInt64, Obj> Map;
		Map Data;

		int ListAdd(UInt64 obj, std::string key, T value, bool allowDuplicate);
		T ListGet(UInt64 obj, std::string key, UInt32 index);
		T ListSet(UInt64 obj, std::string key, UInt32 index, T value);
		T ListAdjust(UInt64 obj, std::string key, UInt32 index, T value);
		int ListRemove(UInt64 obj, std::string key, T value, bool allInstances);
		bool ListInsertAt(UInt64 obj, std::string key, UInt32 index, T value);
		bool ListRemoveAt(UInt64 obj, std::string key, UInt32 index);
		int ListClear(UInt64 obj, std::string key);
		int ListCount(UInt64 obj, std::string key);
		int ListFind(UInt64 obj, std::string key, T value);
		bool ListHas(UInt64 obj, std::string key, T value);
		void ListSort(UInt64 obj, std::string key);
		void ListSlice(UInt64 obj, std::string key, VMArray<T> Output, UInt32 startIndex);
		int ListResize(UInt64 obj, std::string key, UInt32 length, T filler);
		bool ListCopy(UInt64 obj, std::string key, VMArray<T> Input);
		

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
		std::string GetNthKey(UInt64 obj, UInt32 i);
		void RemoveForm(UInt64 &obj);
		int Cleanup();

	};

	/*
	*
	* Data storage holders
	*
	*/

	typedef Values<SInt32, SInt32> intv;
	typedef Values<float, float> flov;
	typedef Values<BSFixedString, std::string> strv;
	typedef Values<TESForm*, UInt32> forv;

	template <typename T, typename S>
	Values<T, S>* GetValues();
	template <> intv* GetValues<SInt32, SInt32>();
	template <> flov* GetValues<float, float>();
	template <> strv* GetValues<BSFixedString, std::string>();
	template <> forv* GetValues<TESForm*, UInt32>();

	typedef Lists<SInt32, SInt32> intl;
	typedef Lists<float, float> flol;
	typedef Lists<BSFixedString, std::string> strl;
	typedef Lists<TESForm*, UInt32> forl;

	template <typename T, typename S>
	Lists<T, S>* GetLists();
	template <> intl* GetLists<SInt32, SInt32>();
	template <> flol* GetLists<float, float>();
	template <> strl* GetLists<BSFixedString, std::string>();
	template <> forl* GetLists<TESForm*, UInt32>();

	forl* GetPackages();
	//aniv* GetAnimations();
}



