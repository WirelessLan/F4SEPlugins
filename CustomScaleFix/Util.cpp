#include "Global.h"

RelocAddr <_GetScale> GetScale(0x3F8540);

bool IsActorFemale(Actor* actor) {
	TESNPC* actorBase = DYNAMIC_CAST(actor->baseForm, TESForm, TESNPC);
	if (!actorBase)
		return false;

	return CALL_MEMBER_FN(actorBase, GetSex)() == 1 ? true : false;
}

void SetRefScale(Actor* actor, float scale, bool isFemale) {
	if (!actor || !actor->race)
		return;

	float baseScale[2];
	memcpy(baseScale, actor->race->unk170, sizeof(float) * 2);

	float modifiedRefScale = std::round(scale * (100.0f / baseScale[isFemale]));
	std::uint16_t refScale = static_cast<std::uint16_t>(modifiedRefScale);
	memcpy(&actor->unk104, &refScale, sizeof std::uint16_t);
}

template <typename T>
T GetOffset(const void* baseObject, int offset) {
	return *reinterpret_cast<T*>((uintptr_t)baseObject + offset);
}

TESForm* GetFormFromIdentifier(const std::string& identifier) {
	auto delimiter = identifier.find('|');
	if (delimiter != std::string::npos) {
		std::string modName = identifier.substr(0, delimiter);
		std::string modForm = identifier.substr(delimiter + 1);

		const ModInfo* mod = (*g_dataHandler)->LookupModByName(modName.c_str());
		if (mod && mod->modIndex != -1) {
			UInt32 formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
			UInt32 flags = GetOffset<UInt32>(mod, 0x334);
			if (flags & (1 << 9)) {
				// ESL
				formID &= 0xFFF;
				formID |= 0xFE << 24;
				formID |= GetOffset<UInt16>(mod, 0x372) << 12;	// ESL load order
			}
			else {
				formID |= (mod->modIndex) << 24;
			}
			return LookupFormByID(formID);
		}
	}
	return nullptr;
}