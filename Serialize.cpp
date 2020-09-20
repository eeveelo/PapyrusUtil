#include "Serialize.h"

#include <sstream>

#include "skse64/GameAPI.h"
#include "skse64/PluginAPI.h"
#include "skse64/GameTypes.h"

#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusArgs.h"

#include "Forms.h"
#include "Data.h"
#include "External.h"
#include "PackageData.h"

//#include <fstream>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>


namespace Data {

	const UInt32 kSerializationDataVersion = 1;

	// Flat obj=>key=>value storage
	extern intv* intValues;
	extern flov* floatValues;
	extern strv* stringValues;
	extern forv* formValues;

	// Vector obj=>key=>vector[i] storage
	extern intl* intLists;
	extern flol* floatLists;
	extern strl* stringLists;
	extern forl* formLists;

	// Overrides
	extern forl* packageLists;
	//extern aniv* animValues;

	template <class T> void Load(T *Data, SKSESerializationInterface *intfc, UInt32 &version, UInt32 &length) {
		if (version == kSerializationDataVersion && length > 5) {
			char *buf = new char[length + 1];
			intfc->ReadRecordData(buf, length);
			buf[length] = 0;
			std::stringstream ss(buf);
			Data->LoadStream(ss);
			delete[] buf;
			_MESSAGE("\t- objects: %d", Data->Data.size());
		}
	}

	template <class T> void Save(T *Data, SKSESerializationInterface* intfc, UInt32 type) {
		if (!Data || Data->Data.empty())
			_MESSAGE("\t- empty!");
		else{
			_MESSAGE("\t- objects: %d", Data->Data.size());
			std::stringstream ss;
			Data->SaveStream(ss);
			if (intfc->OpenRecord(type, kSerializationDataVersion)) {
				const std::string &str = ss.str();
				const char *cstr = str.c_str();
				intfc->WriteRecordData(cstr, strlen(cstr));
			}
		}
	}

	void FormDelete(UInt64 handle){
		if (intValues){
			_MESSAGE("Form Delete Handle: %llu", handle);
			intValues->RemoveForm(handle);
			floatValues->RemoveForm(handle);
			stringValues->RemoveForm(handle);
			formValues->RemoveForm(handle);

			intLists->RemoveForm(handle);
			floatLists->RemoveForm(handle);
			stringLists->RemoveForm(handle);
			formLists->RemoveForm(handle);
		}
	}

	void Serialization_Load(SKSESerializationInterface *intfc) {
		UInt32	type;
		UInt32	version;
		UInt32	length;

		InitLists();

		//Forms::LoadCurrentMods();
		//Forms::LoadModList(intfc);

		_MESSAGE("Storage Loading...");

		while (intfc->GetNextRecordInfo(&type, &version, &length)) {
			switch (type) {
			case 'MODS':
				_MESSAGE("\tMODS Load (old)");
				Forms::LoadModList(intfc);
				break;

			case 'PLGN':
				_MESSAGE("\tPLGN Load");
				Forms::LoadPluginList(intfc);
				break;

			case 'INTV':
				_MESSAGE("\tINTV Load");
				Load(intValues, intfc, version, length);
				break;

			case 'FLOV':
				_MESSAGE("\tFLOV Load");
				Load(floatValues, intfc, version, length);
				break;

			case 'STRV':
				_MESSAGE("\tSTRV Load");
				Load(stringValues, intfc, version, length);
				break;

			case 'FORV':
				_MESSAGE("\tFORV Load");
				Load(formValues, intfc, version, length);
				break;

			case 'INTL':
				_MESSAGE("\tINTL Load");
				Load(intLists, intfc, version, length);
				break;

			case 'FLOL':
				_MESSAGE("\tFLOL Load");
				Load(floatLists, intfc, version, length);
				break;

			case 'STRL':
				_MESSAGE("\tSTRL Load");
				Load(stringLists, intfc, version, length);
				break;

			case 'FORL':
				_MESSAGE("\tFORL Load");
				Load(formLists, intfc, version, length);
				break;
			
			case 'PKGO':
				_MESSAGE("\tPKGO Load");
				Load(PackageData::GetPackages(), intfc, version, length);
				break;

			/*case 'PACK':
				_MESSAGE("PACK Load");
				Load(packageLists, intfc, version, length);
				break;*/
			
			/*case 'DATA':
				if (version == kSerializationDataVersion && length > 0) {
					char *buf = new char[length + 1];
					intfc->ReadRecordData(buf, length);
					buf[length] = 0;
					std::stringstream ss(buf);
					const std::string &tmp = ss.str();
					_MESSAGE("DATA: %d", strlen(tmp.c_str()));
					int ver;
					ss >> ver;
					//Forms::LoadPreviousMods(ss);
					if (ver == -1) {
						_MESSAGE("-- Legacy Data");
						intValues->LoadStream(ss);
						floatValues->LoadStream(ss);
						stringValues->LoadStream(ss);
						formValues->LoadStream(ss);

						intLists->LoadStream(ss);
						floatLists->LoadStream(ss);
						stringLists->LoadStream(ss);
						formLists->LoadStream(ss);
					}
					delete[] buf;
				}
				break;*/

			default:
				_MESSAGE("unhandled type %08X", type);
				break;
			}
		}
		//Forms::ClearPreviousMods();
		_MESSAGE("Done!\n");
	}

