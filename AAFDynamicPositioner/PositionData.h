#pragma once

namespace PositionData {
	struct Data {
		UInt32		index;
		NiPoint3	offset;
	};

	std::vector<Data> LoadPositionData(const std::string& position);
	bool SavePositionData(const SceneData* scene);
}