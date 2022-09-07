#pragma once

namespace ExpressionManager {
	class BSFaceGenAnimationData : public NiExtraData {
	public:
		UInt64	unk18[(0xF0 - 0x18) >> 3];	// 18
		float	exprs[54];					// F0
	};

	BSFaceGenAnimationData* GetFaceGenAnimationData(Actor* actor);
	void SetFaceGenMorphs(Actor* actor, UInt32 a_idx, float value);
	void ResetExpression(Actor* actor);
	bool SaveExpression(const std::string& saveName, BSFaceGenAnimationData* fgAnimData);
	std::vector<std::string> GetExpressions();
	bool LoadExpression(Actor* actor, const std::string& saveName);
	bool DeleteExpression(const std::string& saveName);
}