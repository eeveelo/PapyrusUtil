#pragma once

#include <sstream>
#include <vector>
#include <boost/container/flat_map.hpp>

#include "common/ICriticalSection.h"

#include "skse/PluginAPI.h"
#include "skse/GameTypes.h"
#include "skse/GameData.h"
#include "skse/PapyrusVM.h"
#include "skse/PapyrusArgs.h"

#include "Forms.h"



namespace Storage
{
	enum ValueType {
		nullValue = 0,
		intValue,
		floatValue,
		stringValue,
		formValue
	};

	class Value{
		union ValueHolder {
			SInt32    _int;
			float  _float;
			char*  _string;
			char*  _form;
		} value;
		ValueType type : 4;

		Value(ValueType type = nullValue);
		Value(SInt32 var);
		Value(float var);
		Value(BSFixedString var);
		Value(TESForm* var);


		SInt32 asInt();
		float asFloat();
		BSFixedString asString();
		TESForm* asForm();
		template <typename T> inline T Decode();
	};

	
	/*
	* Decalare classes methods
	*/

	template <typename T>
	class Values {
	public:
				
		ICriticalSection s_dataLock;
		typedef boost::container::flat_map<std::string, Value> Obj;
		typedef boost::container::flat_map<UInt64, Obj> Map;

		void SetValue(UInt64 obj, std::string key, T value);
		T GetValue(UInt64 obj, std::string key, T value);
		
		/*
		typedef boost::container::flat_map<std::string, S> Obj;
		typedef boost::container::flat_map<UInt64, Obj> Map;
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
		*/
	};

}