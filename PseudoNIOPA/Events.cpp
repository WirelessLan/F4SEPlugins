#include "Global.h"

namespace Events {
	class UnloadedActorManager : public BSTEventSink<TESObjectLoadedEvent> {
	protected:
		virtual	EventResult ReceiveEvent(TESObjectLoadedEvent* a_evn, void* a_dispatcher) override {
			if (a_evn->loaded)
				return kEvent_Continue;

			auto it = g_modifiedMap.find(a_evn->formId);
			if (it == g_modifiedMap.end())
				return kEvent_Continue;

			g_modifiedMap.erase(it);

			if (g_selectedActor && a_evn->formId == g_selectedActor->formID) {
				g_selectedActor = nullptr;
				g_selectedNode = nullptr;
			}

			return kEvent_Continue;
		}
	};

	void Attach_Events() {
		GetEventDispatcher<TESObjectLoadedEvent>()->AddEventSink(new UnloadedActorManager());
	}
}
