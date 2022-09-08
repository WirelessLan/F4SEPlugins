#include "Global.h"

namespace ExpressionManager {
	BSFaceGenAnimationData* GetFaceGenAnimationData(Actor* actor) {
		if (!actor || !actor->middleProcess || !actor->middleProcess->unk08)
			return nullptr;

		BSFaceGenAnimationData* faceGenAnimData = reinterpret_cast<BSFaceGenAnimationData*>(actor->middleProcess->unk08->unk3B0[3]);
		return faceGenAnimData;
	}

	void SetFaceGenMorphs(Actor* actor, UInt32 a_idx, float value) {
		if (a_idx >= 54)
			return;

		if (value < 0.0f || value > 1.0f)
			return;

		BSFaceGenAnimationData* fgAnimData = GetFaceGenAnimationData(actor);
		if (!fgAnimData)
			return;

		fgAnimData->exprs[a_idx] = value;
	}

	void ResetExpression(Actor* actor) {
		BSFaceGenAnimationData* fgAnimData = GetFaceGenAnimationData(actor);
		if (!fgAnimData)
			return;

		for (UInt32 ii = 0; ii < 54; ii++)
			fgAnimData->exprs[ii] = 0.0f;
	}

	bool SaveExpression(const std::string& saveName, BSFaceGenAnimationData* fgAnimData) {
		std::string savePath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Expressions\\" + saveName + ".exp";
		std::ofstream saveFile(savePath);

		if (!saveFile.is_open())
			return false;

		for (UInt32 ii = 0; ii < 54; ii++)
			saveFile << ii << "|" << int(fgAnimData->exprs[ii] * 100) << std::endl;

		return true;
	}

	std::vector<std::string> GetExpressions() {
		std::string directory = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Expressions\\";
		const char* match = "*.exp";
		std::vector<std::string> retVec;

		for (IDirectoryIterator iter(directory.c_str(), match); !iter.Done(); iter.Next()) {
			WIN32_FIND_DATA* fileData = iter.Get();

			std::string s_fileName = fileData->cFileName;
			size_t lastDotIdx = s_fileName.find_last_of(".");
			std::string rawName = s_fileName.substr(0, lastDotIdx);

			retVec.push_back(rawName);
		}

		return retVec;
	}

	std::vector<std::string> GetExpressionNames() {
		std::string expNamesPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "_ExpNames.cfg";
		std::ifstream expNamesFile(expNamesPath);
		std::vector<std::string> retVec;

		for (UInt32 ii = 0; ii < 54; ii++)
			retVec.push_back(std::to_string(ii));

		if (!expNamesFile.is_open())
			return retVec;

		std::string line;
		std::string expIndexStr, expName;
		while (std::getline(expNamesFile, line)) {
			Utility::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			UInt32 index = 0;

			expIndexStr = Utility::GetNextData(line, index, '|');
			if (expIndexStr.empty()) {
				_MESSAGE("Cannot read Expression Index: %s", line.c_str());
				continue;
			}

			UInt32 expIdx = std::stoi(expIndexStr);
			if (expIdx >= 54) {
				_MESSAGE("Invalid Expression Index: %s", line.c_str());
				continue;
			}

			expName = Utility::GetNextData(line, index, 0);
			if (expName.empty()) {
				_MESSAGE("Cannot read Expression Name: %s", line.c_str());
				continue;
			}

			retVec[expIdx] = expName;
		}

		return retVec;
	}

	bool LoadExpression(Actor* actor, const std::string& saveName) {
		std::string savePath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Expressions\\" + saveName + ".exp";
		std::ifstream saveFile(savePath);

		if (!saveFile.is_open())
			return false;

		BSFaceGenAnimationData* fgAnimData = GetFaceGenAnimationData(actor);
		if (!fgAnimData)
			return false;

		std::string line;
		std::string expIndexStr, expValueStr;
		while (std::getline(saveFile, line)) {
			Utility::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			UInt32 index = 0;

			expIndexStr = Utility::GetNextData(line, index, '|');
			if (expIndexStr.empty()) {
				_MESSAGE("Cannot read a Index: %s", line.c_str());
				continue;
			}

			expValueStr = Utility::GetNextData(line, index, 0);
			if (expValueStr.empty()) {
				_MESSAGE("Cannot read a Value: %s", line.c_str());
				continue;
			}

			UInt32 expIndex = std::stoi(expIndexStr);
			if (expIndex >= 54) {
				_MESSAGE("Invalid Expression Index: %s", line.c_str());
				continue;
			}

			UInt32 expValue = std::stoi(expValueStr);
			if (expValue > 100) {
				_MESSAGE("Invalid Expression Value: %s", line.c_str());
				continue;
			}

			fgAnimData->exprs[expIndex] = expValue / 100.0f;
		}

		return true;
	}

	bool DeleteExpression(const std::string& saveName) {
		std::string savePath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Expressions\\" + saveName + ".exp";
		return remove(savePath.c_str()) == 0;
	}
}