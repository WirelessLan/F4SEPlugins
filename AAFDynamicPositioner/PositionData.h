#pragma once

namespace PositionData {
	struct Data {
		UInt32		index;
		NiPoint3	offset;
	};

	std::vector<Data> LoadPositionData(const std::string& position, bool isPlayerScene);
	bool SavePositionData(const std::string& position, const std::vector<UInt32>& actorList, bool isPlayerScene);
}