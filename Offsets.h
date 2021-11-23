#pragma once

#include "versionlibdb.h"

static unsigned long long Offset_ToggleFreeCam = 0; // 50809
static unsigned long long Offset_IsValidPackage = 0; // 29619
static unsigned long long Offset_PackageStartOrig = 0; // 12057
static unsigned long long Offset_PackageEndOrig = 0; // 69166
static unsigned long long Offset_PackageStart_Enter = 0; // 37398 + 0x47
static unsigned long long Offset_PackageEnd_Enter = 0; // 53984 + 0x103

namespace Plugin {

	bool InitializeOffsets();

}
