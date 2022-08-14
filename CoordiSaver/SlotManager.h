#pragma once

namespace SlotManager {
	VMArray<TESForm*> LoadSlot(BSFixedString& slot);
	bool SaveSlot(BSFixedString& slot);
	bool SaveSlot(std::string& slot);
	bool DeleteSlot(std::string& slot);
}