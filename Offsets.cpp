#include "Offsets.h"

unsigned long long Offset_ToggleFreeCam = 0; // 50809
unsigned long long Offset_IsValidPackage = 0; // 29619
unsigned long long Offset_PackageStartOrig = 0; // 12057
unsigned long long Offset_PackageEndOrig = 0; // 69166
unsigned long long Offset_PackageStart_Enter = 0; // 37398 + 0x47
unsigned long long Offset_PackageEnd_Enter = 0; // 53984 + 0x103


namespace Plugin {


	bool InitializeOffsets()
	{
		VersionDb db;

		// Load database with current executable version.
		//if (!db.Load(1, 6, 318, 0))
		if (!db.Load())
		{
			_FATALERROR("Failed to load version database for current executable!");
			return false;
		}
		else
		{
			_MESSAGE("Loaded database for %s version %s.", db.GetModuleName().c_str(), db.GetLoadedVersionString().c_str());
		}

		if (!db.FindOffsetById(50809, Offset_ToggleFreeCam))
		{
			_FATALERROR("Failed to find version database address for ToggleFreeCam!");
			return false;
		}

		if (!db.FindOffsetById(29619, Offset_IsValidPackage))
		{
			_FATALERROR("Failed to find version database address for IsValidPackage!");
			return false;
		}

		if (!db.FindOffsetById(12057, Offset_PackageStartOrig))
		{
			_FATALERROR("Failed to find version database address for PackageStartOrig!");
			return false;
		}

		if (!db.FindOffsetById(69166, Offset_PackageEndOrig))
		{
			_FATALERROR("Failed to find version database address for PackageEndOrig!");
			return false;
		}

		if (!db.FindOffsetById(37398, Offset_PackageStart_Enter))
		{
			_FATALERROR("Failed to find version database address for PackageStartEnter!");
			return false;
		}
		Offset_PackageStart_Enter += 0x47;

		if (!db.FindOffsetById(53984, Offset_PackageEnd_Enter))
		{
			_FATALERROR("Failed to find version database address for PackageEndEnter!");
			return false;
		}
		Offset_PackageEnd_Enter += 0x103;


		return true;
	}

	bool DumpSpecificVersion()
	{
		VersionDb db;

		// Try to load database of version 1.5.62.0 regardless of running executable version.
		if (!db.Load(1, 6, 323, 0))
		{
			_FATALERROR("Failed to load database for 1.6.323.0!");
			return false;
		}

		// Write out a file called offsets-1.5.62.0.txt where each line is the ID and offset.
		db.Dump("offsets-1.6.323.0.txt");
		_MESSAGE("Dumped offsets for 1.6.323.0");
		return true;
	}

}