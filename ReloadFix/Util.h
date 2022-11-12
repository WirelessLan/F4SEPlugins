#pragma once

bool IsSprinting();
bool IsFirstPerson();
bool IsWeaponDrawn();
bool IsVanityModeEnabled();
void ToggleVanityMode(bool enable);

bool IsWeaponReloadable();

#define BUTTON_UP 0.0f
#define BUTTON_HOLD_TIMER 0.4f

bool IsButtonPressed(ButtonEvent* btnEvent);