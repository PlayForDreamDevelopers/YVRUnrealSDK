// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRHMD_Swapchain.h"
#include "YvrXRCore.h"
#include "XRThreadUtils.h"

static TAutoConsoleVariable<int32> CVarOpenXRSwapchainRetryCount(
	TEXT("vr.OpenXRSwapchainRetryCount"),
	9,
	TEXT("Number of times the OpenXR plugin will attempt to wait for the next swapchain image."),
	ECVF_RenderThreadSafe);

FYvrXRSwapchain::FYvrXRSwapchain(TArray<FTextureRHIRef>&& InRHITextureSwapChain, const FTextureRHIRef& InRHITexture, XrSwapchain InHandle)
	: FXRSwapChain(MoveTemp(InRHITextureSwapChain), InRHITexture)
	, Handle(InHandle)
	, Acquired(false)
{
}

FYvrXRSwapchain::~FYvrXRSwapchain()
{
	XR_ENSURE(xrDestroySwapchain(Handle));
}

#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
void FYvrXRSwapchain::IncrementSwapChainIndex_RHIThread()
{
	check(IsInRenderingThread() || IsInRHIThread());

	if (Acquired)
	{
		return;
	}

	SCOPED_NAMED_EVENT(AcquireImage, FColor::Red);

	XrSwapchainImageAcquireInfo Info;
	Info.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;
	Info.next = nullptr;
	uint32_t SwapChainIndex = 0;
	XR_ENSURE(xrAcquireSwapchainImage(Handle, &Info, &SwapChainIndex));

	GDynamicRHI->RHIAliasTextureResources((FTextureRHIRef&)RHITexture, (FTextureRHIRef&)RHITextureSwapChain[SwapChainIndex]);
	Acquired = true;
}

void FYvrXRSwapchain::WaitCurrentImage_RHIThread(int64 Timeout)
{
	check(IsInRenderingThread() || IsInRHIThread());

	SCOPED_NAMED_EVENT(WaitImage, FColor::Red);

	XrSwapchainImageWaitInfo WaitInfo;
	WaitInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
	WaitInfo.next = nullptr;
	WaitInfo.timeout = Timeout;

	XrResult WaitResult = XR_SUCCESS;
	int RetryCount = CVarOpenXRSwapchainRetryCount.GetValueOnAnyThread();
	do
	{
		XR_ENSURE(WaitResult = xrWaitSwapchainImage(Handle, &WaitInfo));
		if (WaitResult == XR_TIMEOUT_EXPIRED) //-V547
		{
			UE_LOG(LogHMD, Warning, TEXT("Timed out waiting on swapchain image %u! Attempts remaining %d."), SwapChainIndex_RHIThread.load(), RetryCount);
		}
	} while (WaitResult == XR_TIMEOUT_EXPIRED && RetryCount-- > 0);

	if (WaitResult != XR_SUCCESS) //-V547
	{
		// We can't continue without acquiring a new swapchain image since we won't have an image available to render to.
		UE_LOG(LogHMD, Fatal, TEXT("Failed to wait on acquired swapchain image. This usually indicates a problem with the OpenXR runtime."));
	}
}

#else
void FYvrXRSwapchain::IncrementSwapChainIndex_RHIThread(int64 Timeout)
{
	check(IsInRenderingThread() || IsInRHIThread());

	if (Acquired)
		return;

	XrSwapchainImageAcquireInfo Info;
	Info.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;
	Info.next = nullptr;
	XR_ENSURE(xrAcquireSwapchainImage(Handle, &Info, &SwapChainIndex_RHIThread));

	Acquired = true;

	XrSwapchainImageWaitInfo WaitInfo;
	WaitInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
	WaitInfo.next = nullptr;
	WaitInfo.timeout = Timeout;

	XrResult WaitResult = XR_SUCCESS;
	int RetryCount = 3;
	do
	{
		XR_ENSURE(WaitResult = xrWaitSwapchainImage(Handle, &WaitInfo));
		if (WaitResult == XR_TIMEOUT_EXPIRED)	//-V547
		{
			UE_LOG(LogHMD, Warning, TEXT("Timed out waiting on swapchain image %u! Attempts remaining %d."), SwapChainIndex_RHIThread, RetryCount);
		}
	} while (WaitResult == XR_TIMEOUT_EXPIRED && RetryCount-- > 0);

	if (WaitResult != XR_SUCCESS) //-V547
	{
		// We can't continue without acquiring a new swapchain image since we won't have an image available to render to.
		UE_LOG(LogHMD, Fatal, TEXT("Failed to wait on acquired swapchain image. This usually indicates a problem with the OpenXR runtime."));
	}

	GDynamicRHI->RHIAliasTextureResources((FTextureRHIRef&)RHITexture, (FTextureRHIRef&)RHITextureSwapChain[SwapChainIndex_RHIThread]);
}
#endif

