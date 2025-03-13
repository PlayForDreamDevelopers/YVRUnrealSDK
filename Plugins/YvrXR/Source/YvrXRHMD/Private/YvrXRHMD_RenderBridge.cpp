// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRHMD_RenderBridge.h"
#include "YvrXRHMD.h"
#include "YvrXRHMD_Swapchain.h"
#include "YvrXRCore.h"

bool FYvrXRRenderBridge::Present(int32& InOutSyncInterval)
{
	bool bNeedsNativePresent = true;

	if (OpenXRHMD)
	{
		OpenXRHMD->OnFinishRendering_RHIThread();
		bNeedsNativePresent = !OpenXRHMD->IsStandaloneStereoOnlyDevice();
	}

	InOutSyncInterval = 0; // VSync off

	return bNeedsNativePresent;
}
#ifdef XR_USE_GRAPHICS_API_OPENGL_ES
class FOpenGLRenderBridge : public FYvrXRRenderBridge
{
public:
	FOpenGLRenderBridge(XrInstance InInstance, XrSystemId InSystem)
		:Binding()
	{
		PFN_xrGetOpenGLESGraphicsRequirementsKHR GetOpenGLESGraphicsRequirementsKHR;
		XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetOpenGLESGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&GetOpenGLESGraphicsRequirementsKHR));

		XrGraphicsRequirementsOpenGLESKHR Requirements;
		Requirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR;
		Requirements.next = nullptr;
		Requirements.minApiVersionSupported = 0;
		Requirements.maxApiVersionSupported = 0;
		XR_ENSURE(GetOpenGLESGraphicsRequirementsKHR(InInstance, InSystem, &Requirements));

		XrVersion RHIVersion = XR_MAKE_VERSION(FOpenGL::GetMajorVersion(), FOpenGL::GetMinorVersion(), 0);
		if (RHIVersion < Requirements.minApiVersionSupported) //-V547
		{
			UE_LOG(LogHMD, Fatal, TEXT("The OpenGL API version does not meet the minimum version required by the OpenXR runtime"));
		}

		if (RHIVersion > Requirements.maxApiVersionSupported) //-V547
		{
			UE_LOG(LogHMD, Warning, TEXT("The OpenGL API version has not been tested with the OpenXR runtime"));
		}
	}

	virtual void* GetGraphicsBinding() override
	{
		Binding.type = XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR;
		Binding.next = nullptr;
		Binding.display = AndroidEGL::GetInstance()->GetDisplay();
		Binding.config = (EGLConfig)0;
		Binding.context = AndroidEGL::GetInstance()->GetRenderingContext()->eglContext;
		return &Binding;
	}


	virtual FXRSwapChainPtr CreateSwapchain(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding, uint32 FaceCount = 1) override final
	{
		return CreateSwapchain_OpenGL(InSession, Format, SizeX, SizeY, ArraySize, NumMips, NumSamples, Flags, TargetableTextureFlags, ClearValueBinding, FaceCount);
	}

	virtual TArray<FXRSwapChainPtr> CreateSwapchainWithFoveation(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding) override final
	{
		TArray<FXRSwapChainPtr> SwapchainArray;
		FXRSwapChainPtr ColorSwapchain = CreateSwapchain_OpenGL(InSession, Format, SizeX, SizeY, ArraySize, NumMips, NumSamples, Flags, TargetableTextureFlags, ClearValueBinding);
		SwapchainArray.Add(ColorSwapchain);
		return SwapchainArray;
	}

private:
	XrGraphicsBindingOpenGLESAndroidKHR Binding;
};

FYvrXRRenderBridge* CreateRenderBridge_OpenGL(XrInstance InInstance, XrSystemId InSystem) { return new FOpenGLRenderBridge(InInstance, InSystem); }
#endif

