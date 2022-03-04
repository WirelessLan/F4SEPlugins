#pragma once

class BGSZoomData : public TESForm {
public:
    struct Data {
    public:
        float fovMult;              // 00
        std::uint32_t overlay;      // 04
        std::uint32_t isModFormID;  // 08
        NiPoint3 cameraOffset;      // 0C
    };

    Data zoomData;                 // 20
    TESImageSpaceModifier* isMod;  // 38
};

typedef NiPoint3* (*_GetZoomData)(BGSZoomData*);

void Hooks_GetZoomData();