#include "PackageData.h"

#include "skse/GameRTTI.h"
#include "skse/GameTypes.h"
#include "skse/GameReferences.h"

#include "Forms.h"

namespace PackageData {
	static Packages* s_PackageData;
	Packages* GetPackages(){ if (!s_PackageData) { s_PackageData = new Packages(); } return s_PackageData; }

	/*
	*  Package override serialize
	*/

	void Packages::LoadStream(std::stringstream &ss){
		int count;
		ss >> count;
		if (count < 1) return;
		s_dataLock.Enter();
		Data.reserve(count);
		for (int i = 0; i < count; ++i) {
			// Unpack actor
			UInt32 ActorID, ActorCount;
			ss >> ActorID;
			ss >> ActorCount;
			// Validate actor
			ActorID = Forms::ResolveFormID(ActorID);
			TESForm* ActorRef = LookupFormByID(ActorID);
			// TODO: Validate ActorRef before assuming they are still valid
			Data[ActorRef->formID].reserve(ActorCount);
			for (int n = 0; n < ActorCount; ++n) {
				// Unpack Package
				UInt32 PackID, priority, flag;
				ss >> PackID;
				ss >> priority;
				ss >> flag;
				// Validate Package
				PackID = Forms::ResolveFormID(PackID);
				TESForm* PackageRef = LookupFormByID(PackID);
				if (ActorRef && PackageRef && PackageRef->formType == kFormType_Package){
					// LOAD Package
					if (priority > 100) priority = 100;
					else if (priority < 1) priority = 1;
					Data[ActorRef->formID][PackageRef->formID] = Flags(priority, flag == 1 ? 1 : 0);
				}
			}
		}
		Data.shrink_to_fit();
		s_dataLock.Leave();
	}
	void Packages::SaveStream(std::stringstream &ss){
		s_dataLock.Enter();
		// Cleanup
		for (PackageMap::iterator ActorItr = Data.begin(); ActorItr != Data.end();){
			if (ActorItr->second.size() < 1) Data.erase(ActorItr);
			else ++ActorItr;
		}
		// Serialize
		ss << (int)Data.size();
		for (PackageMap::iterator i = Data.begin(); i != Data.end(); ++i) {
			ss << ' ' << i->first; // Actor formID
			ss << ' ' << (int)i->second.size(); // Actor package count
			for (ActorPackages::iterator n = i->second.begin(); n != i->second.end(); ++n) {
				ss << ' ' << (UInt32)n->first; // Package
				ss << ' ' << (UInt32)n->second.first; // Priority
				ss << ' ' << (UInt32)n->second.second; // Flag
			}
		}
		s_dataLock.Leave();
	}
	void Packages::Revert(){
		s_dataLock.Enter();
		Data.clear();
		s_dataLock.Leave();
	}


	/*
	*  Package override interface
	*/

	void Packages::AddPackage(Actor* ActorRef, TESPackage* PackageRef, UInt32 priority, UInt32 flags){
		if (ActorRef && PackageRef){
			if (priority > 100) priority = 100;
			else if (priority < 1) priority = 1;
			if (flags != 1) flags = 0;

			s_dataLock.Enter();
			Data[ActorRef->formID][PackageRef->formID] = Flags(priority, flags);
			s_dataLock.Leave();
		}
	}

	bool Packages::RemovePackage(Actor* ActorRef, TESPackage* PackageRef){
		bool removed = false;
		if (ActorRef && PackageRef && Data.size() > 0){
			s_dataLock.Enter();
			ActorPackages* ActorPacks = GetActor(ActorRef);
			if (ActorPacks != NULL){
				// Remove
				ActorPackages::iterator PacksItr = ActorPacks->find(PackageRef->formID);
				if (PacksItr != ActorPacks->end())
					ActorPacks->erase(PackageRef->formID);
				removed = ActorPacks->find(PackageRef->formID) == ActorPacks->end();
				// Cleanup
				if (Data[ActorRef->formID].size() == 0)
					Data.erase(ActorRef->formID);
			}
			s_dataLock.Leave();
		}
		return removed;
	}

	UInt32 Packages::CountPackages(Actor* ActorRef){
		UInt32 count = 0;
		if (ActorRef && Data.size() > 0){
			s_dataLock.Enter();
			ActorPackages* ActorPacks = GetActor(ActorRef);
			count = ActorPacks != NULL ? ActorPacks->size() : 0;
			s_dataLock.Leave();
		}
		return count;
	}

	UInt32 Packages::ClearActor(Actor* ActorRef){
		UInt32 count = 0;
		if (ActorRef && Data.size() > 0){
			s_dataLock.Enter();
			count = Data[ActorRef->formID].size();
			Data.erase(ActorRef->formID);
			s_dataLock.Leave();
		}
		return count;
	}

