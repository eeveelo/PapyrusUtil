#include "PackageData.h"

#include "skse64/GameRTTI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameReferences.h"
#include "skse64_common/BranchTrampoline.h"
#include "xbyak/xbyak.h"

#include "Forms.h"

namespace PackageData {
	static Packages* s_PackageData;
	Packages* GetPackages() { if (!s_PackageData) { s_PackageData = new Packages(); } return s_PackageData; }

	/*
	*  Package override serialize
	*/

	void Packages::LoadStream(std::stringstream &ss) {
		int count;
		ss >> count;
		if (count < 1) return;
		s_dataLock.Enter();
		Data.reserve(count);
		for (int i = 0; i < count; ++i) {
			// Unpack actor
			UInt32 formID;
			ss >> formID;
			int count2;
			ss >> count2;
			// Validate actor
			UInt32 newID = Forms::ResolveFormID(formID);
			TESForm *FormRef = LookupFormByID(newID);
			Actor *ActorRef = FormRef != NULL ? DYNAMIC_CAST(FormRef, TESForm, Actor) : NULL;
			if (ActorRef == NULL) {
				for (int n = 0; n < count2; ++n) {
					UInt32 PackID, priority, flag;
					ss >> PackID; ss >> priority; ss >> flag;
				}
			}
			else {
				Data[newID].reserve(count2);
				for (int n = 0; n < count2; ++n) {
					// Unpack Package
					UInt32 PackID, priority, flag;
					ss >> PackID; ss >> priority; ss >> flag;
					// Validate Package
					PackID = Forms::ResolveFormID(PackID);
					TESForm* PackageRef = LookupFormByID(PackID);
					if (PackageRef != NULL && PackageRef->formType == kFormType_Package) {
						// LOAD Package
						if (priority > 100) priority = 100;
						else if (priority < 1) priority = 1;
						Data[newID][PackageRef->formID] = Flags(priority, flag == 1 ? 1 : 0);
					}
				}
				Data[newID].shrink_to_fit();
			}

		}
		Data.shrink_to_fit();
		s_dataLock.Leave();
	}
	void Packages::SaveStream(std::stringstream &ss) {
		s_dataLock.Enter();
		// Cleanup
		for (PackageMap::iterator ActorItr = Data.begin(); ActorItr != Data.end();) {
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
	void Packages::Revert() {
		s_dataLock.Enter();
		Data.clear();
		s_dataLock.Leave();
	}


	/*
	*  Package override interface
	*/

	void Packages::AddPackage(Actor* ActorRef, TESPackage* PackageRef, UInt32 priority, UInt32 flags) {
		if (ActorRef && PackageRef) {
			if (priority > 100) priority = 100;
			else if (priority < 1) priority = 1;
			if (flags != 1) flags = 0;

			s_dataLock.Enter();
			Data[ActorRef->formID][PackageRef->formID] = Flags(priority, flags);
			s_dataLock.Leave();
		}
	}

	bool Packages::RemovePackage(Actor* ActorRef, TESPackage* PackageRef) {
		bool removed = false;
		if (ActorRef && PackageRef && Data.size() > 0) {
			s_dataLock.Enter();
			ActorPackages* ActorPacks = GetActor(ActorRef);
			if (ActorPacks != NULL) {
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

	UInt32 Packages::CountPackages(Actor* ActorRef) {
		UInt32 count = 0;
		if (ActorRef && Data.size() > 0) {
			s_dataLock.Enter();
			ActorPackages* ActorPacks = GetActor(ActorRef);
			count = ActorPacks != NULL ? ActorPacks->size() : 0;
			s_dataLock.Leave();
		}
		return count;
	}

	UInt32 Packages::ClearActor(Actor* ActorRef) {
		UInt32 count = 0;
		if (ActorRef && Data.size() > 0) {
			s_dataLock.Enter();
			count = Data[ActorRef->formID].size();
			Data.erase(ActorRef->formID);
			s_dataLock.Leave();
		}
		return count;
	}

	UInt32 Packages::ClearPackage(TESPackage* PackageRef) {
		UInt32 count = 0;
		if (PackageRef && Data.size() > 0) {
			s_dataLock.Enter();
			for (PackageMap::iterator ActorItr = Data.begin(); ActorItr != Data.end(); ++ActorItr) {
				ActorPackages *ActorPacks = &ActorItr->second;
				ActorPackages::iterator PacksItr = ActorPacks->find(PackageRef->formID);
				if (PacksItr != ActorPacks->end()) {
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

#include "skse64_common/SafeWrite.h"
#include "SafeRead.h"
#include "Forms.h"
#include "Offsets.h"

namespace PackageData {

	TESPackage *DecidePackage(Actor *ActorID, TESPackage *PackageID) {
		if (s_PackageData && ActorID != 0) return s_PackageData->DecidePackage(ActorID, PackageID);
		else return PackageID;
	}
	TESPackage *Packages::DecidePackage(Actor *ActorID, TESPackage *PackageID) {
		if (Data.size() == 0 || ActorID == 0) return PackageID;
		s_dataLock.Enter();
		ActorPackages* Overrides = GetActor((TESForm*)ActorID);
		if (Overrides != NULL) {
			UInt32 pickedPack = 0;
			Flags pickedFlags = Flags(0, 0);
			for (ActorPackages::iterator itr = Overrides->begin(); itr != Overrides->end(); ++itr) {
				if (itr->second.first >= pickedFlags.first) {
					pickedPack = itr->first;
					pickedFlags = itr->second;
					_MESSAGE("Package[%lu] Priority[%lu] Flag[%lu]", pickedPack, pickedFlags.first, pickedFlags.second);
				}
			}
			TESPackage *pid = (TESPackage *)LookupFormByID(pickedPack);
			TESForm* FormRef = pid == 0 ? NULL : (TESForm*)pid;
			if (FormRef && FormRef->formType == kFormType_Package) {
				if (pickedFlags.second == 1 || IsValidPackage(pid, ActorID)) {
					_MESSAGE("Override Picked -- Package[%lu] Priority[%lu] Flag[%lu]", pid, pickedFlags.first, pickedFlags.second);
					PackageID = pid;
				}
			}
		}
		s_dataLock.Leave();
		return PackageID;
	}

	typedef int(*_IsValid)(TESPackage *PackageID, Actor *ActorID);
	bool Packages::IsValidPackage(TESPackage *PackageID, Actor *ActorID) {
		//static RelocAddr<_IsValid> IsValid(0x00454330);
		RelocAddr<_IsValid> IsValid((std::uintptr_t)Offset_IsValidPackage);
		return (IsValid(PackageID, ActorID) != 0);
	}

	int EndPackID = 0;
	int EndActorID = 0;
	void PackageEnded() {
		if (s_PackageData && EndPackID != 0 && EndActorID != 0)
			s_PackageData->PackageEnded();
		EndPackID = 0;
		EndActorID = 0;
	}
	void Packages::PackageEnded() {
		if (Data.size() == 0) return;

		TESForm* ActorRef = LookupFormByID(EndActorID);
		TESForm* PackageRef = LookupFormByID(EndPackID);
		if (ActorRef && PackageRef) {
			s_dataLock.Enter();
			ActorPackages* ActorPacks = GetActor(ActorRef);
			if (ActorPacks != NULL) {
				// Remove
				ActorPackages::iterator PacksItr = ActorPacks->find(PackageRef->formID);
				if (PacksItr != ActorPacks->end()) ActorPacks->erase(PackageRef->formID);
				// Cleanup
				if (Data[ActorRef->formID].size() == 0)
					Data.erase(ActorRef->formID);
			}
			s_dataLock.Leave();
		}
		EndPackID = 0;
		EndActorID = 0;
	}

	typedef TESPackage* (*_PackageStartOrig)(void *, Actor *);
	TESPackage *PackageStartHook(void *pthis, Actor *actor)
	{
		//static RelocAddr<_PackageStartOrig> PackageStartOrig(0x0012D460);
		RelocAddr<_PackageStartOrig> PackageStartOrig((std::uintptr_t)Offset_PackageStartOrig);
		TESPackage *pkg = PackageStartOrig(pthis, actor);

		if (actor && actor->formID != 0)
		{
			return (TESPackage *)DecidePackage(actor, pkg);
		}

		return pkg;
	}

	typedef UInt64(*_PackageEndOrig)(void *, void *);
	UInt64 PackageEndHook(void *pthis, void *arg1, Actor *actor, int PackID)
	{
		//static RelocAddr<_PackageEndOrig> PackageEndOrig(0x00C4EB70);
		RelocAddr<_PackageEndOrig> PackageEndOrig((std::uintptr_t)Offset_PackageEndOrig);

		EndActorID = (actor) ? actor->formID : 0;
		EndPackID = PackID;

		PackageEnded();

		return PackageEndOrig(pthis, arg1);
	}

	void InitPlugin() {

		//static RelocAddr <uintptr_t> PackageStart_Enter(0x00600A70 + 0x47);
		//static RelocAddr <uintptr_t> PackageEnd_Enter(0x00955020 + 0x103);
		RelocAddr <uintptr_t> PackageStart_Enter((std::uintptr_t)Offset_PackageStart_Enter);
		RelocAddr <uintptr_t> PackageEnd_Enter((std::uintptr_t)Offset_PackageEnd_Enter);

		g_branchTrampoline.Write5Branch(PackageStart_Enter, (uintptr_t)PackageStartHook);

		{
			struct PackageEndHook_Code : Xbyak::CodeGenerator {
				PackageEndHook_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
				{
					// Extra parameters as r8, r9
					mov(r8, qword[rsi]);
					mov(r9, dword[rsi + 8]);

					jmp(ptr[rip]);
					dq((uintptr_t)PackageEndHook);
				}
			};

			void * codeBuf = g_localTrampoline.StartAlloc();
			PackageEndHook_Code code(codeBuf);
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write5Call(PackageEnd_Enter, (uintptr_t)code.getCode());
		}
	}

} // PackageData
