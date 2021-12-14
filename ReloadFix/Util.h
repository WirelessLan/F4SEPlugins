#pragma once

bool IsReloading();
bool IsWeaponReloadable();
bool IsSprinting();
bool IsThirdPerson();
bool IsWeaponDrawn();
bool IsButtonPressed(ButtonEvent* btnEvent);
bool IsHoldingButton(ButtonEvent* btnEvent);

void ToggleSprint(bool a_sprint);