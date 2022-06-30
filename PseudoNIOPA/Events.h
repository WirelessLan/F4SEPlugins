#pragma once
namespace Events {
	class UnloadedActorManager : public BSTEventSink<TESObjectLoadedEvent> {
	protected:
		virtual	EventResult ReceiveEvent(TESObjectLoadedEvent* evn, void* dispatcher) override;
	};

	void Attach_Events();
};