	void Serialization_Save(SKSESerializationInterface *intfc) {
		_MESSAGE("Storage Saving...");

		//Forms::LoadCurrentMods();
		//Forms::ClearPreviousMods();

		// Init lists if for some weird reason unset
		InitLists();

		/*if (intfc->OpenRecord('DATA', kSerializationDataVersion)) {
			std::stringstream ss;
			ss << (int)1;
			Forms::SaveCurrentMods(ss);
			std::string str = ss.str();
			const char *cstr = str.c_str();
			intfc->WriteRecordData(cstr, strlen(cstr));
		}*/
		// Save load order
		//Forms::SaveModList(intfc);
		
		Forms::SavePluginsList(intfc);
		_MESSAGE("\tPLGN Saved");

		// Cleanup removed forms
		/*int cleaned = 0;
		cleaned += intValues->Cleanup();
		cleaned += floatValues->Cleanup();
		cleaned += stringValues->Cleanup();
		cleaned += formValues->Cleanup();
		cleaned += intLists->Cleanup();
		cleaned += floatLists->Cleanup();
		cleaned += stringLists->Cleanup();
		cleaned += formLists->Cleanup();
		if (cleaned > 0)
			_MESSAGE("- discarded: %d", cleaned);*/

		// Save value storage
		Save(intValues, intfc, 'INTV');
		_MESSAGE("\tINTV Saved");
		Save(floatValues, intfc, 'FLOV');
		_MESSAGE("\tFLOV Saved");
		Save(stringValues, intfc, 'STRV');
		_MESSAGE("\tSTRV Saved");
		Save(formValues, intfc, 'FORV');
		_MESSAGE("\tFORV Saved");

		// Save list storage
		Save(intLists, intfc, 'INTL');
		_MESSAGE("\tINTL Saved");
		Save(floatLists, intfc, 'FLOL');
		_MESSAGE("\tFLOL Saved");
		Save(stringLists, intfc, 'STRL');
		_MESSAGE("\tSTRL Saved");
		Save(formLists, intfc, 'FORL');
		_MESSAGE("\tFORL Saved");

		// Overrides
		Save(PackageData::GetPackages(), intfc, 'PKGO');
		_MESSAGE("\tPKGO Saved");

		// Save external files
		External::SaveFiles();
		_MESSAGE("\tExternal JSON Saved");

		_MESSAGE("Save Done!\n");
	}

	void Serialization_Revert(SKSESerializationInterface* intfc) {
		_MESSAGE("Storage Reverting...");

		_MESSAGE("\t - Mod List");
		Forms::ClearModList();
		//Forms::LoadCurrentMods();
		//Forms::ClearPreviousMods();


		_MESSAGE("\t - StorageUtil: Values");
		intValues->Revert();
		floatValues->Revert();
		stringValues->Revert();
		formValues->Revert();

		_MESSAGE("\t - StorageUtil: Lists");
		intLists->Revert();
		floatLists->Revert();
		stringLists->Revert();
		formLists->Revert();

		_MESSAGE("\t - Package Overrides");
		PackageData::GetPackages()->Revert();

		// Revert external files
		_MESSAGE("\t - JSON Files");
		External::RevertFiles();

		_MESSAGE("\t - Re-Init");
		InitLists();

		_MESSAGE("Done!\n");
		
		//_MESSAGE("Revert skip!\n");

	}
}
