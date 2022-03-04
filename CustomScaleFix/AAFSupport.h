#pragma once

struct TESSceneActionEvent {
	UInt32 sceneFormID;		// 00
	UInt32 actionIndex;		// 04
};

class SceneActionEventReceiver : public BSTEventSink<TESSceneActionEvent> {
protected:
	virtual	EventResult ReceiveEvent(TESSceneActionEvent* evn, void* dispatcher) override;
};

DECLARE_EVENT_DISPATCHER(TESSceneActionEvent, 0x004438B0)