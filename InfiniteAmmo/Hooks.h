#pragma once

struct WeaponData {
	TESForm* item;					// 00
	TBO_InstanceData* instanceData;	// 08
};

void Install_Hooks();