#ifdef XR_USE_GRAPHICS_API_VULKAN
class FVulkanRenderBridge : public FYvrXRRenderBridge
{
public:
	FVulkanRenderBridge(XrInstance InInstance, XrSystemId InSystem)
		: Binding()
		, Instance(InInstance)
		, System(InSystem)
	{
		PFN_xrGetVulkanGraphicsRequirementsKHR GetVulkanGraphicsRequirementsKHR;
		XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&GetVulkanGraphicsRequirementsKHR));

		XrGraphicsRequirementsVulkanKHR Requirements;
		Requirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR;
		Requirements.next = nullptr;
		Requirements.minApiVersionSupported = 0;
		Requirements.maxApiVersionSupported = 0;
		XR_ENSURE(GetVulkanGraphicsRequirementsKHR(InInstance, InSystem, &Requirements));

		// The extension uses the OpenXR version format instead of the Vulkan one
		XrVersion RHIVersion = XR_MAKE_VERSION(
			VK_VERSION_MAJOR(UE_VK_API_VERSION),
			VK_VERSION_MINOR(UE_VK_API_VERSION),
			VK_VERSION_PATCH(UE_VK_API_VERSION)
		);
		if (RHIVersion < Requirements.minApiVersionSupported) //-V547
		{
			UE_LOG(LogHMD, Fatal, TEXT("The Vulkan API version does not meet the minimum version required by the OpenXR runtime"));
		}

		if (RHIVersion > Requirements.maxApiVersionSupported) //-V547
		{
			UE_LOG(LogHMD, Warning, TEXT("The Vulkan API version has not been tested with the OpenXR runtime"));
		}

		PFN_xrGetVulkanGraphicsDeviceKHR GetVulkanGraphicsDeviceKHR;
		XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction*)&GetVulkanGraphicsDeviceKHR));
		XR_ENSURE(GetVulkanGraphicsDeviceKHR(Instance, System, GVulkanRHI->GetInstance(), &Gpu));
	}

	virtual void* GetGraphicsBinding() override
	{
		FVulkanDevice* Device = GVulkanRHI->GetDevice();
		FVulkanQueue* Queue = Device->GetGraphicsQueue();

		Binding.type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR;
		Binding.next = nullptr;
		Binding.instance = GVulkanRHI->GetInstance();
		Binding.physicalDevice = Device->GetPhysicalHandle();
		Binding.device = Device->GetInstanceHandle();
		Binding.queueFamilyIndex = Queue->GetFamilyIndex();
		Binding.queueIndex = 0;
		return &Binding;
	}

	virtual uint64 GetGraphicsAdapterLuid() override
	{
#if VULKAN_SUPPORTS_DRIVER_PROPERTIES
		if (!AdapterLuid && GVulkanRHI->GetOptionalExtensions().HasKHRGetPhysicalDeviceProperties2)
		{
			VkPhysicalDeviceIDPropertiesKHR GpuIdProps;
			VkPhysicalDeviceProperties2KHR GpuProps2;
			ZeroVulkanStruct(GpuProps2, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR);
			GpuProps2.pNext = &GpuIdProps;
			ZeroVulkanStruct(GpuIdProps, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR);

			VulkanRHI::vkGetPhysicalDeviceProperties2KHR(Gpu, &GpuProps2);
			check(GpuIdProps.deviceLUIDValid);
			AdapterLuid = reinterpret_cast<const uint64&>(GpuIdProps.deviceLUID);
		}
#endif
		return AdapterLuid;
	}

	virtual FXRSwapChainPtr CreateSwapchain(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding, uint32 FaceCount = 1) override final
	{
		return CreateSwapchain_Vulkan(InSession, Format, SizeX, SizeY, ArraySize, NumMips, NumSamples, Flags, TargetableTextureFlags, ClearValueBinding, FaceCount);
	}

	virtual TArray<FXRSwapChainPtr> CreateSwapchainWithFoveation(XrSession InSession, uint8 Format, uint32 SizeX, uint32 SizeY, uint32 ArraySize, uint32 NumMips, uint32 NumSamples, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, const FClearValueBinding& ClearValueBinding) override final
	{
		return CreateSwapchainWithFoveation_Vulkan(InSession, Format, SizeX, SizeY, ArraySize, NumMips, NumSamples, Flags, TargetableTextureFlags, ClearValueBinding);
	}

private:
	XrGraphicsBindingVulkanKHR Binding;
	XrInstance Instance;
	XrSystemId System;
	VkPhysicalDevice Gpu;
};

FYvrXRRenderBridge* CreateRenderBridge_Vulkan(XrInstance InInstance, XrSystemId InSystem) { return new FVulkanRenderBridge(InInstance, InSystem); }
#endif