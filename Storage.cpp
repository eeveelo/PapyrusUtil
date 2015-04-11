#include "Storage.h"

namespace Storage
{

	static inline char* copystr(const char* value){
		unsigned int length = (unsigned int)strlen(value);
		char* newString = static_cast<char*>(malloc(length + 1));
		memcpy(newString, value, length);
		newString[length] = 0;
		return newString;
	}

	Value::Value(ValueType varType = nullValue){
		type = varType;
		switch (varType){
		case nullValue:
			break;
		case intValue:
			value._int = 0;
			break;
		case floatValue:
			value._float = 0.0f;
			break;
		case stringValue:
			value._string = 0;
			break;
		case formValue:
			value._form = 0;
			break;
		}
	}
	Value::Value(SInt32 var){
		type = intValue;
		value._int = var;
	}
	Value::Value(float var){
		type = floatValue;
		value._float = var;
	}
	Value::Value(BSFixedString var){
		type = stringValue;
		value._string = copystr(var.data);
	}
	Value::Value(TESForm* var){
		type = formValue;
		if (!var) value._form = 0;
		else{
			std::string formstr = Forms::GetFormString(var);
			value._form = copystr(formstr.c_str());
		}
	}

	SInt32 Value::asInt(){
		return type == intValue ? value._int : 0;
	}
	float Value::asFloat(){
		return type == floatValue ? value._float : 0.0f;
	}
	BSFixedString Value::asString(){
		return type == stringValue ? BSFixedString(value._string) : BSFixedString("");
	}
	TESForm* Value::asForm(){
		std::string formstr = value._form;
		return type == formValue ? Forms::ParseFormString(formstr) : NULL;
	}

	template <> inline SInt32 Value::Decode<SInt32>(){ return asInt(); }
	template <> inline float Value::Decode<float>(){ return asFloat(); }
	template <> inline BSFixedString Value::Decode<BSFixedString>(){ return asString(); }
	template <> inline TESForm* Value::Decode<TESForm*>(){ return asForm(); }


	template <typename T>
	void Values<T>::SetValue(UInt64 obj, std::string key, T value){
		s_dataLock.Enter();
		boost::to_lower(key);
		Data[obj][key] = Value(value);
		s_dataLock.Leave();
		return value;
	}

	template <typename T>
	T Values<T>::GetValue(UInt64 obj, std::string key, T value){
		s_dataLock.Enter();
		Map::iterator itr = Data.find(obj);
		if (itr != Data.end()){
			boost::to_lower(key);
			Obj::iterator itr2 = itr->second.find(key);
			if (itr2 != itr->second.end())
				value = itr2->second.Decode<T>();
		}
		s_dataLock.Leave();
		return value;
	}


}