void FYvrXRSwapchain::ReleaseCurrentImage_RHIThread()
{
	check(IsInRenderingThread() || IsInRHIThread());

	if (!Acquired)
	{
		return;
	}

	SCOPED_NAMED_EVENT(ReleaseImage, FColor::Red);

	XrSwapchainImageReleaseInfo ReleaseInfo;
	ReleaseInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO;
	ReleaseInfo.next = nullptr;
	XR_ENSURE(xrReleaseSwapchainImage(Handle, &ReleaseInfo));

	Acquired = false;
}

uint8 FYvrXRSwapchain::GetNearestSupportedSwapchainFormat(XrSession InSession, uint8 RequestedFormat, TFunction<uint32(uint8)> ToPlatformFormat /*= nullptr*/)
{
	if (!ToPlatformFormat)
	{
		ToPlatformFormat = [](uint8 InFormat) { return GPixelFormats[InFormat].PlatformFormat; };
	}

	uint32_t FormatsCount = 0;
	XR_ENSURE(xrEnumerateSwapchainFormats(InSession, 0, &FormatsCount, nullptr));

	TArray<int64_t> Formats;
	Formats.SetNum(FormatsCount);
	XR_ENSURE(xrEnumerateSwapchainFormats(InSession, (uint32_t)Formats.Num(), &FormatsCount, Formats.GetData()));
	ensure(FormatsCount == Formats.Num());

	// Return immediately if the runtime supports the exact format being requested.
	uint32 PlatformFormat = ToPlatformFormat(RequestedFormat);

	if (Formats.Contains(PlatformFormat))
	{
		return RequestedFormat;
	}

	// Search for a fallback format in order of preference (first element in the array has the highest preference).
	uint8 FallbackFormat = 0;
	uint32 FallbackPlatformFormat = 0;
	for (int64_t Format : Formats)
	{
		if (RequestedFormat == PF_DepthStencil)
		{
			if (Format == ToPlatformFormat(PF_D24))
			{
				FallbackFormat = PF_D24;
				FallbackPlatformFormat = Format;
				break;
			}
		}
		else
		{
			if (Format == ToPlatformFormat(PF_B8G8R8A8))
			{
				FallbackFormat = PF_B8G8R8A8;
				FallbackPlatformFormat = Format;
				break;
			}
			else if (Format == ToPlatformFormat(PF_R8G8B8A8))
			{
				FallbackFormat = PF_R8G8B8A8;
				FallbackPlatformFormat = Format;
				break;
			}
		}
	}

	if (!FallbackFormat)
	{
		UE_LOG(LogHMD, Warning, TEXT("No compatible swapchain format found!"));
		return PF_Unknown;
	}

	UE_LOG(LogHMD, Warning, TEXT("Swapchain format not supported (%d), falling back to runtime preferred format (%d)."), PlatformFormat, FallbackPlatformFormat);
	return FallbackFormat;
}

