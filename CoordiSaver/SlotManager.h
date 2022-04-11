#pragma once

namespace SlotManager {
	VMArray<TESForm*> LoadSlot(BSFixedString slot);
	bool SaveSlot(BSFixedString slot);
}