#include "Global.h"

namespace PositionData {
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

		Utility::trim(retVal);
		return retVal;
	}

	std::vector<Data> LoadPositionData(const std::string& position) {
		std::string posPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\" + position + ".txt";
		std::ifstream posFile(posPath);

		std::vector<Data> result;

		if (!posFile.is_open())
			return result;

		std::string line;
		std::string indexStr, offX, offY, offZ;
		while (std::getline(posFile, line)) {
			Utility::trim(line);
			if (line.length() == 0 || line[0] == '#')
				continue;

			UInt32 index = 0;

			indexStr = GetNextData(line, index, '|');
			if (indexStr.empty()) {
				_MESSAGE("Cannot read position index[%s]", line.c_str());
				continue;
			}

			offX = GetNextData(line, index, ',');
			if (offX.length() == 0) {
				_MESSAGE("Cannot read offset X[%s]", line.c_str());
				continue;
			}

			offY = GetNextData(line, index, ',');
			if (offY.length() == 0) {
				_MESSAGE("Cannot read offset Y[%s]", line.c_str());
				continue;
			}

			offZ = GetNextData(line, index, 0);
			if (offZ.length() == 0) {
				_MESSAGE("Cannot read offset Z[%s]", line.c_str());
				continue;
			}

			result.push_back({ stoul(indexStr), NiPoint3(stof(offX), stof(offY), stof(offZ)) });
		}

		return result;
	}

	bool SavePositionData(const SceneData* scene) {
		std::string posPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\" + scene->position + ".txt";
		std::ofstream posFile(posPath);

		if (!posFile.is_open())
			return false;

		for (UInt32 formId : scene->actorList) {
			ActorData* actorData = Positioner::GetActorDataByFormId(formId);
			if (!actorData)
				return false;

			posFile << actorData->posIndex << "|" << actorData->offset.x << "," << actorData->offset.y << "," << actorData->offset.z << std::endl;
		}

		return true;
	}
}