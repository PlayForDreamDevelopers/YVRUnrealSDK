// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IStereoLayers.h"
#include "YvrPreviewManager.h"

#include "openvr.h"

 /**
  * Struct for managing stereo layer data.
  */
struct FYvrPreviewLayer
{
    typedef IStereoLayers::FLayerDesc FLayerDesc;
    FLayerDesc	          LayerDesc;
    vr::VROverlayHandle_t OverlayHandle;
    bool				  bUpdateTexture;

    FYvrPreviewLayer(const FLayerDesc& InLayerDesc)
  	    : LayerDesc(InLayerDesc)
  	    , OverlayHandle(vr::k_ulOverlayHandleInvalid)
  	    , bUpdateTexture(false)
    {}

    // Required by TStereoLayerManager:
    void SetLayerId(uint32 InId) { LayerDesc.SetLayerId(InId); }
    uint32 GetLayerId() const { return LayerDesc.GetLayerId(); }
    friend bool GetLayerDescMember(const FYvrPreviewLayer& Layer, FLayerDesc& OutLayerDesc);
    friend void SetLayerDescMember(FYvrPreviewLayer& Layer, const FLayerDesc& InLayerDesc);
    friend void MarkLayerTextureForUpdate(FYvrPreviewLayer& Layer);
};