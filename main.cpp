#include <locale.h>

#include "common/IDebugLog.h"
#include "skse64/PluginAPI.h"
#include "skse64_common/skse_version.h"
#include "skse64/ScaleformCallbacks.h"

#include "Plugin.h"
#include "Offsets.h"
#include "Data.h"
#include "Serialize.h"

#include <shlobj.h>

#include "versionlibdb.h"

IDebugLog	gLog;

PluginHandle	g_pluginHandle = kPluginHandle_Invalid;

// SKSE Interfaces
SKSESerializationInterface* g_serialization = NULL;
SKSEPapyrusInterface* g_papyrus = NULL;
//SKSETrampolineInterface* g_trampoline = NULL;

extern "C" {
	__declspec(dllexport) SKSEPluginVersionData SKSEPlugin_Version =
	{

		SKSEPluginVersionData::kVersion,

		2,
		"PapyrusUtil",

		"Ashal",
		"Ashal@loverslab.com",

		0,
		SKSEPluginVersionData::kVersionIndependent_StructsPost629,
		{ RUNTIME_VERSION_1_6_1130, 0 },

		0,	// works with any version of the script extender. you probably do not need to put anything here
	};


	bool SKSEPlugin_Load(const SKSEInterface* skse) {
		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		// Don't load in editor.
		if (skse->isEditor) {
			return false;
		}

		// Set log path
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\PapyrusUtilDev.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
		//_MESSAGE("Loading Version: %d", (int)PAPYRUSUTIL_VERSION);

		// Initialize offsets with address library
		if (!Plugin::InitializeOffsets()) {
			_MESSAGE("InitializeOffsets failed!");
			return false;
		}

		// get trampoline interface
		/*g_trampoline = (SKSETrampolineInterface*)skse->QueryInterface(kInterface_Trampoline);
		if (!g_trampoline) {
			_MESSAGE("couldn't get trampoline interface");
			return false;
		}

		g_trampoline->AllocateFromBranchPool(g_pluginHandle, 1024 * 64);
		g_trampoline->AllocateFromLocalPool(g_pluginHandle, 1024 * 64);*/

		// Init storage
		Plugin::InitPlugin();
		Data::InitLists();

		// Get the serialization interface and query its version
		g_serialization = (SKSESerializationInterface*)skse->QueryInterface(kInterface_Serialization);
		if (!g_serialization) {
			_MESSAGE("couldn't get serialization interface");
			return false;
		}
		if (g_serialization->version < SKSESerializationInterface::kVersion) {
			_MESSAGE("serialization interface too old (%d expected %d)", g_serialization->version, SKSESerializationInterface::kVersion);
			return false;
		}

		// get papyrus vm interface
		g_papyrus = (SKSEPapyrusInterface*)skse->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus) {
			_MESSAGE("couldn't get papyrus interface");
			return false;
		}


		/*if (!g_branchTrampoline.Create(1024 * 64))
		{
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return;
		}

		if (!g_localTrampoline.Create(1024 * 64, GetModuleHandle("PapyrusUtil.dll")))
		{
			_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return;
		}*/

		// Serialization handlers
		g_serialization->SetUniqueID(g_pluginHandle, 884715692 + 227106806);
		g_serialization->SetSaveCallback(g_pluginHandle, Data::Serialization_Save);
		g_serialization->SetLoadCallback(g_pluginHandle, Data::Serialization_Load);
		g_serialization->SetRevertCallback(g_pluginHandle, Data::Serialization_Revert);
		//g_serialization->SetFormDeleteCallback(g_pluginHandle, Data::FormDelete);

		// Register Papyrus functions
		g_papyrus->Register(Plugin::RegisterFuncs);


		return true;
	}

}