XrSwapchain FYvrXRSwapchain::CreateSwapchain(XrSession InSession, uint32 PlatformFormat, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, uint32 FaceCount)
{
	XrSwapchainUsageFlags Usage = XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
	if (EnumHasAnyFlags(TargetableTextureFlags, TexCreate_RenderTargetable))
	{
		Usage |= XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
	}
	if (EnumHasAnyFlags(TargetableTextureFlags, TexCreate_DepthStencilTargetable))
	{
		Usage |= XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}
	if (EnumHasAnyFlags(TargetableTextureFlags, TexCreate_UAV))
	{
		Usage |= XR_SWAPCHAIN_USAGE_UNORDERED_ACCESS_BIT;
	}

	XrSwapchain Swapchain;
	XrSwapchainCreateInfo info;
	info.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
	info.next = nullptr;
	info.createFlags = EnumHasAnyFlags(Flags, TexCreate_Dynamic) ? 0 : XR_SWAPCHAIN_CREATE_STATIC_IMAGE_BIT;
	info.usageFlags = Usage;
	info.format = PlatformFormat;
	info.sampleCount = NumSamples;
	info.width = SizeX;
	info.height = SizeY;
	info.faceCount = FaceCount == 6 ? 6 : 1;
	info.arraySize = ArraySize;
	info.mipCount = NumMips;

	//Create for foveation
	XrSwapchainCreateInfoFoveationFB SwapChainFoveationCreateInfo;
	if (EnumHasAnyFlags(TargetableTextureFlags, TexCreate_Foveation))
	{
		memset(&SwapChainFoveationCreateInfo, 0, sizeof(SwapChainFoveationCreateInfo));
		SwapChainFoveationCreateInfo.type = XR_TYPE_SWAPCHAIN_CREATE_INFO_FOVEATION_FB;
		FString RHIString = FApp::GetGraphicsRHI();
		if (RHIString == TEXT("OpenGL"))
		{
			SwapChainFoveationCreateInfo.flags = XR_SWAPCHAIN_CREATE_FOVEATION_SCALED_BIN_BIT_FB;
			info.next = &SwapChainFoveationCreateInfo;
		}
		else if (RHIString == TEXT("Vulkan"))
		{
			SwapChainFoveationCreateInfo.flags = XR_SWAPCHAIN_CREATE_FOVEATION_FRAGMENT_DENSITY_MAP_BIT_FB;
			info.next = &SwapChainFoveationCreateInfo;
		}
		else
		{
			// do nothing
		}
	}

	if (!XR_ENSURE(xrCreateSwapchain(InSession, &info, &Swapchain)))
	{
		return XR_NULL_HANDLE;
	}
	return Swapchain;
}

template<typename T>
TArray<T> EnumerateImages(XrSwapchain InSwapchain, XrStructureType InType)
{
	TArray<T> Images;
	uint32_t ChainCount;
	xrEnumerateSwapchainImages(InSwapchain, 0, &ChainCount, nullptr);
	Images.AddZeroed(ChainCount);
	for (auto& Image : Images)
	{
		Image.type = InType;
	}
	XR_ENSURE(xrEnumerateSwapchainImages(InSwapchain, ChainCount, &ChainCount, reinterpret_cast<XrSwapchainImageBaseHeader*>(Images.GetData())));
	return Images;
}

#ifdef XR_USE_GRAPHICS_API_OPENGL_ES
FTextureRHIRef CreateTexture_OpenGL(uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, FClearValueBinding InBinding, uint32 InNumMips, uint32 InNumSamples, uint32 InNumSamplesTileMem, ERHIResourceType InResourceType, uint32 InTexture, ETextureCreateFlags InTexCreateFlags)
{
	check(IsInRenderingThread());

	FOpenGLDynamicRHI* DynamicRHI = static_cast<FOpenGLDynamicRHI*>(GDynamicRHI);

	switch (InResourceType)
	{
	case RRT_Texture2D:
		return DynamicRHI->RHICreateTexture2DFromResource(InFormat, InSizeX, InSizeY, InNumMips, InNumSamples, InNumSamplesTileMem, InBinding, (GLuint)InTexture, InTexCreateFlags).GetReference();

	case RRT_Texture2DArray:
		return DynamicRHI->RHICreateTexture2DArrayFromResource(InFormat, InSizeX, InSizeY, 2, InNumMips, InNumSamples, InNumSamplesTileMem, InBinding, (GLuint)InTexture, InTexCreateFlags).GetReference();

	case RRT_TextureCube:
		return DynamicRHI->RHICreateTextureCubeFromResource(InFormat, InSizeX, false, 1, InNumMips, InNumSamples, InNumSamplesTileMem, InBinding, (GLuint)InTexture, InTexCreateFlags).GetReference();

	default:
		return nullptr;
	}
}

