#pragma once

struct SKSESerializationInterface;

namespace Data {
	void FormDelete(UInt64 handle);
	void Serialization_Load(SKSESerializationInterface* intfc);
	void Serialization_Save(SKSESerializationInterface* intfc);
	void Serialization_Revert(SKSESerializationInterface* intfc);
}
