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

	VMArray<TESForm*> LoadSlot(BSFixedString& slot) {
		std::string slotPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "_Slots\\" + std::string(slot.c_str()) + ".codss";
		_MESSAGE("LoadSlot: %s", slotPath.c_str());
		std::ifstream slotFile(slotPath);

		VMArray<TESForm*> result;

		if (!slotFile.is_open()) {
			_MESSAGE("Cannot read Slot - %s", slotPath.c_str());
			return result;
		}

		std::string line;
		std::string pluginName, formId;
		while (std::getline(slotFile, line)) {
			Util::trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			UInt32 index = 0;

			pluginName = GetNextData(line, index, '|');
			if (pluginName.empty()) {
				_MESSAGE("Cannot read Plugin Name - %s", line.c_str());
				continue;
			}

			formId = GetNextData(line, index, 0);
			if (formId.empty()) {
				_MESSAGE("Cannot read Form ID - %s", line.c_str());
				continue;
			}

			TESForm* form = Util::GetFormFromIdentifier(pluginName, formId);
			if (!form) {
				_MESSAGE("Cannot find Form - %s", line.c_str());
				continue;
			}

			result.Push(&form);
		}

		slotFile.close();

		return result;
	}

	bool SaveSlot(BSFixedString& slot) {
		return SaveSlot(std::string(slot));
	}

	bool SaveSlot(std::string & slot) {
		std::string slotPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "_Slots\\" + slot + ".codss";
		_MESSAGE("SaveSlot: %s", slotPath.c_str());
		std::ofstream slotFile(slotPath);

		Actor* actor = Util::GetCurrentConsoleActor();
		if (!actor)
			actor = *g_player;

		std::vector<TESForm*> currentEquippedItemList = Util::GetCurrentEquippedItemList(actor);
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

	bool DeleteSlot(std::string& slot) {
		std::string slotPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "_Slots\\" + slot + ".codss";
		_MESSAGE("DeleteSlot: %s", slotPath.c_str());
		return remove(slotPath.c_str()) == 0;
	}
}