FXRSwapChainPtr CreateSwapchain_OpenGL(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding, uint32 FaceCount)
{
	TFunction<uint32(uint8)> ToPlatformFormat = [](uint8 InFormat)
	{
		auto PlatformFormat = GOpenGLTextureFormats[InFormat];

		UE_LOG(LogHMD, Warning, TEXT("RequestedFormat %d PlatformFormat %d %d"), InFormat, PlatformFormat.InternalFormat[0], PlatformFormat.InternalFormat[1]);

		return PlatformFormat.InternalFormat[1] != 0 ? PlatformFormat.InternalFormat[1] : PlatformFormat.InternalFormat[0];
	};

	Format = FYvrXRSwapchain::GetNearestSupportedSwapchainFormat(InSession, Format, ToPlatformFormat);
	if (!Format)
	{
		return nullptr;
	}

	XrSwapchain Swapchain = FYvrXRSwapchain::CreateSwapchain(InSession, ToPlatformFormat(Format), SizeX, SizeY, ArraySize, NumMips, NumSamples, Flags, TargetableTextureFlags | TexCreate_Foveation, FaceCount);
	if (!Swapchain)
	{
		return nullptr;
	}

	TArray<FTextureRHIRef> TextureChain;
	TArray<XrSwapchainImageOpenGLESKHR> Images = EnumerateImages<XrSwapchainImageOpenGLESKHR>(Swapchain, XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR);
	for (const auto& Image : Images)
	{
		FTextureRHIRef TempTexture = CreateTexture_OpenGL(SizeX, SizeY, GPixelFormats[Format].UnrealFormat, ClearValueBinding, NumMips, NumSamples, 1, FaceCount == 6 ? RRT_TextureCube : (ArraySize == 1 ? RRT_Texture2D : RRT_Texture2DArray), Image.image, TargetableTextureFlags);
		TextureChain.Add(TempTexture);
	}

	FTextureRHIRef ChainTarget = GDynamicRHI->RHICreateAliasedTexture(TextureChain[0]);

	return CreateXRSwapChain<FYvrXRSwapchain>(MoveTemp(TextureChain), ChainTarget, Swapchain);
}
#endif

#ifdef XR_USE_GRAPHICS_API_VULKAN
FTextureRHIRef CreateTexture_Vulkan(uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, FClearValueBinding InBinding, uint32 InNumMips, uint32 InNumSamples, uint32 InNumSamplesTileMem, ERHIResourceType InResourceType, VkImage InTexture, ETextureCreateFlags InTexCreateFlags)
{
	check(IsInRenderingThread());

	switch (InResourceType)
	{
	case RRT_Texture2D:
		return GVulkanRHI->RHICreateTexture2DFromResource(InFormat, InSizeX, InSizeY, InNumMips, InNumSamples, InTexture, InTexCreateFlags).GetReference();

	case RRT_Texture2DArray:
		return GVulkanRHI->RHICreateTexture2DArrayFromResource(InFormat, InSizeX, InSizeY, 2, InNumMips, InNumSamples, InTexture, InTexCreateFlags).GetReference();

	case RRT_TextureCube:
		return GVulkanRHI->RHICreateTextureCubeFromResource(InFormat, InSizeX, false, 1, InNumMips, InTexture, InTexCreateFlags).GetReference();

	default:
		return nullptr;
	}
}

FXRSwapChainPtr CreateSwapchain_Vulkan(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding, uint32 FaceCount)
{
	TFunction<uint32(uint8)> ToPlatformFormat = [](uint8 InFormat)
	{
		return UEToVkTextureFormat(GPixelFormats[InFormat].UnrealFormat, true);
	};
	Format = FYvrXRSwapchain::GetNearestSupportedSwapchainFormat(InSession, Format, ToPlatformFormat);
	if (!Format)
	{
		return nullptr;
	}

	XrSwapchain Swapchain = FYvrXRSwapchain::CreateSwapchain(InSession, ToPlatformFormat(Format), SizeX, SizeY, ArraySize, NumMips, NumSamples, Flags, TargetableTextureFlags, FaceCount);
	if (!Swapchain)
	{
		return nullptr;
	}

	TArray<FTextureRHIRef> TextureChain;
	TArray<XrSwapchainImageVulkanKHR> Images = EnumerateImages<XrSwapchainImageVulkanKHR>(Swapchain, XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR);
	for (const auto& Image : Images)
	{
		FTextureRHIRef TempTexture = CreateTexture_Vulkan(SizeX, SizeY, GPixelFormats[Format].UnrealFormat, ClearValueBinding, NumMips, NumSamples, 1, FaceCount == 6 ? RRT_TextureCube : (ArraySize == 1 ? RRT_Texture2D : RRT_Texture2DArray), Image.image, TargetableTextureFlags);
		TextureChain.Add(TempTexture);
	}
	FTextureRHIRef ChainTarget = GDynamicRHI->RHICreateAliasedTexture((FTextureRHIRef&)TextureChain[0]);

	return CreateXRSwapChain<FYvrXRSwapchain>(MoveTemp(TextureChain), ChainTarget, Swapchain);
}

