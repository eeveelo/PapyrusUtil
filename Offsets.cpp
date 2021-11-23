#include "Offsets.h"

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

}
