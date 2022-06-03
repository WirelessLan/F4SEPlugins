#pragma once

struct WeaponPosition {
	bool hasPos;
	float eulerHeading;
	float eulerAttitude;
	float eulerBank;
	float posX;
	float posY;
	float posZ;
};

void AdjustPosition(TESForm* weapForm, NiNode* attachNode);
void SavePosition();