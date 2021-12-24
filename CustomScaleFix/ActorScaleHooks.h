#pragma once

class BGSZoomData : public TESForm {
public:
    struct Data {
    public:
        // members
        float fovMult;              // 00
        std::uint32_t overlay;      // 04
        std::uint32_t isModFormID;  // 08
        NiPoint3 cameraOffset;      // 0C
    };

    // members
    Data zoomData;                 // 20
    TESImageSpaceModifier* isMod;  // 38
};

void Hooks_ActorScaleChanged();
void Hooks_GetZoomData();