	UInt32 Packages::ClearPackage(TESPackage* PackageRef){
		UInt32 count = 0;
		if (PackageRef && Data.size() > 0){
			s_dataLock.Enter();
			for (PackageMap::iterator ActorItr = Data.begin(); ActorItr != Data.end(); ++ActorItr){
				ActorPackages *ActorPacks = &ActorItr->second;
				ActorPackages::iterator PacksItr = ActorPacks->find(PackageRef->formID);
				if (PacksItr != ActorPacks->end()){
					ActorPacks->erase(PackageRef->formID);
					count++;
				}	
			}
			s_dataLock.Leave();
		}
		return count;
	};


}

/*
*  Package override handling
*/

#include "skse/SafeWrite.h"
#include "SafeRead.h"
#include "Forms.h"

namespace PackageData {

	UInt32 DecidePackage(int ActorID, int PackageID){
		if (s_PackageData && ActorID != 0) return s_PackageData->DecidePackage(ActorID, PackageID);
		else return PackageID;
	}
	UInt32 Packages::DecidePackage(int ActorID, int PackageID){
		if (Data.size() == 0 || ActorID == 0) return PackageID;
		s_dataLock.Enter();
		ActorPackages* Overrides = GetActor((TESForm*)ActorID);
		if (Overrides != NULL){
			UInt32 pickedPack = 0;
			Flags pickedFlags = Flags(0, 0);
			for (ActorPackages::iterator itr = Overrides->begin(); itr != Overrides->end(); ++itr){
				if (itr->second.first >= pickedFlags.first){
					pickedPack  = itr->first;
					pickedFlags = itr->second;
					//_MESSAGE("Package[%lu] Priority[%lu] Flag[%lu]", pickedPack, pickedFlags.first, pickedFlags.second);
				}
			}
			int pid = Forms::GameGetForm(pickedPack);
			TESForm* FormRef = pid == 0 ? NULL : (TESForm*)pid;
			if (FormRef && FormRef->formType == kFormType_Package){
				if (pickedFlags.second == 1 || IsValidPackage(pid, ActorID)){
					//_MESSAGE("Override Picked -- Package[%lu] Priority[%lu] Flag[%lu]", pid, pickedFlags.first, pickedFlags.second);
					PackageID = pid;
				}
			}
		}
		s_dataLock.Leave();
		return PackageID;
	}
	bool Packages::IsValidPackage(int PackageID, int ActorID) {
		int IsValid = 0;
		int FuncAddr = 0x05E1F40;
		_asm
		{
			mov ecx, PackageID
				push ActorID
				call FuncAddr
				mov IsValid, eax
		}
		return IsValid != 0;
	}


	int EndPackID  = 0;
	int EndActorID = 0;
	void PackageEnded(){
		if (s_PackageData && EndPackID != 0 && EndActorID != 0)
			s_PackageData->PackageEnded();
		EndPackID  = 0;
		EndActorID = 0;
	}
	void Packages::PackageEnded(){
		if (Data.size() == 0) return;

		TESForm* ActorRef   = LookupFormByID(EndActorID);
		TESForm* PackageRef = LookupFormByID(EndPackID);
		if (ActorRef && PackageRef){
			s_dataLock.Enter();
			ActorPackages* ActorPacks = GetActor(ActorRef);
			if (ActorPacks != NULL){
				// Remove
				ActorPackages::iterator PacksItr = ActorPacks->find(PackageRef->formID);
				if (PacksItr != ActorPacks->end()) ActorPacks->erase(PackageRef->formID);
				// Cleanup
				if (Data[ActorRef->formID].size() == 0)
					Data.erase(ActorRef->formID);
			}
			s_dataLock.Leave();
		}
		EndPackID  = 0;
		EndActorID = 0;
	}

	int endCall = 0xA51280;
	int endBack = 0x8CAB59;
	int packageFunc = 0x40E4F0;
	int jumpPackage = 0x6A9E95;
	void InitPlugin() {
		void * packageCodeStart;
		_asm
		{
			mov packageCodeStart, offset packageStart
				jmp packageEnd
			packageStart :
			call packageFunc
				cmp esi, 0
				je packageOrig
				push eax
				push esi
				call DecidePackage
				add esp, 8
			packageOrig:
			jmp jumpPackage
			packageEnd :
		}

		WriteRelJump(0x6A9E90, (UInt32)packageCodeStart);

		void * endPackageCodeStart;
		_asm
		{
			mov endPackageCodeStart, offset endPackageStart
				jmp endPackageEnd
			endPackageStart :
			push eax
				push ecx
				push edx
				mov eax, [edi]
				mov EndActorID, eax
				mov eax, [edi + 4]
				mov EndPackID, eax
				call PackageEnded
				pop edx
				pop ecx
				pop eax
				//endPackageOrig:
				call endCall
				jmp endBack
			endPackageEnd :
		}

		WriteRelJump(0x8CAB54, (UInt32)endPackageCodeStart);
	}

} // PackageData