void EnumerateVulkanImagesWithFoveation(XrSwapchain InSwapchain, TArray<XrSwapchainImageVulkanKHR>& OutImages, TArray<XrSwapchainImageFoveationVulkanFB>& OutFoveationImages)
{
	TArray<XrSwapchainImageVulkanKHR> Images;
	TArray<XrSwapchainImageFoveationVulkanFB> FoveationImages;
	uint32_t ChainCount;
	xrEnumerateSwapchainImages(InSwapchain, 0, &ChainCount, nullptr);
	Images.AddZeroed(ChainCount);
	FoveationImages.AddZeroed(ChainCount);
	for (int Index = 0; Index < ChainCount; ++Index)
	{
		Images[Index].type = XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR;
		FoveationImages[Index] = { XR_TYPE_SWAPCHAIN_IMAGE_FOVEATION_VULKAN_FB };
		Images[Index].next = &FoveationImages[Index];
	}
	XR_ENSURE(xrEnumerateSwapchainImages(InSwapchain, ChainCount, &ChainCount, reinterpret_cast<XrSwapchainImageBaseHeader*>(Images.GetData())));
	OutImages = Images;
	OutFoveationImages = FoveationImages;
}

TArray<FXRSwapChainPtr> CreateSwapchainWithFoveation_Vulkan(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding)
{
	TArray<FXRSwapChainPtr> SwapchainArray;

	TFunction<uint32(uint8)> ToPlatformFormat = [](uint8 InFormat)
	{
		return UEToVkTextureFormat(GPixelFormats[InFormat].UnrealFormat, true);
	};

	Format = FYvrXRSwapchain::GetNearestSupportedSwapchainFormat(InSession, Format, ToPlatformFormat);

	if (!Format)
	{
		return SwapchainArray;
	}

	XrSwapchain Swapchain = FYvrXRSwapchain::CreateSwapchain(InSession, ToPlatformFormat(Format), SizeX, SizeY, ArraySize, NumMips, NumSamples, Flags, TargetableTextureFlags | TexCreate_Foveation);
	if (!Swapchain)
	{
		return SwapchainArray;
	}

	TArray<FTextureRHIRef> TextureChain;
	TArray<FTextureRHIRef> FoveationTextureChain;
	TArray<XrSwapchainImageVulkanKHR> Images;
	TArray<XrSwapchainImageFoveationVulkanFB> FoveationImages;
	EnumerateVulkanImagesWithFoveation(Swapchain, Images, FoveationImages);
	for (int Index = 0; Index < Images.Num(); ++Index)
	{
		FTextureRHIRef TempTexture = CreateTexture_Vulkan(SizeX, SizeY, GPixelFormats[Format].UnrealFormat, ClearValueBinding, NumMips, NumSamples, 1, ArraySize == 1 ? RRT_Texture2D : RRT_Texture2DArray, Images[Index].image, TargetableTextureFlags);
		TextureChain.Add(TempTexture);

		FTextureRHIRef TempFoveationTexture = CreateTexture_Vulkan(FoveationImages[Index].width, FoveationImages[Index].height, PF_R8G8, ClearValueBinding, 1, 1, 1, ArraySize == 1 ? RRT_Texture2D : RRT_Texture2DArray, FoveationImages[Index].image, TexCreate_Foveation);
		FoveationTextureChain.Add(TempFoveationTexture);
	}
	FTextureRHIRef ChainTarget = GDynamicRHI->RHICreateAliasedTexture((FTextureRHIRef&)TextureChain[0]);
	FTextureRHIRef FoveationChainTarget = GDynamicRHI->RHICreateAliasedTexture((FTextureRHIRef&)FoveationTextureChain[0]);

	SwapchainArray.Add(CreateXRSwapChain<FYvrXRSwapchain>(MoveTemp(TextureChain), ChainTarget, Swapchain));
	SwapchainArray.Add(CreateXRSwapChain<FYvrXRSwapchain>(MoveTemp(FoveationTextureChain), FoveationChainTarget));
	return SwapchainArray;
}


#endif