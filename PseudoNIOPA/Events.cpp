#include "Global.h"

namespace Events {
	void Attach_Events() {
		GetEventDispatcher<TESObjectLoadedEvent>()->AddEventSink(new UnloadedActorManager());
	}

	EventResult UnloadedActorManager::ReceiveEvent(TESObjectLoadedEvent* evn, void* dispatcher)	{
		if (!evn->loaded) {
			auto it = g_modifiedMap.find(evn->formId);
			if (it != g_modifiedMap.end()) {
				g_modifiedMap.erase(it);

				if (g_selectedActor && evn->formId == g_selectedActor->formID) {
					g_selectedActor = nullptr;
					g_selectedNode = nullptr;
				}
			}
		}

		return kEvent_Continue;
	}
}
