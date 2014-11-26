#include "Serialize.h"

#include <sstream>

#include "skse/GameAPI.h"
#include "skse/PluginAPI.h"
#include "skse/GameTypes.h"

#include "skse/PapyrusVM.h"
#include "skse/PapyrusArgs.h"

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
	//extern forl* packageLists;
	//extern aniv* animValues;

	template <class T> void Load(T *Data, SKSESerializationInterface *intfc, UInt32 &version, UInt32 &length) {
		if (version == kSerializationDataVersion && length > 1) {
			char *buf = new char[length + 1];
			intfc->ReadRecordData(buf, length);
			buf[length] = 0;
			std::stringstream ss(buf);
			_MESSAGE("- length: %d", strlen(buf));
			if (strlen(buf) < 200) _MESSAGE(buf);
			Data->LoadStream(ss);
			delete[] buf;
		}
	}

	template <class T> void Save(T *Data, SKSESerializationInterface* intfc, UInt32 type) {
		std::stringstream ss;
		Data->SaveStream(ss);
		if (intfc->OpenRecord(type, kSerializationDataVersion)) {
			std::string str = ss.str();
			const char *cstr = str.c_str();
			intfc->WriteRecordData(cstr, strlen(cstr));
			if (strlen(cstr) < 200) _MESSAGE(cstr);
		}
	}

	void Serialization_Load(SKSESerializationInterface *intfc) {
		UInt32	type;
		UInt32	version;
		UInt32	length;

		if (!intValues) InitLists();

		//Forms::LoadCurrentMods();
		//Forms::LoadModList(intfc);

		_MESSAGE("Storage Loading...");

		while (intfc->GetNextRecordInfo(&type, &version, &length)) {
			switch (type) {
			case 'MODS':
				_MESSAGE("MODS Load");
				Forms::LoadModList(intfc);
				break;

			case 'INTV':
				_MESSAGE("INTV Load");
				Load(intValues, intfc, version, length);
				break;

			case 'FLOV':
				_MESSAGE("FLOV Load");
				Load(floatValues, intfc, version, length);
				break;

			case 'STRV':
				_MESSAGE("STRV Load");
				Load(stringValues, intfc, version, length);
				break;

			case 'FORV':
				_MESSAGE("FORV Load");
				Load(formValues, intfc, version, length);
				break;

			case 'INTL':
				_MESSAGE("INTL Load");
				Load(intLists, intfc, version, length);
				break;

			case 'FLOL':
				_MESSAGE("FLOL Load");
				Load(floatLists, intfc, version, length);
				break;

			case 'STRL':
				_MESSAGE("STRL Load");
				Load(stringLists, intfc, version, length);
				break;

			case 'FORL':
				_MESSAGE("FORL Load");
				Load(formLists, intfc, version, length);
				break;
			
			case 'PKGO':
				_MESSAGE("PKGO Load");
				Load(PackageData::GetPackages(), intfc, version, length);
				break;

			/*case 'PACK':
				_MESSAGE("PACK Load");
				Load(packageLists, intfc, version, length);
				break;*/
			
			case 'DATA':
				if (version == kSerializationDataVersion && length > 0) {
					char *buf = new char[length + 1];
					intfc->ReadRecordData(buf, length);
					buf[length] = 0;
					std::stringstream ss(buf);
					_MESSAGE("DATA: %d", strlen(ss.str().c_str()));
					int ver;
					ss >> ver;
					Forms::LoadPreviousMods(ss);
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
				break;

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
		if (!intValues) InitLists();

		/*if (intfc->OpenRecord('DATA', kSerializationDataVersion)) {
			std::stringstream ss;
			ss << (int)1;
			Forms::SaveCurrentMods(ss);
			std::string str = ss.str();
			const char *cstr = str.c_str();
			intfc->WriteRecordData(cstr, strlen(cstr));
		}*/
		// Save load order
		Forms::SaveModList(intfc);



		// Cleanup removed forms
		int cleaned = 0;
		cleaned += intValues->Cleanup();
		cleaned += floatValues->Cleanup();
		cleaned += stringValues->Cleanup();
		cleaned += formValues->Cleanup();
		cleaned += intLists->Cleanup();
		cleaned += floatLists->Cleanup();
		cleaned += stringLists->Cleanup();
		cleaned += formLists->Cleanup();

		if (cleaned > 0)
			_MESSAGE("- discarded: %d", cleaned);		

		// Save value storage
		Save(intValues, intfc, 'INTV');
		Save(floatValues, intfc, 'FLOV');
		Save(stringValues, intfc, 'STRV');
		Save(formValues, intfc, 'FORV');

		// Save list storage
		Save(intLists, intfc, 'INTL');
		Save(floatLists, intfc, 'FLOL');
		Save(stringLists, intfc, 'STRL');
		Save(formLists, intfc, 'FORL');

		// Overrides
		Save(PackageData::GetPackages(), intfc, 'PKGO');

		// Save external files
		External::SaveFiles();

		_MESSAGE("Done!\n");
	}

	void Serialization_Revert(SKSESerializationInterface* intfc) {
		_MESSAGE("Storage Reverting...");
		if (!intValues)
			InitLists();
		else {
			//Forms::LoadCurrentMods();
			//Forms::ClearPreviousMods();

			intValues->Revert();
			floatValues->Revert();
			stringValues->Revert();
			formValues->Revert();

			intLists->Revert();
			floatLists->Revert();
			stringLists->Revert();
			formLists->Revert();

			PackageData::GetPackages()->Revert();

			// Revert external files
			External::RevertFiles();
		}
		_MESSAGE("Done!\n");
	}
}
