#include "Global.h"

RelocPtr <ActorValue> g_actorValue(0x58CFFF0);

ModifiedValues  g_modifiedSpeedMult = { 0 };

void ModifyAnimMult() {
	if (!*g_player || !g_actorValue || !g_actorValue->animationMult || !g_actorValue->speedMult)
		return;

	ModifiedValues speedMultDiff = { 0 };
	speedMultDiff.base = (*g_player)->actorValueOwner.GetBase(g_actorValue->speedMult) - DEFAULT_ANIM_MULT;
	speedMultDiff.temp = (*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->speedMult) - DEFAULT_MOD_VALUE;
	speedMultDiff.perm = (*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Perm, g_actorValue->speedMult) - DEFAULT_MOD_VALUE;
	speedMultDiff.damage = (*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Damage, g_actorValue->speedMult) - DEFAULT_MOD_VALUE;

	float totalSpeedMultDiff = speedMultDiff.GetTotalSum();
	if (totalSpeedMultDiff == 0.0f)
		return;

	g_modifiedSpeedMult += speedMultDiff;
	(*g_player)->actorValueOwner.Mod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->animationMult, totalSpeedMultDiff);
	(*g_player)->actorValueOwner.Mod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->speedMult, -totalSpeedMultDiff);

#ifdef LOGGING
	_MESSAGE("ModifyAnimMult animMult Value[%f] Base[%f] Mod[%f][%f][%f]",
		(*g_player)->actorValueOwner.GetValue(g_actorValue->animationMult),
		(*g_player)->actorValueOwner.GetBase(g_actorValue->animationMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->animationMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Perm, g_actorValue->animationMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Damage, g_actorValue->animationMult)
	);
	_MESSAGE("ModifyAnimMult speedMult Value[%f] Base[%f] Mod[%f][%f][%f]",
		(*g_player)->actorValueOwner.GetValue(g_actorValue->speedMult),
		(*g_player)->actorValueOwner.GetBase(g_actorValue->speedMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->speedMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Perm, g_actorValue->speedMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Damage, g_actorValue->speedMult)
	);
#endif
}

void RestoreAnimMult() {
	if (!g_modifiedSpeedMult.IsModified())
		return;

	if (!*g_player || !g_actorValue || !g_actorValue->animationMult || !g_actorValue->speedMult)
		return;

	float restoreVal = g_modifiedSpeedMult.GetTotalSum();
	(*g_player)->actorValueOwner.Mod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->animationMult, -restoreVal);
	(*g_player)->actorValueOwner.Mod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->speedMult, restoreVal);
	g_modifiedSpeedMult.ClearValues();

#ifdef LOGGING
	_MESSAGE("RestoreAnimMult animMult Value[%f] Base[%f] Mod[%f][%f][%f]",
		(*g_player)->actorValueOwner.GetValue(g_actorValue->animationMult),
		(*g_player)->actorValueOwner.GetBase(g_actorValue->animationMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->animationMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Perm, g_actorValue->animationMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Damage, g_actorValue->animationMult)
	);
	_MESSAGE("RestoreAnimMult speedMult Value[%f] Base[%f] Mod[%f][%f][%f]",
		(*g_player)->actorValueOwner.GetValue(g_actorValue->speedMult),
		(*g_player)->actorValueOwner.GetBase(g_actorValue->speedMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->speedMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Perm, g_actorValue->speedMult),
		(*g_player)->actorValueOwner.GetMod(ActorValue::ACTOR_VALUE_MODIFIER::Damage, g_actorValue->speedMult)
	);
#endif
}