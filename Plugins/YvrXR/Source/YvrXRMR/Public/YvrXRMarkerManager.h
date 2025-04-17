#pragma once

#include "CoreMinimal.h"
#include "YvrXRCore.h"
#include "YvrXRHMD.h"

class YVRXRMR_API FYvrXRMarkerManager {
public:
    FYvrXRMarkerManager();
    ~FYvrXRMarkerManager();

    static FYvrXRMarkerManager* GetInstance();
    void Initialize(FYvrXRHMD* InYvrXRHMD);
    void PollEvent(XrEventDataBuffer EventData);

    bool SetYvrMarkerEnabled(bool Enabled);

private:
    FYvrXRHMD* YvrXRHMD;

    typedef struct {
        XrSpace space;
        XrExtent2Df size;
    } Marker;
    TMap<uint64_t, Marker> markers;
};