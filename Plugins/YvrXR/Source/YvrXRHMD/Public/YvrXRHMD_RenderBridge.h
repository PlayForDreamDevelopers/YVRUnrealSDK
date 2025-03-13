// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HeadMountedDisplayBase.h"
#include "XRRenderBridge.h"
#include "XRSwapChain.h"
#include "YvrXRPlatformRHI.h"

#include "openxr.h"

class FYvrXRHMD;
class FRHICommandListImmediate;

class FYvrXRRenderBridge : public FXRRenderBridge
{
public:
	FYvrXRRenderBridge()
		: AdapterLuid(0)
		, OpenXRHMD(nullptr)
	{ }

	void SetOpenXRHMD(FYvrXRHMD* InHMD) { OpenXRHMD = InHMD; }
	virtual uint64 GetGraphicsAdapterLuid() { return AdapterLuid; }

	virtual void* GetGraphicsBinding() = 0;
	virtual FXRSwapChainPtr CreateSwapchain(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding, uint32 FaceCount = 1) = 0;
	virtual TArray<FXRSwapChainPtr> CreateSwapchainWithFoveation(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding) = 0;

	/** FRHICustomPresent */
	virtual bool Present(int32& InOutSyncInterval) override;

protected:
	uint64 AdapterLuid;

private:
	FYvrXRHMD* OpenXRHMD;
};

#ifdef XR_USE_GRAPHICS_API_OPENGL_ES
FYvrXRRenderBridge* CreateRenderBridge_OpenGL(XrInstance InInstance, XrSystemId InSystem);
#endif
#ifdef XR_USE_GRAPHICS_API_VULKAN
FYvrXRRenderBridge* CreateRenderBridge_Vulkan(XrInstance InInstance, XrSystemId InSystem);
#endif
