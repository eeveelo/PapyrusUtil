#pragma once

#include <sstream>
#include <boost/array.hpp>
#include <boost/container/flat_map.hpp>

#include "common/ICriticalSection.h"
#include "skse64/GameForms.h"
#include "skse64/GameObjects.h"

namespace PackageData {
	
	class Packages{
	public:
		ICriticalSection s_dataLock;
		
		//typedef boost::array<UInt32, 2> Flags;
		typedef std::pair<UInt32, UInt32> Flags;
		typedef boost::container::flat_map<UInt32, Flags> ActorPackages;
		typedef boost::container::flat_map<UInt32, ActorPackages> PackageMap;
		PackageMap Data;

		inline ActorPackages* GetActor(TESForm* FormRef){
			PackageMap::iterator itr = Data.find(FormRef->formID);
			return itr == Data.end() ? NULL : &itr->second;
		}
		inline ActorPackages* GetActor(UInt32 &formID){
			PackageMap::iterator itr = Data.find(formID);
			return itr == Data.end() ? NULL : &itr->second;
		}

		// Interface
		void AddPackage(Actor* ActorRef, TESPackage* PackageRef, UInt32 priority, UInt32 flags);
		bool RemovePackage(Actor* ActorRef, TESPackage* PackageRef);
		UInt32 CountPackages(Actor* ActorRef);
		UInt32 ClearActor(Actor* ActorRef);
		UInt32 ClearPackage(TESPackage* PackageRef);

		// System Handling
		void PackageEnded();
		bool IsValidPackage(TESPackage *PackageID, Actor *ActorID);
		TESPackage *DecidePackage(Actor *ActorID, TESPackage *PackageID);

		// Serialization
		void LoadStream(std::stringstream &ss);
		void SaveStream(std::stringstream &ss);
		void Revert();
	};

	Packages* GetPackages();
	void InitPlugin();

}// PackageData