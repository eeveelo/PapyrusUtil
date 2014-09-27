#include "skse/PluginAPI.h"
#include "skse/skse_version.h"
#include "PapyrusUtil.h"

#include "Data.h"
#include "Serialize.h"

IDebugLog	gLog("PapyrusUtilDev.log");

PluginHandle	g_pluginHandle = kPluginHandle_Invalid;

// SKSE Interfaces
SKSESerializationInterface * g_serialization = NULL;
SKSEPapyrusInterface       * g_papyrus = NULL;

extern "C" {

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) {
		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "papyrusutil plugin";
		info->version = 1;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		// Don't load in editor.
		if(skse->isEditor) {
			return false;
		}

		// Check if version is right.
		else if(skse->runtimeVersion != RUNTIME_VERSION_1_9_32_0) {
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);
			return false;
		}

		// get the serialization interface and query its version
		g_serialization = (SKSESerializationInterface *)skse->QueryInterface(kInterface_Serialization);
		if(!g_serialization) {
			_MESSAGE("couldn't get serialization interface");
			return false;
		}
		if(g_serialization->version < SKSESerializationInterface::kVersion) {
			_MESSAGE("serialization interface too old (%d expected %d)", g_serialization->version, SKSESerializationInterface::kVersion);
			return false;
		}

		// get papyrus vm interface
		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);
		if(!g_papyrus) {
			_MESSAGE("couldn't get papyrus interface");
			return false;
		}

		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse) {
		_MESSAGE("Loading");

		Data::InitLists();

		// Dev tmp
		//g_serialization->SetUniqueID(g_pluginHandle, 884792 + 15325);

		g_serialization->SetUniqueID(g_pluginHandle, 884715692 + 227106806);
		g_serialization->SetSaveCallback(g_pluginHandle, Data::Serialization_Save);
		g_serialization->SetLoadCallback(g_pluginHandle, Data::Serialization_Load);
		g_serialization->SetRevertCallback(g_pluginHandle, Data::Serialization_Revert);

		g_papyrus->Register(PapyrusUtil::RegisterFuncs);

		PapyrusUtil::InitPlugin();

		return true;
	}
}