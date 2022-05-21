#pragma once

class ExtraRefrPath : public BSExtraData {
public:
	UInt64	unk18;
	UInt64	unk20;
	UInt64	unk28;
	UInt64	unk30;
	float	rot;
	float	x;		// 3C
	float	y;		// 40
	float	z;		// 44
};

struct ActorData {
	UInt32			formId;
	Actor*			actor;
	UInt64			sceneId;
	UInt32			posIndex;
	ExtraRefrPath*	extraRefPath;
	NiPoint3		originPos;
	NiPoint3		offset;
};

struct SceneData {
	UInt64					sceneId;
	std::string				position;
	std::vector<UInt32>		actorList;
};

namespace Positioner {
	ActorData* GetActorDataByFormId(UInt32 formId);
	void ResetPositioner();
	void RegisterPositionerFunctions(VirtualMachine* vm);
}