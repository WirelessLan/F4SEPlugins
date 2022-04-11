#include "Global.h"

namespace SlotManager {
	char GetNextChar(const std::string& line, UInt32& index) {
		if (index < line.length())
			return line[index++];

		return -1;
	}

	std::string GetNextData(const std::string& line, UInt32& index, char delimeter) {
		char ch;
		std::string retVal = "";

		while ((ch = GetNextChar(line, index)) > 0) {
			if (ch == '#') {
				if (index > 0) index--;
				break;
			}

			if (delimeter != 0 && ch == delimeter)
				break;

			retVal += ch;
		}

		Util::trim(retVal);
		return retVal;
	}

	VMArray<TESForm*> ReadSlot(BSFixedString slot) {
		std::string slotPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "_Slots\\slot_" + std::string(slot.c_str()) + ".codss";
		std::ifstream slotFile(slotPath);

		VMArray<TESForm*> result;

		if (!slotFile.is_open())
			return result;

		std::string line;
		std::string pluginName, formId;
		while (std::getline(slotFile, line)) {
			Util::trim(line);
			if (line.length() == 0 || line[0] == '#')
				continue;

			UInt32 index = 0;

			pluginName = GetNextData(line, index, '|');
			if (pluginName.length() == 0) {
				_MESSAGE("Cannot read Plugin Name[%s]", line.c_str());
				continue;
			}

			formId = GetNextData(line, index, 0);
			if (formId.length() == 0) {
				_MESSAGE("Cannot read Form ID[%s]", line.c_str());
				continue;
			}

			TESForm* armorForm = Util::GetFormFromIdentifier(pluginName, formId);
			if (!armorForm) {
				_MESSAGE("Cannot find Armor[%s]", line.c_str());
				continue;
			}

			result.Push(&armorForm);
		}
		slotFile.close();

		return result;
	}

	bool SaveSlot(BSFixedString slot) {
		std::string slotPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "_Slots\\slot_" + std::string(slot.c_str()) + ".codss";
		std::ofstream slotFile(slotPath);

		std::vector<TESForm*> currentEquippedItemList = Util::GetCurrentEquippedItemList(*g_player);
		if (currentEquippedItemList.size() == 0)
			return false;

		if (!slotFile.is_open())
			return false;

		for (TESForm* form : currentEquippedItemList) {
			ModInfo* modInfo = form->unk08->entries[0];

			UInt32 formId = modInfo->IsLight() ? form->formID & 0xFFF : form->formID & 0xFFFFFF;
			slotFile << modInfo->name << "|" << std::uppercase << std::hex << formId << std::endl;
		}

		slotFile.close();

		return true;
	}
}