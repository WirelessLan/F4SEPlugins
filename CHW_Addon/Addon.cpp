#include "Global.h"

std::unordered_map<std::string, std::unordered_map<UInt32, WeaponPosition>> g_weapPosMap;

void trim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

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

		if (delimeter != NULL && ch == delimeter)
			break;

		retVal += ch;
	}

	trim(retVal);
	return retVal;
}

bool ReadWeaponPositionFile(std::string pluginName, UInt32 formId, WeaponPosition& weapPos) {
	std::string weapPosPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\" + pluginName + "\\" + std::to_string(formId) + ".txt";
	std::ifstream weapPosFile(weapPosPath);

	if (!weapPosFile.is_open())
		return false;

	std::string line;
	std::string eulerHeading, eulerAttitude, eulerBank, posX, posY, posZ;
	while (std::getline(weapPosFile, line)) {
		trim(line);
		if (line.length() == 0 || line[0] == '#')
			continue;

		UInt32 index = 0;

		eulerHeading = GetNextData(line, index, '|');
		if (eulerHeading.empty()) {
			_MESSAGE("Cannot read eulerHeading[%s]", line.c_str());
			continue;
		}

		eulerAttitude = GetNextData(line, index, '|');
		if (eulerAttitude.empty()) {
			_MESSAGE("Cannot read eulerAttitude[%s]", line.c_str());
			continue;
		}

		eulerBank = GetNextData(line, index, '|');
		if (eulerBank.empty()) {
			_MESSAGE("Cannot read eulerBank[%s]", line.c_str());
			continue;
		}

		posX = GetNextData(line, index, '|');
		if (posX.empty()) {
			_MESSAGE("Cannot read posX[%s]", line.c_str());
			continue;
		}

		posY = GetNextData(line, index, '|');
		if (posY.empty()) {
			_MESSAGE("Cannot read posY[%s]", line.c_str());
			continue;
		}

		posZ = GetNextData(line, index, NULL);
		if (posZ.empty()) {
			_MESSAGE("Cannot read posZ[%s]", line.c_str());
			continue;
		}

		weapPos.eulerHeading = std::stof(eulerHeading);
		weapPos.eulerAttitude = std::stof(eulerAttitude);
		weapPos.eulerBank = std::stof(eulerBank);
		weapPos.posX = std::stof(posX);
		weapPos.posY = std::stof(posY);
		weapPos.posZ = std::stof(posZ);

		weapPos.hasPos = true;

		return true;
	}

	return false;
}

bool WriteWeaponPositionFile(std::string pluginName, UInt32 formId, const WeaponPosition& weapPos) {
	std::string weapPosPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\" + pluginName + "\\" + std::to_string(formId) + ".txt";
	std::ofstream weapPosFile(weapPosPath);

	if (!weapPosFile.is_open())
		return false;

	weapPosFile << weapPos.eulerHeading << '|' << weapPos.eulerAttitude << '|' << weapPos.eulerBank << '|' << weapPos.posX << '|' << weapPos.posY << '|' << weapPos.posZ;
	return true;
}

WeaponPosition* GetWeaponPosition(std::string pluginName, UInt32 formId) {
	auto weapPosMapIt = g_weapPosMap.find(pluginName);
	if (weapPosMapIt == g_weapPosMap.end())
		return nullptr;

	auto pluginPosMapIt = weapPosMapIt->second.find(formId);
	if (pluginPosMapIt == weapPosMapIt->second.end())
		return nullptr;

	return &pluginPosMapIt->second;
}

WeaponPosition* GetWeaponPosition(TESForm* weapForm) {
	ModInfo* modInfo = weapForm->unk08->entries[0];
	if (!modInfo)
		return nullptr;

	WeaponPosition weapPos = WeaponPosition();
	UInt32 formId = modInfo->IsLight() ? weapForm->formID & 0xFFF : weapForm->formID & 0xFFFFFF;

	auto weapPosMapIt = g_weapPosMap.find(modInfo->name);
	if (weapPosMapIt != g_weapPosMap.end()) {
		auto pluginPosMapIt = weapPosMapIt->second.find(formId);
		if (pluginPosMapIt != weapPosMapIt->second.end())
			return &pluginPosMapIt->second;

		ReadWeaponPositionFile(modInfo->name, formId, weapPos);

		weapPosMapIt->second.insert(std::make_pair(formId, weapPos));
		return GetWeaponPosition(modInfo->name, formId);
	}

	ReadWeaponPositionFile(modInfo->name, formId, weapPos);

	std::unordered_map<UInt32, WeaponPosition> newPluginPosMap;
	newPluginPosMap.insert(std::make_pair(formId, weapPos));
	g_weapPosMap.insert(std::make_pair(modInfo->name, newPluginPosMap));

	return GetWeaponPosition(modInfo->name, formId);
}

void AdjustPosition(TESForm* weapForm, NiNode* attachNode) {
	if (!weapForm || !attachNode)
		return;

	WeaponPosition* weapPos = GetWeaponPosition(weapForm);
	if (!weapPos || !weapPos->hasPos)
		return;

	attachNode->m_localTransform.rot.SetEulerAngles(weapPos->eulerHeading, weapPos->eulerAttitude, weapPos->eulerBank);
	attachNode->m_localTransform.pos.x = weapPos->posX;
	attachNode->m_localTransform.pos.y = weapPos->posY;
	attachNode->m_localTransform.pos.z = weapPos->posZ;
}

void SavePosition() {
	TESForm** weapForm = reinterpret_cast<TESForm**>(CHWAddress(0x91FD8));
	NiNode** attachNode = reinterpret_cast<NiNode**>(CHWAddress(0x92390));

	if (!weapForm || !*weapForm)
		return;

	if (!attachNode || !*attachNode)
		return;

	WeaponPosition* weapPos = GetWeaponPosition(*weapForm);
	if (!weapPos)
		return;

	(*attachNode)->m_localTransform.rot.GetEulerAngles(&weapPos->eulerHeading, &weapPos->eulerAttitude, &weapPos->eulerBank);
	weapPos->posX = (*attachNode)->m_localTransform.pos.x;
	weapPos->posY = (*attachNode)->m_localTransform.pos.y;
	weapPos->posZ = (*attachNode)->m_localTransform.pos.z;

	weapPos->hasPos = true;

	ModInfo* modInfo = (*weapForm)->unk08->entries[0];
	if (!modInfo)
		return;

	UInt32 formId = modInfo->IsLight() ? (*weapForm)->formID & 0xFFF : (*weapForm)->formID & 0xFFFFFF;

	WriteWeaponPositionFile(modInfo->name, formId, *weapPos);

	ShowNotification("Weapon Position Saved!");
}