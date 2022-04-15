#pragma once

struct FurnitureKeywordOffset {
    BGSKeyword* keyword;
    float horizontal;
    float vertical;
};

void Hooks_GetFurniturePosition();

bool ShouldLoadFurnitureConfig();
void LoadFurnitureConfig();