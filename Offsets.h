#pragma once

#include "versionlibdb.h"

extern unsigned long long Offset_ToggleFreeCam; // 50809
extern unsigned long long Offset_IsValidPackage; // 29619
extern unsigned long long Offset_PackageStartOrig; // 12057
extern unsigned long long Offset_PackageEndOrig; // 69166
extern unsigned long long Offset_PackageStart_Enter; // 37398 + 0x47
extern unsigned long long Offset_PackageEnd_Enter; // 53984 + 0x103

namespace Plugin {
	bool InitializeOffsets();
	bool DumpSpecificVersion();
}
