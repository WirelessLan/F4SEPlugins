#pragma once

namespace SlotManager {
	VMArray<TESForm*> ReadSlot(BSFixedString slot);
	bool SaveSlot(BSFixedString slot);
}