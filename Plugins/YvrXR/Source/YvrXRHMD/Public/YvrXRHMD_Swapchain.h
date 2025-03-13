// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HeadMountedDisplayBase.h"
#include "XRSwapChain.h"
#include "YvrXRPlatformRHI.h"

#include "openxr.h"

class FYvrXRSwapchain : public FXRSwapChain
{
public:
	FYvrXRSwapchain(TArray<FTextureRHIRef>&& InRHITextureSwapChain, const FTextureRHIRef& InRHITexture, XrSwapchain InHandle = XrSwapchain());
	virtual ~FYvrXRSwapchain();

#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
	virtual void IncrementSwapChainIndex_RHIThread() override final;
	virtual void WaitCurrentImage_RHIThread(int64 Timeout) override final;
#else
	virtual void IncrementSwapChainIndex_RHIThread(int64 Timeout = 0) override final;
#endif

	virtual void ReleaseCurrentImage_RHIThread() override final;

	XrSwapchain GetHandle() { return Handle; }
	static XrSwapchain CreateSwapchain(XrSession InSession, uint32 PlatformFormat, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, uint32 FaceCount = 1);
	static uint8 GetNearestSupportedSwapchainFormat(XrSession InSession, uint8 RequestedFormat, TFunction<uint32(uint8)> ToPlatformFormat = nullptr);

protected:
	XrSwapchain Handle;
	std::atomic<bool> Acquired;
};

#ifdef XR_USE_GRAPHICS_API_OPENGL_ES
FTextureRHIRef CreateTexture_OpenGL(uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, FClearValueBinding InBinding, uint32 InNumMips, uint32 InNumSamples, uint32 InNumSamplesTileMem, ERHIResourceType InResourceType, uint32 InTexture, ETextureCreateFlags InTexCreateFlags);
FXRSwapChainPtr CreateSwapchain_OpenGL(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding, uint32 FaceCount = 1);
#endif
#ifdef XR_USE_GRAPHICS_API_VULKAN
FTextureRHIRef CreateTexture_Vulkan(uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, FClearValueBinding InBinding, uint32 InNumMips, uint32 InNumSamples, uint32 InNumSamplesTileMem, ERHIResourceType InResourceType, VkImage InTexture, ETextureCreateFlags InTexCreateFlags);
FXRSwapChainPtr CreateSwapchain_Vulkan(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding, uint32 FaceCount = 1);
TArray<FXRSwapChainPtr> CreateSwapchainWithFoveation_Vulkan(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding);
#endif