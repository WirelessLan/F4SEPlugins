#pragma once

class ExtraRefrPath : public BSExtraData {
public:
	NiPoint3	startPos;	// 18
	NiPoint3	startTan;	// 24
	NiPoint3	startEuler;	// 30
	NiPoint3	goalPos;	// 3C
	NiPoint3	goalTan;	// 48
	NiPoint3	goalEuler;	// 54
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
	void SetOffset(const std::string& axis, float offset);
}