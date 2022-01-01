#pragma once
#include <f4se/GameReferences.h>

struct TESEquipEvent {
	TESObjectREFR* ref;	// 00
	UInt32 formId;		// 08
	UInt32 unk0C;		// 0C
	UInt16 unk10;		// 10
	bool isEquipping;	// 12
};

DECLARE_EVENT_DISPATCHER(TESEquipEvent, 0x00442870);

class ItemEquipEvent : public BSTEventSink<TESEquipEvent> {
protected:
	virtual	EventResult ReceiveEvent(TESEquipEvent* evn, void* dispatcher) override;
};

void Install_Events();