// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRHMD.h"
#include "YvrXRHMD_Layer.h"
#include "YvrXRHMD_RenderBridge.h"
#include "YvrXRHMD_Swapchain.h"
#include "YvrXRCore.h"
#include "YvrXRHMDSettings.h"
#include "YvrXRShaders.h"
#include "YvrXRHMD_DynamicResolutionState.h"

#include "Misc/App.h"
#include "Misc/Parse.h"
#include "Modules/ModuleManager.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "IYvrXRHMDPlugin.h"
#include "SceneRendering.h"
#include "PostProcess/PostProcessHMD.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/CString.h"
#include "ClearQuad.h"
#include "XRThreadUtils.h"
#include "RenderUtils.h"
#include "PipelineStateCache.h"
#include "Slate/SceneViewport.h"
#include "Engine/GameEngine.h"
#include "BuildSettings.h"
#include "IHandTracker.h"
#include "PixelShaderUtils.h"
#include "ScreenRendering.h"
#include "Interfaces/IPluginManager.h"
#include "DataDrivenShaderPlatformInfo.h"

#if PLATFORM_ANDROID
#include <android_native_app_glue.h>
#include "Android/AndroidPlatformMisc.h"
#include <dlfcn.h>
extern struct android_app* GNativeAndroidApp;
#endif

#if WITH_EDITOR
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#endif

#define LOCTEXT_NAMESPACE "FYvrXRHMDPlugin"

#define OPENXR_PAUSED_IDLE_FPS 10
static const int64 OPENXR_SWAPCHAIN_WAIT_TIMEOUT = 100000000ll;		// 100ms in nanoseconds.

static TAutoConsoleVariable<int32> CVarEnableOpenXRValidationLayer(
	TEXT("xr.EnableOpenXRValidationLayer"),
	0,
	TEXT("If true, enables the OpenXR validation layer, which will provide extended validation of\nOpenXR API calls. This should only be used for debugging purposes.\n")
	TEXT("Changes will only take effect in new game/editor instances - can't be changed at runtime.\n"),
	ECVF_Default);		// @todo: Should we specify ECVF_Cheat here so this doesn't show up in release builds?

static TAutoConsoleVariable<int32> CVarOculusEnableSpaceWarpInternal(
	TEXT("r.Mobile.Oculus.SpaceWarp.EnableInternal"),
	0,
	TEXT("0 Disable spacewarp, for internal engine checking, don't modify.\n")
	TEXT("1 Enable spacewarp, for internal enegine checking, don't modify.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarYvrEnableSpaceWarpInternal(
	TEXT("r.Mobile.Yvr.SpaceWarp.EnableInternal"),
	0,
	TEXT("0 Disable spacewarp, for internal engine checking, don't modify.\n")
	TEXT("1 Enable spacewarp, for internal enegine checking, don't modify.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);

uint32 FYvrXRHMD::EyebufferLayerId = 0;

namespace {
	static TSet<XrEnvironmentBlendMode> SupportedBlendModes{ XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE, XR_ENVIRONMENT_BLEND_MODE_OPAQUE };
	static TSet<XrViewConfigurationType> SupportedViewConfigurations{ XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_QUAD_VARJO };

	/** Helper function for acquiring the appropriate FSceneViewport */
	FSceneViewport* FindSceneViewport()
	{
		if (!GIsEditor)
		{
			UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
			return GameEngine->SceneViewport.Get();
		}
	#if WITH_EDITOR
		else
		{
			UEditorEngine* EditorEngine = CastChecked<UEditorEngine>(GEngine);
			FSceneViewport* PIEViewport = (FSceneViewport*)EditorEngine->GetPIEViewport();
			if (PIEViewport != nullptr && PIEViewport->IsStereoRenderingAllowed())
			{
				// PIE is setup for stereo rendering
				return PIEViewport;
			}
			else
			{
				// Check to see if the active editor viewport is drawing in stereo mode
				// @todo vreditor: Should work with even non-active viewport!
				FSceneViewport* EditorViewport = (FSceneViewport*)EditorEngine->GetActiveViewport();
				if (EditorViewport != nullptr && EditorViewport->IsStereoRenderingAllowed())
				{
					return EditorViewport;
				}
			}
		}
	#endif
		return nullptr;
	}
}

//---------------------------------------------------
// OpenXRHMD Plugin Implementation
//---------------------------------------------------

class FYvrXRHMDPlugin : public IYvrXRHMDPlugin
{
public:
	FYvrXRHMDPlugin()
		: LoaderHandle(nullptr)
		, Instance(XR_NULL_HANDLE)
		, System(XR_NULL_SYSTEM_ID)
		, RenderBridge(nullptr)
	{ }

	~FYvrXRHMDPlugin()
	{
	}

	/** IHeadMountedDisplayModule implementation */
	virtual TSharedPtr< class IXRTrackingSystem, ESPMode::ThreadSafe > CreateTrackingSystem() override;
	virtual TSharedPtr< IHeadMountedDisplayVulkanExtensions, ESPMode::ThreadSafe > GetVulkanExtensions() override;
	virtual uint64 GetGraphicsAdapterLuid() override;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FString GetModuleKeyName() const override
	{
		return FString(TEXT("YvrXRHMD"));
	}

	void GetModuleAliases(TArray<FString>& AliasesOut) const override
	{
		AliasesOut.Add(TEXT("YvrXR"));
	}

	virtual bool IsHMDConnected() override { return true; }
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
	virtual bool IsStandaloneStereoOnlyDevice() override;
#endif
	virtual bool IsExtensionAvailable(const FString& Name) const override { return AvailableExtensions.Contains(Name); }
	virtual bool IsExtensionEnabled(const FString& Name) const override { return EnabledExtensions.Contains(Name); }
	virtual bool IsLayerAvailable(const FString& Name) const override { return EnabledLayers.Contains(Name); }
	virtual bool IsLayerEnabled(const FString& Name) const override { return EnabledLayers.Contains(Name); }

private:
	void* LoaderHandle;
	XrInstance Instance;
	XrSystemId System;
	TSet<FString> AvailableExtensions;
	TSet<FString> AvailableLayers;
	TArray<const char*> EnabledExtensions;
	TArray<const char*> EnabledLayers;
	TRefCountPtr<FYvrXRRenderBridge> RenderBridge;
	TSharedPtr< IHeadMountedDisplayVulkanExtensions, ESPMode::ThreadSafe > VulkanExtensions;

	bool EnumerateExtensions();
	bool EnumerateLayers();
	bool InitRenderBridge();
	bool InitInstanceAndSystem();
	bool InitInstance();
	bool InitSystem();
	PFN_xrGetInstanceProcAddr GetDefaultLoader();
	bool EnableExtensions(const TArray<const ANSICHAR*>& RequiredExtensions, const TArray<const ANSICHAR*>& OptionalExtensions, TArray<const ANSICHAR*>& OutExtensions);
	bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions);
	bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions);
};

IMPLEMENT_MODULE(FYvrXRHMDPlugin, YvrXRHMD)

TSharedPtr< class IXRTrackingSystem, ESPMode::ThreadSafe > FYvrXRHMDPlugin::CreateTrackingSystem()
{
	if (!RenderBridge)
	{
		if (!InitRenderBridge())
		{
			return nullptr;
		}
	}

	auto OpenXRHMD = FSceneViewExtensions::NewExtension<FYvrXRHMD>(Instance, System, RenderBridge, EnabledExtensions);
	if (OpenXRHMD->IsInitialized())
	{
		return OpenXRHMD;
	}

	return nullptr;
}

void FYvrXRHMDPlugin::StartupModule()
{
	IHeadMountedDisplayModule::StartupModule();

	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("YvrXR"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/YvrXR"), PluginShaderDir);
}

void FYvrXRHMDPlugin::ShutdownModule()
{
	IHeadMountedDisplayModule::ShutdownModule();
	if (Instance)
	{
		XR_ENSURE(xrDestroyInstance(Instance));
	}

	if (LoaderHandle)
	{
		FPlatformProcess::FreeDllHandle(LoaderHandle);
		LoaderHandle = nullptr;
	}
}

uint64 FYvrXRHMDPlugin::GetGraphicsAdapterLuid()
{
	if (!RenderBridge)
	{
		if (!InitRenderBridge())
		{
			return 0;
		}
	}
	return RenderBridge->GetGraphicsAdapterLuid();
}

TSharedPtr< IHeadMountedDisplayVulkanExtensions, ESPMode::ThreadSafe > FYvrXRHMDPlugin::GetVulkanExtensions()
{
#ifdef XR_USE_GRAPHICS_API_VULKAN
	if (InitInstanceAndSystem() && IsExtensionEnabled(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME))
	{
		if (!VulkanExtensions.IsValid())
		{
			VulkanExtensions = MakeShareable(new FYvrXRHMD::FVulkanExtensions(Instance, System));
		}
		return VulkanExtensions;
	}
#endif//XR_USE_GRAPHICS_API_VULKAN
	return nullptr;
}

#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
bool FYvrXRHMDPlugin::IsStandaloneStereoOnlyDevice()
{
	if (InitInstanceAndSystem())
	{
		return true;
	}
	return false;
}
#endif

bool FYvrXRHMDPlugin::EnumerateExtensions()
{
	uint32_t ExtensionsCount = 0;
	if (XR_FAILED(xrEnumerateInstanceExtensionProperties(nullptr, 0, &ExtensionsCount, nullptr)))
	{
		// If it fails this early that means there's no runtime installed
		return false;
	}

	TArray<XrExtensionProperties> Properties;
	Properties.SetNum(ExtensionsCount);
	for (auto& Prop : Properties)
	{
		Prop = XrExtensionProperties{ XR_TYPE_EXTENSION_PROPERTIES };
	}

	if (XR_ENSURE(xrEnumerateInstanceExtensionProperties(nullptr, ExtensionsCount, &ExtensionsCount, Properties.GetData())))
	{
		for (const XrExtensionProperties& Prop : Properties)
		{
			AvailableExtensions.Add(Prop.extensionName);
		}
		return true;
	}
	return false;
}

bool FYvrXRHMDPlugin::EnumerateLayers()
{
	uint32 LayerPropertyCount = 0;
	if (XR_FAILED(xrEnumerateApiLayerProperties(0, &LayerPropertyCount, nullptr)))
	{
		// As per EnumerateExtensions - a failure here means no runtime installed.
		return false;
	}

	if (!LayerPropertyCount)
	{
		// It's still legit if we have no layers, so early out here (and return success) if so.
		return true;
	}

	TArray<XrApiLayerProperties> LayerProperties;
	LayerProperties.SetNum(LayerPropertyCount);
	for (auto& Prop : LayerProperties)
	{
		Prop = XrApiLayerProperties{ XR_TYPE_API_LAYER_PROPERTIES };
	}

	if (XR_ENSURE(xrEnumerateApiLayerProperties(LayerPropertyCount, &LayerPropertyCount, LayerProperties.GetData())))
	{
		for (const auto& Prop : LayerProperties)
		{
			AvailableLayers.Add(Prop.layerName);
		}
		return true;
	}

	return false;
}

struct AnsiKeyFunc : BaseKeyFuncs<const ANSICHAR*, const ANSICHAR*, false>
{
	typedef typename TTypeTraits<const ANSICHAR*>::ConstPointerType KeyInitType;
	typedef typename TCallTraits<const ANSICHAR*>::ParamType ElementInitType;

	/**
	 * @return The key used to index the given element.
	 */
	static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
	{
		return Element;
	}

	/**
	 * @return True if the keys match.
	 */
	static FORCEINLINE bool Matches(KeyInitType A, KeyInitType B)
	{
		return FCStringAnsi::Strcmp(A, B) == 0;
	}

	/** Calculates a hash index for a key. */
	static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
	{
		return GetTypeHash(Key);
	}
};

bool FYvrXRHMDPlugin::InitRenderBridge()
{
	FString RHIString = FApp::GetGraphicsRHI();
	if (RHIString.IsEmpty())
	{
		return false;
	}

	if (!InitInstanceAndSystem())
	{
		return false;
	}

	if (RHIString == TEXT("OpenGL"))
	{
#ifdef XR_USE_GRAPHICS_API_OPENGL_ES
		RenderBridge = CreateRenderBridge_OpenGL(Instance, System);
#endif
	}
	else
	if (RHIString == TEXT("Vulkan"))
	{
#ifdef XR_USE_GRAPHICS_API_VULKAN
		RenderBridge = CreateRenderBridge_Vulkan(Instance, System);
#endif
	}
	else
	{
		UE_LOG(LogHMD, Warning, TEXT("%s is not currently supported by the OpenXR runtime"), *RHIString);
		return false;
	}
	return true;
}

PFN_xrGetInstanceProcAddr FYvrXRHMDPlugin::GetDefaultLoader()
{
#if PLATFORM_ANDROID
	dlerror();

	LoaderHandle = FPlatformProcess::GetDllHandle(TEXT("libopenxr_loader.so"));
	if (LoaderHandle == nullptr)
	{
		UE_LOG(LogHMD, Error, TEXT("Unable to load libopenxr_loader.so, error %s"), ANSI_TO_TCHAR(dlerror()));
		return nullptr;
	}

	// clear errors
	dlerror();

	PFN_xrGetInstanceProcAddr xrGetInstanceProcAddrPtr = (PFN_xrGetInstanceProcAddr)FPlatformProcess::GetDllExport(LoaderHandle, TEXT("xrGetInstanceProcAddr"));
	if (xrGetInstanceProcAddrPtr == nullptr)
	{
		UE_LOG(LogHMD, Error, TEXT("Unable to load OpenXR xrGetInstanceProcAddr, error %s"), ANSI_TO_TCHAR(dlerror()));
		return nullptr;
	}
	//*OutGetProcAddr = xrGetInstanceProcAddrPtr;

	extern struct android_app* GNativeAndroidApp;
	PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR;
	xrGetInstanceProcAddrPtr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)&xrInitializeLoaderKHR);
	if (xrInitializeLoaderKHR == nullptr)
	{
		UE_LOG(LogHMD, Error, TEXT("Unable to load OpenXR xrInitializeLoaderKHR"));
		return nullptr;
	}
	XrLoaderInitInfoAndroidKHR LoaderInitializeInfoAndroid;
	LoaderInitializeInfoAndroid.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;
	LoaderInitializeInfoAndroid.next = NULL;
	LoaderInitializeInfoAndroid.applicationVM = GNativeAndroidApp->activity->vm;
	LoaderInitializeInfoAndroid.applicationContext = GNativeAndroidApp->activity->clazz;
	XR_ENSURE(xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR*)&LoaderInitializeInfoAndroid));

	return xrGetInstanceProcAddrPtr;
#endif
	return nullptr;
}

bool FYvrXRHMDPlugin::EnableExtensions(const TArray<const ANSICHAR*>& RequiredExtensions, const TArray<const ANSICHAR*>& OptionalExtensions, TArray<const ANSICHAR*>& OutExtensions)
{
	// Query required extensions and check if they're all available
	bool ExtensionMissing = false;
	for (const ANSICHAR* Ext : RequiredExtensions)
	{
		if (AvailableExtensions.Contains(Ext))
		{
			UE_LOG(LogHMD, Verbose, TEXT("Required extension %S enabled"), Ext);
		}
		else
		{
			UE_LOG(LogHMD, Warning, TEXT("Required extension %S is not available"), Ext);
			ExtensionMissing = true;
		}
	}

	// If any required extensions are missing then we ignore the plugin
	if (ExtensionMissing)
	{
		return false;
	}

	// All required extensions are supported we can safely add them to our set and give the plugin callbacks
	OutExtensions.Append(RequiredExtensions);

	// Add all supported optional extensions to the set
	for (const ANSICHAR* Ext : OptionalExtensions)
	{
		if (AvailableExtensions.Contains(Ext))
		{
			UE_LOG(LogHMD, Verbose, TEXT("Optional extension %S enabled"), Ext);
			OutExtensions.Add(Ext);
		}
		else
		{
			UE_LOG(LogHMD, Log, TEXT("Optional extension %S is not available"), Ext);
		}
	}

	return true;
}

bool FYvrXRHMDPlugin::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
{
#if PLATFORM_ANDROID
	OutExtensions.Add(XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME);
#endif
	return true;
}

bool FYvrXRHMDPlugin::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
{
#if PLATFORM_ANDROID
	OutExtensions.Add(XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME);
	OutExtensions.Add(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_FOVEATION_VULKAN_EXTENSION_NAME);

#endif
	OutExtensions.Add(XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME);
	OutExtensions.Add(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME);
	OutExtensions.Add(XR_KHR_COMPOSITION_LAYER_EQUIRECT_EXTENSION_NAME);
	OutExtensions.Add(XR_KHR_COMPOSITION_LAYER_CUBE_EXTENSION_NAME);

	OutExtensions.Add(XR_EXT_LOCAL_FLOOR_EXTENSION_NAME);
	OutExtensions.Add(XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME);
	OutExtensions.Add(XR_EXT_HAND_TRACKING_EXTENSION_NAME);

	OutExtensions.Add(XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_FOVEATION_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_SWAPCHAIN_UPDATE_STATE_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_SPACE_WARP_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_HAND_TRACKING_MESH_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_COMPOSITION_LAYER_SETTINGS_EXTENSION_NAME);

	OutExtensions.Add(XR_FB_SPATIAL_ENTITY_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_SPATIAL_ENTITY_QUERY_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_SPATIAL_ENTITY_STORAGE_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_SCENE_CAPTURE_EXTENSION_NAME);
	OutExtensions.Add(XR_FB_SCENE_EXTENSION_NAME);

	OutExtensions.Add(XR_YVR_PASSTHROUGH_EXTENSION_NAME);
	OutExtensions.Add(XR_YVR_COLOR_SPACE_EXTENSION_NAME);
	OutExtensions.Add(XR_YVR_ADAPTIVE_RESOLUTION_EXTENSION_NAME);

	OutExtensions.Add(XR_YVR_SPATIAL_ENTITY_MESH_EXTENSION_NAME);
	OutExtensions.Add(XR_META_SPATIAL_ENTITY_MESH_EXTENSION_NAME);

	return true;
}

bool FYvrXRHMDPlugin::InitInstanceAndSystem()
{
	if (!Instance && !InitInstance())
	{
		return false;
	}

	if (!System && !InitSystem())
	{
		return false;
	}

	return true;
}

bool FYvrXRHMDPlugin::InitInstance()
{
	// This should only ever be called if we don't already have an instance.
	check(!Instance);

	// Get all extension plugins
	TSet<const ANSICHAR*, AnsiKeyFunc> ExtensionSet;

	// Query all extension plugins to see if we need to use a custom loader
	PFN_xrGetInstanceProcAddr GetProcAddr = nullptr;

	if (!GetProcAddr)
	{
		GetProcAddr = GetDefaultLoader();
	}

	if (!PreInitOpenXRCore(GetProcAddr))
	{
		UE_LOG(LogHMD, Log, TEXT("Failed to initialize core functions. Please check that you have a valid OpenXR runtime installed."));
		return false;
	}

	if (!EnumerateExtensions())
	{
		UE_LOG(LogHMD, Log, TEXT("Failed to enumerate extensions. Please check that you have a valid OpenXR runtime installed."));
		return false;
	}

	if (!EnumerateLayers())
	{
		UE_LOG(LogHMD, Log, TEXT("Failed to enumerate API layers. Please check that you have a valid OpenXR runtime installed."));
		return false;
	}

	// Enable any required and optional extensions that are not plugin specific (usually platform support extensions)
	{
		TArray<const ANSICHAR*> RequiredExtensions, OptionalExtensions, Extensions;
		// Query required extensions
		RequiredExtensions.Empty();
		if (!GetRequiredExtensions(RequiredExtensions))
		{
			UE_LOG(LogHMD, Error, TEXT("Could not get required OpenXR extensions."));
			return false;
		}

		// Query optional extensions
		OptionalExtensions.Empty();
		if (!GetOptionalExtensions(OptionalExtensions))
		{
			UE_LOG(LogHMD, Error, TEXT("Could not get optional OpenXR extensions."));
			return false;
		}

		if (!EnableExtensions(RequiredExtensions, OptionalExtensions, Extensions))
		{
			UE_LOG(LogHMD, Error, TEXT("Could not enable all required OpenXR extensions."));
			return false;
		}
		ExtensionSet.Append(Extensions);
	}

	if (AvailableExtensions.Contains(XR_EPIC_VIEW_CONFIGURATION_FOV_EXTENSION_NAME))
	{
		ExtensionSet.Add(XR_EPIC_VIEW_CONFIGURATION_FOV_EXTENSION_NAME);
	}

	EnabledExtensions.Reset();
	for (const ANSICHAR* Ext : ExtensionSet)
	{
		EnabledExtensions.Add(Ext);
	}

	// Enable layers, if specified by CVar.
	// Note: For the validation layer to work on Windows (as of latest OpenXR runtime, August 2019), the following are required:
	//   1. Download and build the OpenXR SDK from https://github.com/KhronosGroup/OpenXR-SDK-Source (follow instructions at https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/master/BUILDING.md)
	//	 2. Add a registry key under HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\OpenXR\1\ApiLayers\Explicit, containing the path to the manifest file
	//      (e.g. C:\OpenXR-SDK-Source-master\build\win64\src\api_layers\XrApiLayer_core_validation.json) <-- this file is downloaded as part of the SDK source, above
	//   3. Copy the DLL from the build target at, for example, C:\OpenXR-SDK-Source-master\build\win64\src\api_layers\XrApiLayer_core_validation.dll to
	//      somewhere in your system path (e.g. c:\windows\system32); the OpenXR loader currently doesn't use the path the json file is in (this is a bug)

	const bool bEnableOpenXRValidationLayer = CVarEnableOpenXRValidationLayer.GetValueOnAnyThread() != 0;
	TArray<const char*> Layers;
	if (bEnableOpenXRValidationLayer && AvailableLayers.Contains("XR_APILAYER_LUNARG_core_validation"))
	{
		Layers.Add("XR_APILAYER_LUNARG_core_validation");
	}

	// Engine registration can be disabled via console var.
	auto* CVarDisableEngineAndAppRegistration = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.DisableEngineAndAppRegistration"));
	bool bDisableEngineRegistration = (CVarDisableEngineAndAppRegistration && CVarDisableEngineAndAppRegistration->GetValueOnAnyThread() != 0);

	FText ProjectName = FText();
	GConfig->GetText(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectName"), ProjectName, GGameIni);

	FText ProjectVersion = FText();
	GConfig->GetText(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), ProjectVersion, GGameIni);

	// EngineName will be of the form "UnrealEngine4.21", with the minor version ("21" in this example)
	// updated with every quarterly release
	FString EngineName = bDisableEngineRegistration ? FString("") : FApp::GetEpicProductIdentifier() + FEngineVersion::Current().ToString(EVersionComponent::Minor);
	FString AppName = bDisableEngineRegistration ? TEXT("") : ProjectName.ToString() + ProjectVersion.ToString();

	XrInstanceCreateInfo Info;
	Info.type = XR_TYPE_INSTANCE_CREATE_INFO;
	Info.next = nullptr;
	Info.createFlags = 0;
	FTCHARToUTF8_Convert::Convert(Info.applicationInfo.applicationName, XR_MAX_APPLICATION_NAME_SIZE, *AppName, AppName.Len() + 1);
	Info.applicationInfo.applicationVersion = static_cast<uint32>(BuildSettings::GetCurrentChangelist()) | (BuildSettings::IsLicenseeVersion() ? 0x80000000 : 0);
	FTCHARToUTF8_Convert::Convert(Info.applicationInfo.engineName, XR_MAX_ENGINE_NAME_SIZE, *EngineName, EngineName.Len() + 1);
	Info.applicationInfo.engineVersion = (uint32)(FEngineVersion::Current().GetMinor() << 16 | FEngineVersion::Current().GetPatch());
	Info.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

	Info.enabledApiLayerCount = Layers.Num();
	Info.enabledApiLayerNames = Layers.GetData();

	Info.enabledExtensionCount = EnabledExtensions.Num();
	Info.enabledExtensionNames = EnabledExtensions.GetData();

#if PLATFORM_ANDROID
	XrInstanceCreateInfoAndroidKHR InstanceCreateInfoAndroid;
	InstanceCreateInfoAndroid.type = XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR;
	InstanceCreateInfoAndroid.next = nullptr;
	InstanceCreateInfoAndroid.applicationVM = GNativeAndroidApp->activity->vm;
	InstanceCreateInfoAndroid.applicationActivity = GNativeAndroidApp->activity->clazz;
	Info.next = &InstanceCreateInfoAndroid;
#endif // PLATFORM_ANDROID

	XrResult Result = xrCreateInstance(&Info, &Instance);
	if (XR_FAILED(Result))
	{
		UE_LOG(LogHMD, Log, TEXT("Failed to create an OpenXR instance, result is %s. Please check if you have an OpenXR runtime installed. The following extensions were enabled:"), OpenXRResultToString(Result));
		for (const char* Extension : EnabledExtensions)
		{
			UE_LOG(LogHMD, Log, TEXT("- %S"), Extension);
		}
		return false;
	}

	if (!InitOpenXRCore(Instance))
	{
		UE_LOG(LogHMD, Log, TEXT("Failed to initialize core functions. Please check that you have a valid OpenXR runtime installed."));
		return false;
	}

	return true;
}

bool FYvrXRHMDPlugin::InitSystem()
{
	XrSystemGetInfo SystemInfo;
	SystemInfo.type = XR_TYPE_SYSTEM_GET_INFO;
	SystemInfo.next = nullptr;
	SystemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

	XrResult Result = xrGetSystem(Instance, &SystemInfo, &System);
	if (XR_FAILED(Result))
	{
		UE_LOG(LogHMD, Log, TEXT("Failed to get an OpenXR system, result is %s. Please check that your runtime supports VR headsets."), OpenXRResultToString(Result));
		return false;
	}

	return true;
}

//---------------------------------------------------
// OpenXRHMD IHeadMountedDisplay Implementation
//---------------------------------------------------

bool FYvrXRHMD::FVulkanExtensions::GetVulkanInstanceExtensionsRequired(TArray<const ANSICHAR*>& Out)
{
#ifdef XR_USE_GRAPHICS_API_VULKAN
	TArray<VkExtensionProperties> Properties;
	{
		uint32_t PropertyCount;
		VulkanRHI::vkEnumerateInstanceExtensionProperties(nullptr, &PropertyCount, nullptr);
		Properties.SetNum(PropertyCount);
		VulkanRHI::vkEnumerateInstanceExtensionProperties(nullptr, &PropertyCount, Properties.GetData());
	}

	{
		PFN_xrGetVulkanInstanceExtensionsKHR GetVulkanInstanceExtensionsKHR;
		XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction*)&GetVulkanInstanceExtensionsKHR));

		uint32 ExtensionCount = 0;
		XR_ENSURE(GetVulkanInstanceExtensionsKHR(Instance, System, 0, &ExtensionCount, nullptr));
		Extensions.SetNum(ExtensionCount);
		XR_ENSURE(GetVulkanInstanceExtensionsKHR(Instance, System, ExtensionCount, &ExtensionCount, Extensions.GetData()));
	}

	ANSICHAR* Context = nullptr;
	for (ANSICHAR* Tok = FCStringAnsi::Strtok(Extensions.GetData(), " ", &Context); Tok != nullptr; Tok = FCStringAnsi::Strtok(nullptr, " ", &Context))
	{
		bool ExtensionFound = false;
		for (int32 PropertyIndex = 0; PropertyIndex < Properties.Num(); PropertyIndex++)
		{
			const char* PropertyExtensionName = Properties[PropertyIndex].extensionName;

			if (!FCStringAnsi::Strcmp(PropertyExtensionName, Tok))
			{
				Out.Add(Tok);
				ExtensionFound = true;
				break;
			}
		}

		if (!ExtensionFound)
		{
			UE_LOG(LogHMD, Log, TEXT("Missing required Vulkan instance extension %S."), Tok);
			return false;
		}
	}
#endif
	return true;
}

bool FYvrXRHMD::FVulkanExtensions::GetVulkanDeviceExtensionsRequired(VkPhysicalDevice_T* pPhysicalDevice, TArray<const ANSICHAR*>& Out)
{
#ifdef XR_USE_GRAPHICS_API_VULKAN
	TArray<VkExtensionProperties> Properties;
	{
		uint32_t PropertyCount;
		VulkanRHI::vkEnumerateDeviceExtensionProperties((VkPhysicalDevice)pPhysicalDevice, nullptr, &PropertyCount, nullptr);
		Properties.SetNum(PropertyCount);
		VulkanRHI::vkEnumerateDeviceExtensionProperties((VkPhysicalDevice)pPhysicalDevice, nullptr, &PropertyCount, Properties.GetData());
	}

	{
		PFN_xrGetVulkanDeviceExtensionsKHR GetVulkanDeviceExtensionsKHR;
		XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction*)&GetVulkanDeviceExtensionsKHR));

		uint32 ExtensionCount = 0;
		XR_ENSURE(GetVulkanDeviceExtensionsKHR(Instance, System, 0, &ExtensionCount, nullptr));
		DeviceExtensions.SetNum(ExtensionCount);
		XR_ENSURE(GetVulkanDeviceExtensionsKHR(Instance, System, ExtensionCount, &ExtensionCount, DeviceExtensions.GetData()));
	}

	ANSICHAR* Context = nullptr;
	for (ANSICHAR* Tok = FCStringAnsi::Strtok(DeviceExtensions.GetData(), " ", &Context); Tok != nullptr; Tok = FCStringAnsi::Strtok(nullptr, " ", &Context))
	{
		bool ExtensionFound = false;
		for (int32 PropertyIndex = 0; PropertyIndex < Properties.Num(); PropertyIndex++)
		{
			const char* PropertyExtensionName = Properties[PropertyIndex].extensionName;

			if (!FCStringAnsi::Strcmp(PropertyExtensionName, Tok))
			{
				Out.Add(Tok);
				ExtensionFound = true;
				break;
			}
		}

		if (!ExtensionFound)
		{
			UE_LOG(LogHMD, Log, TEXT("Missing required Vulkan device extension %S."), Tok);
			return false;
		}
	}
#endif
	return true;
}

void FYvrXRHMD::GetMotionControllerData(UObject* WorldContext, const EControllerHand Hand, FXRMotionControllerData& MotionControllerData)
{
	MotionControllerData.DeviceName = NAME_None;
	MotionControllerData.ApplicationInstanceID = FApp::GetInstanceId();
	MotionControllerData.DeviceVisualType = EXRVisualType::Controller;
	MotionControllerData.TrackingStatus = ETrackingStatus::NotTracked;
	MotionControllerData.HandIndex = Hand;

	TArray<int32> Devices;
	if (EnumerateTrackedDevices(Devices, EXRTrackedDeviceType::Controller) && Devices.IsValidIndex((int32)Hand))
	{
		FReadScopeLock SessionLock(SessionHandleMutex);
		if (Session)
		{
			XrInteractionProfileState Profile;
			Profile.type = XR_TYPE_INTERACTION_PROFILE_STATE;
			Profile.next = nullptr;
			if (XR_SUCCEEDED(xrGetCurrentInteractionProfile(Session, GetTrackedDevicePath(Devices[(int32)Hand]), &Profile)) &&
				Profile.interactionProfile != XR_NULL_PATH)
			{
				TArray<char> Path;
				uint32 PathCount = 0;
				XR_ENSURE(xrPathToString(Instance, Profile.interactionProfile, 0, &PathCount, nullptr));
				Path.SetNum(PathCount);
				XR_ENSURE(xrPathToString(Instance, Profile.interactionProfile, PathCount, &PathCount, Path.GetData()));
				MotionControllerData.DeviceName = Path.GetData();
			}
		}
	}

	FName HandTrackerName("OpenXRHandTracking");
	TArray<IHandTracker*> HandTrackers = IModularFeatures::Get().GetModularFeatureImplementations<IHandTracker>(IHandTracker::GetModularFeatureName());
	IHandTracker* HandTracker = nullptr;
	for (auto Itr : HandTrackers)
	{
		if (Itr->GetHandTrackerDeviceTypeName() == HandTrackerName)
		{
			HandTracker = Itr;
			break;
		}
	}

	FName MotionControllerName("OpenXR");
	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	IMotionController* MotionController = nullptr;
	for (auto Itr : MotionControllers)
	{
		if (Itr->GetMotionControllerDeviceTypeName() == MotionControllerName)
		{
			MotionController = Itr;
			break;
		}
	}

	if (MotionController)
	{
		const float WorldToMeters = GetWorldToMetersScale();

		bool bSuccess = false;
		FVector Position = FVector::ZeroVector;
		FRotator Rotation = FRotator::ZeroRotator;
		FTransform trackingToWorld = GetTrackingToWorldTransform();
		FName AimSource = Hand == EControllerHand::Left ? FName("LeftAim") : FName("RightAim");
		bSuccess = MotionController->GetControllerOrientationAndPosition(0, AimSource, Rotation, Position, WorldToMeters);
		if (bSuccess)
		{
			MotionControllerData.AimPosition = trackingToWorld.TransformPosition(Position);
			MotionControllerData.AimRotation = trackingToWorld.TransformRotation(FQuat(Rotation));
		}
		MotionControllerData.bValid |= bSuccess;

		FName GripSource = Hand == EControllerHand::Left ? FName("LeftGrip") : FName("RightGrip");
		bSuccess = MotionController->GetControllerOrientationAndPosition(0, GripSource, Rotation, Position, WorldToMeters);
		if (bSuccess)
		{
			MotionControllerData.GripPosition = trackingToWorld.TransformPosition(Position);
			MotionControllerData.GripRotation = trackingToWorld.TransformRotation(FQuat(Rotation));
		}
		MotionControllerData.bValid |= bSuccess;

		MotionControllerData.TrackingStatus = MotionController->GetControllerTrackingStatus(0, GripSource);
	}

	if (HandTracker && HandTracker->IsHandTrackingStateValid())
	{
		MotionControllerData.DeviceVisualType = EXRVisualType::Hand;

		MotionControllerData.bValid = HandTracker->GetAllKeypointStates(Hand, MotionControllerData.HandKeyPositions, MotionControllerData.HandKeyRotations, MotionControllerData.HandKeyRadii);
		check(!MotionControllerData.bValid || (MotionControllerData.HandKeyPositions.Num() == EHandKeypointCount && MotionControllerData.HandKeyRotations.Num() == EHandKeypointCount && MotionControllerData.HandKeyRadii.Num() == EHandKeypointCount));
	}

	//TODO: this is reportedly a wmr specific convenience function for rapid prototyping.  Not sure it is useful for openxr.
	MotionControllerData.bIsGrasped = false;
}

float FYvrXRHMD::GetWorldToMetersScale() const
{
	return IsInRenderingThread() ? PipelinedFrameStateRendering.WorldToMetersScale : PipelinedFrameStateGame.WorldToMetersScale;
}

#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
FName FYvrXRHMD::GetHMDName() const
{
	return SystemProperties.systemName;
}
#endif

FString FYvrXRHMD::GetVersionString() const
{
	return FString::Printf(TEXT("%s: %d.%d.%d"),
		UTF8_TO_TCHAR(InstanceProperties.runtimeName),
		XR_VERSION_MAJOR(InstanceProperties.runtimeVersion),
		XR_VERSION_MINOR(InstanceProperties.runtimeVersion),
		XR_VERSION_PATCH(InstanceProperties.runtimeVersion));
}

bool FYvrXRHMD::IsTracking(int32 DeviceId)
{
	return GetIsTracked(DeviceId);
}

bool FYvrXRHMD::IsHMDEnabled() const
{
	return true;
}

void FYvrXRHMD::EnableHMD(bool enable)
{
}

bool FYvrXRHMD::GetHMDMonitorInfo(MonitorInfo& MonitorDesc)
{
	MonitorDesc.MonitorName = UTF8_TO_TCHAR(SystemProperties.systemName);
	MonitorDesc.MonitorId = 0;

	FIntPoint RTSize = Settings->RenderTargetSize;
	MonitorDesc.DesktopX = MonitorDesc.DesktopY = 0;
	MonitorDesc.ResolutionX = MonitorDesc.WindowSizeX = RTSize.X;
	MonitorDesc.ResolutionY = MonitorDesc.WindowSizeY = RTSize.Y;
	return true;
}

void FYvrXRHMD::GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const
{
	const FPipelinedFrameState& FrameState = GetPipelinedFrameStateForThread();

	XrFovf UnifiedFov = { 0.0f };
	for (const XrView& View : FrameState.Views)
	{
		UnifiedFov.angleLeft = FMath::Min(UnifiedFov.angleLeft, View.fov.angleLeft);
		UnifiedFov.angleRight = FMath::Max(UnifiedFov.angleRight, View.fov.angleRight);
		UnifiedFov.angleUp = FMath::Max(UnifiedFov.angleUp, View.fov.angleUp);
		UnifiedFov.angleDown = FMath::Min(UnifiedFov.angleDown, View.fov.angleDown);
	}
	OutHFOVInDegrees = FMath::RadiansToDegrees(UnifiedFov.angleRight - UnifiedFov.angleLeft);
	OutVFOVInDegrees = FMath::RadiansToDegrees(UnifiedFov.angleUp - UnifiedFov.angleDown);
}

bool FYvrXRHMD::EnumerateTrackedDevices(TArray<int32>& OutDevices, EXRTrackedDeviceType Type)
{
	if (Type == EXRTrackedDeviceType::Any || Type == EXRTrackedDeviceType::HeadMountedDisplay)
	{
		OutDevices.Add(IXRTrackingSystem::HMDDeviceId);
	}
	if (Type == EXRTrackedDeviceType::Any || Type == EXRTrackedDeviceType::Controller)
	{
		FReadScopeLock DeviceLock(DeviceMutex);

		// Skip the HMD, we already added it to the list
		for (int32 i = 1; i < DeviceSpaces.Num(); i++)
		{
			OutDevices.Add(i);
		}
	}
	return OutDevices.Num() > 0;
}

void FYvrXRHMD::SetInterpupillaryDistance(float NewInterpupillaryDistance)
{
}

float FYvrXRHMD::GetInterpupillaryDistance() const
{
	const FPipelinedFrameState& FrameState = GetPipelinedFrameStateForThread();
	if (FrameState.Views.Num() < 2)
	{
		return 0.064f;
	}

	FVector leftPos = ToFVector(FrameState.Views[0].pose.position);
	FVector rightPos = ToFVector(FrameState.Views[1].pose.position);
	return FVector::Dist(leftPos, rightPos);
}

bool FYvrXRHMD::GetIsTracked(int32 DeviceId)
{
	// This function is called from both the game and rendering thread and each thread maintains separate pose
	// snapshots to prevent inconsistent poses (tearing) on the same frame.
	const FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();

	if (!PipelineState.DeviceLocations.IsValidIndex(DeviceId))
	{
		return false;
	}

	const XrSpaceLocation& Location = PipelineState.DeviceLocations[DeviceId];
	return Location.locationFlags & XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT &&
		Location.locationFlags & XR_SPACE_LOCATION_POSITION_TRACKED_BIT;
}

bool FYvrXRHMD::GetCurrentPose(int32 DeviceId, FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	CurrentOrientation = FQuat::Identity;
	CurrentPosition = FVector::ZeroVector;

	// This function is called from both the game and rendering thread and each thread maintains separate pose
	// snapshots to prevent inconsistent poses (tearing) on the same frame.
	const FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();

	if (!PipelineState.DeviceLocations.IsValidIndex(DeviceId))
	{
		return false;
	}

	const XrSpaceLocation& Location = PipelineState.DeviceLocations[DeviceId];
	if (Location.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT)
	{
		CurrentOrientation = ToFQuat(Location.pose.orientation);
	}
	if (Location.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT)
	{
		CurrentPosition = ToFVector(Location.pose.position, GetWorldToMetersScale());
	}

	return true;
}


bool FYvrXRHMD::IsChromaAbCorrectionEnabled() const
{
	return false;
}

void FYvrXRHMD::ResetOrientationAndPosition(float Yaw)
{
	Recenter(EOrientPositionSelector::OrientationAndPosition, Yaw);
}

void FYvrXRHMD::ResetOrientation(float Yaw)
{
	Recenter(EOrientPositionSelector::Orientation, Yaw);
}

void FYvrXRHMD::ResetPosition()
{
	Recenter(EOrientPositionSelector::Position);
}

void FYvrXRHMD::Recenter(EOrientPositionSelector::Type Selector, float Yaw)
{
	const FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();
	const XrTime TargetTime = PipelineState.FrameState.predictedDisplayTime;

	XrSpace DeviceSpace = XR_NULL_HANDLE;
	{
		FReadScopeLock DeviceLock(DeviceMutex);
		const FDeviceSpace& DeviceSpaceStruct = DeviceSpaces[HMDDeviceId];
		DeviceSpace = DeviceSpaceStruct.Space;
	}
	XrSpaceLocation DeviceLocation = { XR_TYPE_SPACE_LOCATION, nullptr };

	XrSpace BaseSpace = TrackingSpaceType == XR_REFERENCE_SPACE_TYPE_STAGE ? StageSpace : LocalSpace;

	XR_ENSURE(xrLocateSpace(DeviceSpace, BaseSpace, TargetTime, &DeviceLocation));

	const FQuat CurrentOrientation = ToFQuat(DeviceLocation.pose.orientation);
	const FVector CurrentPosition = ToFVector(DeviceLocation.pose.position, GetWorldToMetersScale());

	if (Selector == EOrientPositionSelector::Position ||
		Selector == EOrientPositionSelector::OrientationAndPosition)
	{
		FVector NewPosition;
		NewPosition.X = CurrentPosition.X;
		NewPosition.Y = CurrentPosition.Y;
		if (TrackingSpaceType == XR_REFERENCE_SPACE_TYPE_LOCAL)
		{
			NewPosition.Z = CurrentPosition.Z;
		}
		else
		{
			NewPosition.Z = 0.0f;
		}
		SetBasePosition(NewPosition);
	}

	if (Selector == EOrientPositionSelector::Orientation ||
		Selector == EOrientPositionSelector::OrientationAndPosition)
	{
		FRotator NewOrientation(0.0f, CurrentOrientation.Rotator().Yaw - Yaw, 0.0f);
		SetBaseOrientation(NewOrientation.Quaternion());
	}
}

void FYvrXRHMD::SetBaseRotation(const FRotator& InBaseRotation)
{
	SetBaseOrientation(InBaseRotation.Quaternion());
}

FRotator FYvrXRHMD::GetBaseRotation() const
{
	return BaseOrientation.Rotator();
}

void FYvrXRHMD::SetBaseOrientation(const FQuat& InBaseOrientation)
{
	BaseOrientation = InBaseOrientation;
	UpdateBasePose();
	UpdateDeviceLocations(false);
}

FQuat FYvrXRHMD::GetBaseOrientation() const
{
	return BaseOrientation;
}

void FYvrXRHMD::SetBasePosition(const FVector& InBasePosition)
{
	BasePosition = InBasePosition;
	UpdateBasePose();
	UpdateDeviceLocations(false);
}

FVector FYvrXRHMD::GetBasePosition() const
{
	return BasePosition;
}

void FYvrXRHMD::SetTrackingOrigin(EHMDTrackingOrigin::Type NewOrigin)
{
	switch (NewOrigin)
	{
	case EHMDTrackingOrigin::Local:
		TrackingSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
		break;
	case EHMDTrackingOrigin::LocalFloor:
		TrackingSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR_EXT;
		break;
	case EHMDTrackingOrigin::Stage:
		TrackingSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
		break;
	default:
		UE_LOG(LogHMD, Error, TEXT("Unsupported yvr tracking origin type %d"), int(NewOrigin));
		break;
	}
}

EHMDTrackingOrigin::Type FYvrXRHMD::GetTrackingOrigin() const
{
	EHMDTrackingOrigin::Type TrackingOrigin = EHMDTrackingOrigin::Local;
	switch (TrackingSpaceType)
	{
	case XR_REFERENCE_SPACE_TYPE_LOCAL:
		TrackingOrigin = EHMDTrackingOrigin::Local;
		break;
	case XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR_EXT:
		TrackingOrigin = EHMDTrackingOrigin::LocalFloor;
		break;
	case XR_REFERENCE_SPACE_TYPE_STAGE:
		TrackingOrigin = EHMDTrackingOrigin::Stage;
		break;
	default:
		break;
	}
	return TrackingOrigin;
}

bool FYvrXRHMD::IsStereoEnabled() const
{
	return bStereoEnabled;
}

bool FYvrXRHMD::EnableStereo(bool stereo)
{
	UE_LOG(LogHMD, Log, TEXT("EnableStereo"));

	if (stereo == bStereoEnabled)
	{
		return true;
	}

	bStereoEnabled = stereo;
	if (stereo)
	{
		GEngine->bForceDisableFrameRateSmoothing = true;
		if (OnStereoStartup())
		{
			StartSession();
			UpdateStereoRenderingParams();

			FApp::SetUseVRFocus(true);
			FApp::SetHasVRFocus(true);

			return true;
		}
		return false;
	}
	else
	{
		GEngine->bForceDisableFrameRateSmoothing = false;

		FApp::SetUseVRFocus(false);
		FApp::SetHasVRFocus(false);

#if WITH_EDITOR
		if (GIsEditor)
		{
			if (FSceneViewport* SceneVP = FindSceneViewport())
			{
				TSharedPtr<SWindow> Window = SceneVP->FindWindow();
				if (Window.IsValid())
				{
					Window->SetViewportSizeDrivenByWindow(true);
				}
			}
		}
#endif // WITH_EDITOR

		return OnStereoTeardown();
	}
}


int32 FYvrXRHMD::GetDesiredNumberOfViews(bool bStereoRequested) const
{
	const FPipelinedFrameState& FrameState = GetPipelinedFrameStateForThread();

	// FIXME: Monoscopic actually needs 2 views for quad vr
	return bStereoRequested ? FrameState.ViewConfigs.Num() : 1;
}

#if ENGINE_MAJOR_VERSION > 4
#else
uint32 FYvrXRHMD::GetViewIndexForPass(EStereoscopicPass StereoPassType) const
{
	switch (StereoPassType)
	{
	case eSSP_LEFT_EYE:
	case eSSP_FULL:
		return 0;

	case eSSP_RIGHT_EYE:
		return 1;

	default:
		return StereoPassType - eSSP_LEFT_EYE;
	}
}
#endif

void FYvrXRHMD::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	InViewFamily.EngineShowFlags.MotionBlur = 0;
	InViewFamily.EngineShowFlags.HMDDistortion = false;
	InViewFamily.EngineShowFlags.StereoRendering = IsStereoEnabled();
	InViewFamily.EngineShowFlags.ScreenPercentage = Settings->bPixelDensityAdaptive;


	const FPipelinedFrameState& FrameState = GetPipelinedFrameStateForThread();
	if (FrameState.Views.Num() > 2)
	{
		InViewFamily.EngineShowFlags.Vignette = 0;
		InViewFamily.EngineShowFlags.Bloom = 0;
	}
}

void FYvrXRHMD::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
}

void FYvrXRHMD::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	uint32 ViewConfigCount = 0;
	XR_ENSURE(xrEnumerateViewConfigurationViews(Instance, System, SelectedViewConfigurationType, 0, &ViewConfigCount, nullptr));

	PipelinedLayerStateRendering.ProjectionLayers.SetNum(ViewConfigCount);
	PipelinedLayerStateRendering.DepthLayers.SetNum(ViewConfigCount);
	PipelinedLayerStateRendering.MotionVectorLayers.SetNum(ViewConfigCount);

	PipelinedLayerStateRendering.ColorImages.SetNum(PipelinedFrameStateRendering.ViewConfigs.Num());
	PipelinedLayerStateRendering.DepthImages.SetNum(PipelinedFrameStateRendering.ViewConfigs.Num());
	PipelinedLayerStateRendering.MotionVectorImages.SetNum(PipelinedFrameStateRendering.ViewConfigs.Num());
	PipelinedLayerStateRendering.MotionVectorDepthImages.SetNum(PipelinedFrameStateRendering.ViewConfigs.Num());

	PipelinedLayerStateRendering.LayerSettings.SetNum(ViewConfigCount);
}

void FYvrXRHMD::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
	check(IsInRenderingThread());
}

void FYvrXRHMD::PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	check(IsInRenderingThread());
}

bool FYvrXRHMD::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	// Don't activate the SVE if xr is being used for tracking only purposes
	static const bool bXrTrackingOnly = FParse::Param(FCommandLine::Get(), TEXT("xrtrackingonly"));

	return GEngine && GEngine->IsStereoscopic3D(Context.Viewport) && !bXrTrackingOnly;
}

void FYvrXRHMD::CalculateRenderTargetSize(const FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY)
{
	if (!bStereoEnabled)
	{
		return;
	}

	InOutSizeX = Settings->RenderTargetSize.X;
	InOutSizeY = Settings->RenderTargetSize.Y;

	check(InOutSizeX != 0 && InOutSizeY != 0);
}

FYvrXRHMD::FYvrXRHMD(const FAutoRegister& AutoRegister, XrInstance InInstance, XrSystemId InSystem, TRefCountPtr<FYvrXRRenderBridge>& InRenderBridge, TArray<const char*> InEnabledExtensions)
	: FHeadMountedDisplayBase(nullptr)
	, FSceneViewExtensionBase(AutoRegister)
	, bStereoEnabled(false)
	, bIsRunning(false)
	, bIsReady(false)
	, bIsRendering(false)
	, bIsSynchronized(false)
	, bNeedReAllocatedDepth(false)
	, bIsMobileMultiViewEnabled(false)
	, bIsMobileHDREnabled(false)
	, bSupportsHandTracking(false)
	, bIsStandaloneStereoOnlyDevice(false)
	, bEnableSpaceWarp(false)
	, CurrentSessionState(XR_SESSION_STATE_UNKNOWN)
	, EnabledExtensions(std::move(InEnabledExtensions))
	, Instance(InInstance)
	, System(InSystem)
	, Session(XR_NULL_HANDLE)
	, LocalSpace(XR_NULL_HANDLE)
	, FloorSpace(XR_NULL_HANDLE)
	, StageSpace(XR_NULL_HANDLE)
	, TrackingSpaceType(XR_REFERENCE_SPACE_TYPE_STAGE)
	, SelectedViewConfigurationType(XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM)
	, SelectedEnvironmentBlendMode(XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM)
	, RenderBridge(InRenderBridge)
	, RendererModule(nullptr)
	, LastRequestedSwapchainFormat(0)
	, LastRequestedDepthSwapchainFormat(0)
	, bNeedReAllocatedMotionVector(false)
	, RecommendedMotionVectorImageRectWidth(0)
	, RecommendedMotionVectorImageRectHeight(0)
	, BaseOrientation(FQuat::Identity)
	, BasePosition(FVector::ZeroVector)
	, bNeedReAllocatedFoveation(false)
{
	InstanceProperties = { XR_TYPE_INSTANCE_PROPERTIES, nullptr };
	XR_ENSURE(xrGetInstanceProperties(Instance, &InstanceProperties));

	bDepthExtensionSupported = false;
	bViewConfigurationFovSupported = IsExtensionEnabled(XR_EPIC_VIEW_CONFIGURATION_FOV_EXTENSION_NAME);

	// Retrieve system properties and check for hand tracking support
	XrSystemHandTrackingPropertiesEXT HandTrackingSystemProperties = { XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT };
	SystemProperties = XrSystemProperties{ XR_TYPE_SYSTEM_PROPERTIES, &HandTrackingSystemProperties };
	XR_ENSURE(xrGetSystemProperties(Instance, System, &SystemProperties));
	bSupportsHandTracking = HandTrackingSystemProperties.supportsHandTracking == XR_TRUE;
	SystemProperties.next = nullptr;

	XrSystemSpaceWarpPropertiesFB SpaceWarpProperties = { XR_TYPE_SYSTEM_SPACE_WARP_PROPERTIES_FB };
	SystemProperties.next = &SpaceWarpProperties;
	XR_ENSURE(xrGetSystemProperties(Instance, System, &SystemProperties));

	RecommendedMotionVectorImageRectWidth = SpaceWarpProperties.recommendedMotionVectorImageRectWidth;
	RecommendedMotionVectorImageRectHeight = SpaceWarpProperties.recommendedMotionVectorImageRectHeight;

	static const auto CVarMobileMultiView = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("vr.MobileMultiView"));
	static const auto CVarMobileHDR = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.MobileHDR"));
	bIsMobileHDREnabled = (CVarMobileHDR && CVarMobileHDR->GetValueOnAnyThread() != 0);
	const bool bMobileMultiView = !bIsMobileHDREnabled && (CVarMobileMultiView && CVarMobileMultiView->GetValueOnAnyThread() != 0);
	bIsMobileMultiViewEnabled = bMobileMultiView && RHISupportsMobileMultiView(GMaxRHIShaderPlatform);

	static const auto CVarSupportMobileSpaceWarp = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("vr.SupportMobileSpaceWarp"));
	bool bIsVulkan = FApp::GetGraphicsRHI() == TEXT("Vulkan");
	bool bIsMobileSpaceWarpEnabled = CVarSupportMobileSpaceWarp && (CVarSupportMobileSpaceWarp->GetValueOnAnyThread() != 0);
	bEnableSpaceWarp = bIsVulkan && bIsMobileMultiViewEnabled && bIsMobileSpaceWarpEnabled;

	// Enumerate the viewport configurations
	uint32 ConfigurationCount;
	TArray<XrViewConfigurationType> ViewConfigTypes;
	XR_ENSURE(xrEnumerateViewConfigurations(Instance, System, 0, &ConfigurationCount, nullptr));
	ViewConfigTypes.SetNum(ConfigurationCount);
	// Fill the initial array with valid enum types (this will fail in the validation layer otherwise).
	for (auto& TypeIter : ViewConfigTypes)
		TypeIter = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO;
	XR_ENSURE(xrEnumerateViewConfigurations(Instance, System, ConfigurationCount, &ConfigurationCount, ViewConfigTypes.GetData()));

	// Select the first view configuration returned by the runtime that is supported.
	// This is the view configuration preferred by the runtime.
	for (XrViewConfigurationType ViewConfigType : ViewConfigTypes)
	{
		if (SupportedViewConfigurations.Contains(ViewConfigType))
		{
			SelectedViewConfigurationType = ViewConfigType;
			break;
		}
	}

	// If there is no supported view configuration type, use the first option as a last resort.
	if (!ensure(SelectedViewConfigurationType != XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM))
	{
		UE_LOG(LogHMD, Error, TEXT("No compatible view configuration type found, falling back to runtime preferred type."));
		SelectedViewConfigurationType = ViewConfigTypes[0];
	}

	// Enumerate the views we will be simulating with.
	EnumerateViews(PipelinedFrameStateGame);

	// Enumerate environment blend modes and select the best one.
	{
		uint32 BlendModeCount;
		TArray<XrEnvironmentBlendMode> BlendModes;
		XR_ENSURE(xrEnumerateEnvironmentBlendModes(Instance, System, SelectedViewConfigurationType, 0, &BlendModeCount, nullptr));
		// Fill the initial array with valid enum types (this will fail in the validation layer otherwise).
		for (auto& TypeIter : BlendModes)
			TypeIter = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
		BlendModes.SetNum(BlendModeCount);
		XR_ENSURE(xrEnumerateEnvironmentBlendModes(Instance, System, SelectedViewConfigurationType, BlendModeCount, &BlendModeCount, BlendModes.GetData()));

		// Select the first blend mode returned by the runtime that is supported.
		// This is the environment blend mode preferred by the runtime.
		for (XrEnvironmentBlendMode BlendMode : BlendModes)
		{
			if (SupportedBlendModes.Contains(BlendMode) &&
				// On mobile platforms the alpha channel can contain depth information, so we can't use alpha blend.
				(BlendMode != XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND || !IsMobilePlatform(GMaxRHIShaderPlatform)))
			{
				SelectedEnvironmentBlendMode = BlendMode;
				break;
			}
		}

		// If there is no supported environment blend mode, use the first option as a last resort.
		if (!ensure(SelectedEnvironmentBlendMode != XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM))
		{
			SelectedEnvironmentBlendMode = BlendModes[0];
		}
	}

	bIsStandaloneStereoOnlyDevice = true;

	// Add a device space for the HMD without an action handle and ensure it has the correct index
	XrPath UserHead = XR_NULL_PATH;
	XR_ENSURE(xrStringToPath(Instance, "/user/head", &UserHead));
	ensure(DeviceSpaces.Emplace(XR_NULL_HANDLE, UserHead) == HMDDeviceId);

	// Give the all frame states the same initial values.
	PipelinedFrameStateRHI = PipelinedFrameStateRendering = PipelinedFrameStateGame;

	Settings = GetMutableDefault<UYvrXRHMDSettings>();

	LastTrackingToWorld = FTransform::Identity;

	IStereoLayers::FLayerDesc EyeLayerDesc;
	EyeLayerDesc.Priority = INT_MIN;
	EyeLayerDesc.Flags = LAYER_FLAG_TEX_CONTINUOUS_UPDATE;
	EyebufferLayerId = CreateLayer(EyeLayerDesc);
}

FYvrXRHMD::~FYvrXRHMD()
{
	DestroySession();
}

const FYvrXRHMD::FPipelinedFrameState& FYvrXRHMD::GetPipelinedFrameStateForThread() const
{
	// Relying on implicit selection of the RHI struct is hazardous since the RHI thread isn't always present
	check(!IsInRHIThread());

	if (IsInRenderingThread())
	{
		return PipelinedFrameStateRendering;
	}
	else
	{
		check(IsInGameThread());
		return PipelinedFrameStateGame;
	}
}

FYvrXRHMD::FPipelinedFrameState& FYvrXRHMD::GetPipelinedFrameStateForThread()
{
	// Relying on implicit selection of the RHI struct is hazardous since the RHI thread isn't always present
	check(!IsInRHIThread());

	if (IsInRenderingThread())
	{
		return PipelinedFrameStateRendering;
	}
	else
	{
		check(IsInGameThread());
		return PipelinedFrameStateGame;
	}
}

void FYvrXRHMD::UpdateDeviceLocations(bool bUpdateOpenXRExtensionPlugins)
{
	SCOPED_NAMED_EVENT(UpdateDeviceLocations, FColor::Red);

	FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();

	// Only update the device locations if the frame state has been predicted
	if (PipelineState.FrameState.predictedDisplayTime > 0)
	{
		FReadScopeLock Lock(DeviceMutex);
		PipelineState.DeviceLocations.SetNum(DeviceSpaces.Num());
		for (int32 DeviceIndex = 0; DeviceIndex < PipelineState.DeviceLocations.Num(); DeviceIndex++)
		{
			const FDeviceSpace& DeviceSpace = DeviceSpaces[DeviceIndex];
			if (DeviceSpace.Space != XR_NULL_HANDLE)
			{
				XrSpaceLocation& DeviceLocation = PipelineState.DeviceLocations[DeviceIndex];
				DeviceLocation.type = XR_TYPE_SPACE_LOCATION;
				DeviceLocation.next = nullptr;
				XrResult Result = xrLocateSpace(DeviceSpace.Space, PipelineState.TrackingSpace->Handle, PipelineState.FrameState.predictedDisplayTime, &DeviceLocation);
				if (Result == XR_ERROR_TIME_INVALID)
				{
					// The display time is no longer valid so set the location as invalid as well
					PipelineState.DeviceLocations[DeviceIndex].locationFlags = 0;
				}
				else
				{
					XR_ENSURE(Result);
				}
			}
			else
			{
				// Ensure the location flags are zeroed out so the pose is detected as invalid
				PipelineState.DeviceLocations[DeviceIndex].locationFlags = 0;
			}
		}
	}
}

void FYvrXRHMD::UpdateBasePose()
{
	FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();

	//Tracking Space
	FTransform BaseTransform(BaseOrientation, BasePosition);
	PipelineState.TrackingSpace = MakeShared<FTrackingSpace>(TrackingSpaceType, ToXrPose(BaseTransform, WorldToMetersScale));
	PipelineState.TrackingSpace->CreateSpace(Session);
}

void FYvrXRHMD::EnumerateViews(FPipelinedFrameState& PipelineState)
{
	SCOPED_NAMED_EVENT(EnumerateViews, FColor::Red);

	// Enumerate the viewport configuration views
	uint32 ViewConfigCount = 0;
	TArray<XrViewConfigurationViewFovEPIC> ViewFov;
	XR_ENSURE(xrEnumerateViewConfigurationViews(Instance, System, SelectedViewConfigurationType, 0, &ViewConfigCount, nullptr));

	ViewFov.SetNum(ViewConfigCount);
	PipelineState.ViewConfigs.Empty(ViewConfigCount);
	PipelineState.PluginViews.Empty(ViewConfigCount);
	for (uint32 ViewIndex = 0; ViewIndex < ViewConfigCount; ViewIndex++)
	{
		XrViewConfigurationView View;
		View.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;

		ViewFov[ViewIndex].type = XR_TYPE_VIEW_CONFIGURATION_VIEW_FOV_EPIC;
		ViewFov[ViewIndex].next = nullptr;
		View.next = bViewConfigurationFovSupported ? &ViewFov[ViewIndex] : nullptr;

		// These are core views that don't have an associated plugin
		PipelineState.PluginViews.Add(nullptr);
		PipelineState.ViewConfigs.Add(View);
	}
	XR_ENSURE(xrEnumerateViewConfigurationViews(Instance, System, SelectedViewConfigurationType, ViewConfigCount, &ViewConfigCount, PipelinedFrameStateGame.ViewConfigs.GetData()));

	if (Session)
	{
		LocateViews(PipelineState, true);
	}
	else if (bViewConfigurationFovSupported)
	{
		// We can't locate the views yet, but we can already retrieve their field-of-views
		PipelineState.Views.SetNum(PipelineState.ViewConfigs.Num());
		for (int ViewIndex = 0; ViewIndex < PipelineState.Views.Num(); ViewIndex++)
		{
			XrView& View = PipelineState.Views[ViewIndex];
			View.type = XR_TYPE_VIEW;
			View.next = nullptr;
			View.fov = ViewFov[ViewIndex].recommendedFov;
			View.pose = ToXrPose(FTransform::Identity);
		}
	}
	else
	{
		// Ensure the views have sane values before we locate them
		PipelineState.Views.SetNum(PipelineState.ViewConfigs.Num());
		for (XrView& View : PipelineState.Views)
		{
			View.type = XR_TYPE_VIEW;
			View.next = nullptr;
			View.fov = XrFovf{ -PI / 4.0f, PI / 4.0f, PI / 4.0f, -PI / 4.0f };
			View.pose = ToXrPose(FTransform::Identity);
		}
	}
}

bool FYvrXRHMD::OnStereoStartup()
{
	FWriteScopeLock Lock(SessionHandleMutex);
	FWriteScopeLock DeviceLock(DeviceMutex);

	XrSessionCreateInfo SessionInfo;
	SessionInfo.type = XR_TYPE_SESSION_CREATE_INFO;
	SessionInfo.next = RenderBridge->GetGraphicsBinding();
	SessionInfo.createFlags = 0;
	SessionInfo.systemId = System;

	if (!XR_ENSURE(xrCreateSession(Instance, &SessionInfo, &Session)))
	{
		UE_LOG(LogHMD, Log, TEXT("xrCreateSession failed"));
		return false;
	}

	uint32_t ReferenceSpacesCount;
	XR_ENSURE(xrEnumerateReferenceSpaces(Session, 0, &ReferenceSpacesCount, nullptr));

	TArray<XrReferenceSpaceType> Spaces;
	Spaces.SetNum(ReferenceSpacesCount);
	// Initialize spaces array with valid enum values (avoid triggering validation error).
	for (auto& SpaceIter : Spaces)
		SpaceIter = XR_REFERENCE_SPACE_TYPE_VIEW;
	XR_ENSURE(xrEnumerateReferenceSpaces(Session, (uint32_t)Spaces.Num(), &ReferenceSpacesCount, Spaces.GetData()));
	ensure(ReferenceSpacesCount == Spaces.Num());

	XrSpace HmdSpace = XR_NULL_HANDLE;
	XrReferenceSpaceCreateInfo SpaceInfo;

	ensure(Spaces.Contains(XR_REFERENCE_SPACE_TYPE_VIEW));
	SpaceInfo.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
	SpaceInfo.next = nullptr;
	SpaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
	SpaceInfo.poseInReferenceSpace = ToXrPose(FTransform::Identity);
	XR_ENSURE(xrCreateReferenceSpace(Session, &SpaceInfo, &HmdSpace));
	DeviceSpaces[HMDDeviceId].Space = HmdSpace;

	ensure(Spaces.Contains(XR_REFERENCE_SPACE_TYPE_LOCAL));
	SpaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
	XR_ENSURE(xrCreateReferenceSpace(Session, &SpaceInfo, &LocalSpace));

	ensure(Spaces.Contains(XR_REFERENCE_SPACE_TYPE_STAGE));
	SpaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
	XR_ENSURE(xrCreateReferenceSpace(Session, &SpaceInfo, &StageSpace));

	// Prefer a floor space over a local space
	if (Spaces.Contains(XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR_EXT))
	{
		TrackingSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR_EXT;
		SpaceInfo.referenceSpaceType = TrackingSpaceType;
		XR_ENSURE(xrCreateReferenceSpace(Session, &SpaceInfo, &FloorSpace));
	}
	else
	{
		TrackingSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
	}

	// Create initial tracking space
	BaseOrientation = FQuat::Identity;
	BasePosition = FVector::ZeroVector;
	PipelinedFrameStateGame.TrackingSpace = MakeShared<FTrackingSpace>(TrackingSpaceType);
	PipelinedFrameStateGame.TrackingSpace->CreateSpace(Session);

	// Create action spaces for all devices
	for (FDeviceSpace& DeviceSpace : DeviceSpaces)
	{
		DeviceSpace.CreateSpace(Session);
	}

	if (RenderBridge.IsValid())
	{
		RenderBridge->SetOpenXRHMD(this);
	}

	// Set color space linear when using mobile hdr
	if(bIsMobileHDREnabled)
	{
		PFN_xrSetColorSpaceYVR SetColorSpaceYVR;
		XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrSetColorSpaceYVR", (PFN_xrVoidFunction*)&SetColorSpaceYVR));
		XR_ENSURE(SetColorSpaceYVR(Session, XrColorSpaceYVR::XR_COLOR_SPACE_Linear_YVR));
	}

	// grab a pointer to the renderer module for displaying our mirror window
	static const FName RendererModuleName("Renderer");
	RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);

	return true;
}

bool FYvrXRHMD::OnStereoTeardown()
{
	XrResult Result = XR_ERROR_SESSION_NOT_RUNNING;
	{
		FReadScopeLock Lock(SessionHandleMutex);
		if (Session != XR_NULL_HANDLE)
		{
			Result = xrRequestExitSession(Session);
		}
	}

	if (Result == XR_ERROR_SESSION_NOT_RUNNING)
	{
		// Session was never running - most likely PIE without putting the headset on.
		DestroySession();
	}
	else
	{
		XR_ENSURE(Result);
	}
	return true;
}

void FYvrXRHMD::DestroySession()
{
	// FlushRenderingCommands must be called outside of SessionLock since some rendering threads will also lock this mutex.
	FlushRenderingCommands();
	FWriteScopeLock SessionLock(SessionHandleMutex);

	if (Session != XR_NULL_HANDLE)
	{
		FWriteScopeLock DeviceLock(DeviceMutex);

		// We need to reset all swapchain references to ensure there are no attempts
		// to destroy swapchain handles after the session is already destroyed.
		ForEachLayer([&](uint32 /* unused */, FYvrXRLayer& Layer)
		{
			Layer.RightEye.Swapchain.Reset();
			Layer.LeftEye.Swapchain.Reset();
		});

		PipelinedLayerStateRendering.ColorSwapchain.Reset();
		PipelinedLayerStateRendering.DepthSwapchain.Reset();
		PipelinedLayerStateRendering.NativeOverlaySwapchains.Reset();
		PipelinedLayerStateRendering.MotionVectorSwapchain.Reset();
		PipelinedLayerStateRendering.MotionVectorDepthSwapchain.Reset();

		// TODO: Once we handle OnFinishRendering_RHIThread + StopSession interactions
		// properly, we can release these shared pointers in that function, and use
		// `ensure` here to make sure these are released.
		PipelinedLayerStateRHI.ColorSwapchain.Reset();
		PipelinedLayerStateRHI.DepthSwapchain.Reset();
		PipelinedLayerStateRHI.NativeOverlaySwapchains.Reset();
		PipelinedLayerStateRHI.MotionVectorSwapchain.Reset();
		PipelinedLayerStateRHI.MotionVectorDepthSwapchain.Reset();

		PipelinedFrameStateGame.TrackingSpace.Reset();
		PipelinedFrameStateRendering.TrackingSpace.Reset();
		PipelinedFrameStateRHI.TrackingSpace.Reset();

		// Reset the frame state.
		PipelinedFrameStateGame.FrameState = XrFrameState{ XR_TYPE_FRAME_STATE };
		PipelinedFrameStateRendering.FrameState = XrFrameState{ XR_TYPE_FRAME_STATE };
		PipelinedFrameStateRHI.FrameState = XrFrameState{ XR_TYPE_FRAME_STATE };

		// VRFocus must be reset so FWindowsApplication::PollGameDeviceState does not incorrectly short-circuit.
		FApp::SetUseVRFocus(false);
		FApp::SetHasVRFocus(false);

		// Destroy device spaces, they will be recreated
		// when the session is created again.
		for (FDeviceSpace& Device : DeviceSpaces)
		{
			Device.DestroySpace();
		}

		// Close the session now we're allowed to.
		XR_ENSURE(xrDestroySession(Session));
		Session = XR_NULL_HANDLE;

		bStereoEnabled = false;
		bIsReady = false;
		bIsRunning = false;
		bIsRendering = false;
		bIsSynchronized = false;
		bNeedReAllocatedDepth = true;
		bNeedReAllocatedMotionVector = true;
	}
}

int32 FYvrXRHMD::AddActionDevice(XrAction Action, XrPath Path)
{
	FWriteScopeLock DeviceLock(DeviceMutex);

	// Ensure the HMD device is already emplaced
	ensure(DeviceSpaces.Num() > 0);

	int32 DeviceId = DeviceSpaces.Emplace(Action, Path);

	FReadScopeLock Lock(SessionHandleMutex);
	if (Session)
	{
		DeviceSpaces[DeviceId].CreateSpace(Session);
	}

	return DeviceId;
}

void FYvrXRHMD::ResetActionDevices()
{
	FWriteScopeLock DeviceLock(DeviceMutex);

	// Index 0 is HMDDeviceId and is preserved. The remaining are action devices.
	if (DeviceSpaces.Num() > 0)
	{
		DeviceSpaces.RemoveAt(HMDDeviceId + 1, DeviceSpaces.Num() - 1);
	}
}

XrPath FYvrXRHMD::GetTrackedDevicePath(const int32 DeviceId)
{
	FReadScopeLock DeviceLock(DeviceMutex);
	if (DeviceSpaces.IsValidIndex(DeviceId))
	{
		return DeviceSpaces[DeviceId].Path;
	}
	return XR_NULL_PATH;
}

XrSpace FYvrXRHMD::GetTrackingSpace()
{
	const FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();
	if (PipelineState.TrackingSpace.IsValid())
	{
		return PipelineState.TrackingSpace->Handle;
	}
	else
	{
		return XR_NULL_HANDLE;
	}
}

XrTime FYvrXRHMD::GetDisplayTime() const
{
	const FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();
	return PipelineState.FrameState.predictedDisplayTime;
}

YVRXRHMD_API bool FYvrXRHMD::IsHandTrackingEnabled() const
{
	bool HandTrackingEnabled = false;
	HandTrackingEnabled = bSupportsHandTracking && (Settings->HandTrackingSupport != EYvrXRHandTrackingSupport::ControllersOnly);
	return HandTrackingEnabled;
}

bool FYvrXRHMD::IsInitialized() const
{
	return Instance != XR_NULL_HANDLE;
}

bool FYvrXRHMD::IsRunning() const
{
	return bIsRunning;
}

bool FYvrXRHMD::IsFocused() const
{
	return CurrentSessionState == XR_SESSION_STATE_FOCUSED;
}

void FYvrXRHMD::SetCpuLevel(const uint8 CpuLevel)
{
	Settings->CPULevel = (EYvrPerformanceLevel)CpuLevel;

	PFN_xrPerfSettingsSetPerformanceLevelEXT PerfSettingsSetPerformanceLevelEXT;
	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrPerfSettingsSetPerformanceLevelEXT", (PFN_xrVoidFunction*)&PerfSettingsSetPerformanceLevelEXT));
	XR_ENSURE(PerfSettingsSetPerformanceLevelEXT(Session, XR_PERF_SETTINGS_DOMAIN_CPU_EXT, (XrPerfSettingsLevelEXT)CpuLevel));
}

void FYvrXRHMD::SetGpuLevel(const uint8 GpuLevel)
{
	Settings->GPULevel = (EYvrPerformanceLevel)GpuLevel;

	PFN_xrPerfSettingsSetPerformanceLevelEXT PerfSettingsSetPerformanceLevelEXT;
	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrPerfSettingsSetPerformanceLevelEXT", (PFN_xrVoidFunction*)&PerfSettingsSetPerformanceLevelEXT));
	XR_ENSURE(PerfSettingsSetPerformanceLevelEXT(Session, XR_PERF_SETTINGS_DOMAIN_GPU_EXT, (XrPerfSettingsLevelEXT)GpuLevel));
}

void FYvrXRHMD::SetDisplayRefreshRate(const float RefreshRate)
{
	PFN_xrRequestDisplayRefreshRateFB RequestDisplayRefreshRateFB;
	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrRequestDisplayRefreshRateFB", (PFN_xrVoidFunction*)&RequestDisplayRefreshRateFB));
	XR_ENSURE(RequestDisplayRefreshRateFB(Session, RefreshRate));
}

float FYvrXRHMD::GetDisplayRefreshRate()
{
	float DisplayRefreshRate = 0.0f;
	PFN_xrGetDisplayRefreshRateFB GetDisplayRefreshRateFB;
	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrGetDisplayRefreshRateFB", (PFN_xrVoidFunction*)&GetDisplayRefreshRateFB));
	XR_ENSURE(GetDisplayRefreshRateFB(Session, &DisplayRefreshRate));

	return DisplayRefreshRate;
}

void FYvrXRHMD::SetFFRLevel(const uint8 Level)
{
	Settings->FFRLevel = (EYvrFixedFoveatedRenderingLevel)Level;
}

uint8 FYvrXRHMD::GetFFRLevel() const
{
	return (uint8)Settings->FFRLevel;
}

void FYvrXRHMD::SetColorScaleAndOffset(FLinearColor ColorScale, FLinearColor ColorOffset, bool bApplyToAllLayers)
{
	Settings->ColorScale = ColorScale;
	Settings->ColorOffset = ColorOffset;
	Settings->bApplyColorScaleAndOffsetToAllLayers = bApplyToAllLayers;
}

void FYvrXRHMD::SetSpaceWarpEnabled(bool bIsEnabled)
{
	Settings->bUseSpaceWarp = bIsEnabled;
}

void FYvrXRHMD::SetSeeThroughBackgroundEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		PFN_xrPassthroughStartYVR xrPassthroughStartYVR;
		XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrPassthroughStartYVR", (PFN_xrVoidFunction*)&xrPassthroughStartYVR));
		XR_ENSURE(xrPassthroughStartYVR(Session));
	}
	else
	{
		PFN_xrPassthroughStopYVR xrPassthroughStopYVR;
		XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrPassthroughStopYVR", (PFN_xrVoidFunction*)&xrPassthroughStopYVR));
		XR_ENSURE(xrPassthroughStopYVR(Session));
	}
}

void FYvrXRHMD::SetSharpenType(const uint8 SharpenType, bool bApplyToAllLayers)
{
	Settings->SharpenType = (EYvrLayerSharpenType)SharpenType;
	Settings->bApplySharpenTypeToAllLayers = bApplyToAllLayers;
}

bool FYvrXRHMD::StartSession()
{
	// If the session is not yet ready, we'll call into this function again when it is
	FWriteScopeLock Lock(SessionHandleMutex);
	if (!bIsReady || bIsRunning)
	{
		return false;
	}

	XrSessionBeginInfo Begin = { XR_TYPE_SESSION_BEGIN_INFO, nullptr, SelectedViewConfigurationType };
	bIsRunning = XR_ENSURE(xrBeginSession(Session, &Begin));

	SetCpuLevel((uint8)Settings->CPULevel);
	SetGpuLevel((uint8)Settings->GPULevel);

	return bIsRunning;
}

bool FYvrXRHMD::StopSession()
{
	FWriteScopeLock Lock(SessionHandleMutex);
	if (!bIsRunning)
	{
		return false;
	}

	bIsRunning = !XR_ENSURE(xrEndSession(Session));
	return !bIsRunning;
}

void FYvrXRHMD::OnBeginPlay(FWorldContext& InWorldContext)
{
}

void FYvrXRHMD::OnEndPlay(FWorldContext& InWorldContext)
{
}

IStereoRenderTargetManager* FYvrXRHMD::GetRenderTargetManager()
{
	return this;
}

bool FYvrXRHMD::AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples)
{
	check(IsInRenderingThread());

	FReadScopeLock Lock(SessionHandleMutex);
	if (!Session)
	{
		return false;
	}

	// This is not a static swapchain
	Flags |= TexCreate_Dynamic;

	// We need to ensure we can sample from the texture in CopyTexture
	TargetableTextureFlags |= TexCreate_ShaderResource;

	TargetableTextureFlags |= TexCreate_ResolveTargetable;

	TargetableTextureFlags |= TexCreate_RenderTargetable;

	// On mobile without HDR all render targets need to be marked sRGB
	bool MobileHWsRGB = IsMobileColorsRGB() && IsMobilePlatform(GMaxRHIShaderPlatform);
	if (MobileHWsRGB)
	{
		TargetableTextureFlags |= TexCreate_SRGB;
	}

	// Temporary workaround to swapchain formats - OpenXR doesn't support 10-bit sRGB swapchains, so prefer 8-bit sRGB instead.
	if (Format == PF_A2B10G10R10)
	{
		Format = PF_R8G8B8A8;
	}

	if (Settings->RenderTargetSize.X != 0 && Settings->RenderTargetSize.Y != 0)
	{
		SizeX = Settings->RenderTargetSize.X;
		SizeY = Settings->RenderTargetSize.Y;
	}

	FClearValueBinding ClearColor = (SelectedEnvironmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_OPAQUE) ? FClearValueBinding::Black : FClearValueBinding::Transparent;

	TArray<FXRSwapChainPtr> TempSwapchainArray;
	FXRSwapChainPtr& Swapchain = PipelinedLayerStateRendering.ColorSwapchain;
	FXRSwapChainPtr& FoveationSwapchain = PipelinedLayerStateRendering.FoveationSwapchain;
	const FRHITexture2D* const SwapchainTexture = Swapchain == nullptr ? nullptr : Swapchain->GetTexture2DArray() ? Swapchain->GetTexture2DArray() : Swapchain->GetTexture2D();
	if (Swapchain == nullptr || SwapchainTexture == nullptr || Format != LastRequestedSwapchainFormat || SwapchainTexture->GetSizeX() != SizeX || SwapchainTexture->GetSizeY() != SizeY)
	{
		ensureMsgf(NumSamples == 1, TEXT("OpenXR supports MSAA swapchains, but engine logic expects the swapchain target to be 1x."));

		TempSwapchainArray = RenderBridge->CreateSwapchainWithFoveation(Session, Format, SizeX, SizeY, bIsMobileMultiViewEnabled ? 2 : 1, NumMips, NumSamples, Flags, TargetableTextureFlags, ClearColor);

		 if(TempSwapchainArray.Num() == 1)
		 {
		 	Swapchain = TempSwapchainArray[0];
		 }
		 else if(TempSwapchainArray.Num() == 2)
		 {
		 	Swapchain = TempSwapchainArray[0];
		 	FoveationSwapchain = TempSwapchainArray[1];
		 }

		if (!Swapchain)
		{
			return false;
		}

		// Acquire the first swapchain image
		Swapchain->IncrementSwapChainIndex_RHIThread();

#if WITH_EDITOR
		if (GIsEditor)
		{
			if (FSceneViewport* SceneVP = FindSceneViewport())
			{
				TSharedPtr<SWindow> Window = SceneVP->FindWindow();
				if (Window.IsValid())
				{
					// Window continues to be processed when PIE spectator window is minimized
					Window->SetIndependentViewportSize(FVector2D(SizeX, SizeY));
				}
			}
		}
#endif
	}

	// Grab the presentation texture out of the swapchain.
	OutTargetableTexture = OutShaderResourceTexture = (FTexture2DRHIRef&)Swapchain->GetTextureRef();
	LastRequestedSwapchainFormat = Format;

	bNeedReAllocatedDepth = bDepthExtensionSupported;
	bNeedReAllocatedMotionVector = bEnableSpaceWarp;
	bNeedReAllocatedFoveation = true;

	return true;
}

bool FYvrXRHMD::AllocateDepthTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples)
{
	check(IsInRenderingThread());

	// FIXME: UE4 constantly calls this function even when there is no reason to reallocate the depth texture
	FReadScopeLock Lock(SessionHandleMutex);
	if (!Session || !bDepthExtensionSupported)
	{
		return false;
	}

	// This is not a static swapchain
	Flags |= TexCreate_Dynamic;

	FXRSwapChainPtr& Swapchain = PipelinedLayerStateRendering.DepthSwapchain;
	const FRHITexture2D* const DepthSwapchainTexture = Swapchain == nullptr ? nullptr : Swapchain->GetTexture2DArray() ? Swapchain->GetTexture2DArray() : Swapchain->GetTexture2D();
	if (Swapchain == nullptr || DepthSwapchainTexture == nullptr || Format != LastRequestedDepthSwapchainFormat || DepthSwapchainTexture->GetSizeX() != SizeX || DepthSwapchainTexture->GetSizeY() != SizeY)
	{
		ensureMsgf(NumSamples == 1, TEXT("OpenXR supports MSAA swapchains, but engine logic expects the swapchain target to be 1x."));

		Swapchain = RenderBridge->CreateSwapchain(Session, PF_DepthStencil, SizeX, SizeY, bIsMobileMultiViewEnabled ? 2 : 1, FMath::Max(NumMips, 1u), NumSamples, Flags, TargetableTextureFlags, FClearValueBinding::DepthFar);
		if (!Swapchain)
		{
			return false;
		}

		// Acquire the first swapchain image
		Swapchain->IncrementSwapChainIndex_RHIThread();
	}

	bNeedReAllocatedDepth = false;

	OutTargetableTexture = OutShaderResourceTexture = (FTexture2DRHIRef&)Swapchain->GetTextureRef();
	LastRequestedDepthSwapchainFormat = Format;

	return true;
}

#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
bool FYvrXRHMD::NeedReAllocateShadingRateTexture(const TRefCountPtr<IPooledRenderTarget>& FoveationTarget)
#else
bool FYvrXRHMD::NeedReAllocateFoveationTexture(const TRefCountPtr<IPooledRenderTarget>& FoveationTarget)
#endif
{
	return bNeedReAllocatedFoveation;
}

#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
bool FYvrXRHMD::AllocateShadingRateTexture(uint32 Index, uint32 RenderSizeX, uint32 RenderSizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags InTexFlags, ETextureCreateFlags InTargetableTextureFlags, FTexture2DRHIRef& OutTexture, FIntPoint& OutTextureSize)
#else
bool FYvrXRHMD::AllocateFoveationTexture(uint32 Index, uint32 RenderSizeX, uint32 RenderSizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags InTexFlags, ETextureCreateFlags InTargetableTextureFlags, FTexture2DRHIRef& OutTexture, FIntPoint& OutTextureSize)
#endif
{
	FXRSwapChainPtr& FoveationSwapchain = PipelinedLayerStateRendering.FoveationSwapchain;

#if ENGINE_MAJOR_VERSION > 4
	//TODO: Fix GRHIVariableRateShadingImageFormat issue
	GRHISupportsAttachmentVariableRateShading = false;
#endif

	if(!FoveationSwapchain)
	{
		return false;
	}

	bNeedReAllocatedFoveation = false;

	const FTexture2DRHIRef& FoveationTexture = (FTexture2DRHIRef&)FoveationSwapchain->GetTextureRef();

	OutTexture = FoveationTexture;
	OutTextureSize = FoveationTexture->GetSizeXY();

	return true;
}

#ifdef YVR_APPSW

bool FYvrXRHMD::NeedReAllocateMotionVectorTexture(const TRefCountPtr<IPooledRenderTarget>& MotionVectorTarget, const TRefCountPtr<IPooledRenderTarget>& MotionVectorDepthTarget)
{
	return bNeedReAllocatedMotionVector;
}

bool FYvrXRHMD::AllocateMotionVectorTexture(uint32 Index, uint8 Format, uint32 NumMips, uint32 InTexFlags, uint32 InTargetableTextureFlags, FTexture2DRHIRef& OutTexture, FIntPoint& OutTextureSize, FTexture2DRHIRef& OutDepthTexture, FIntPoint& OutDepthTextureSize)
{
	check(IsInRenderingThread());

	FReadScopeLock Lock(SessionHandleMutex);
	if (!Session)
	{
		return false;
	}

	FClearValueBinding ClearColor = FClearValueBinding::Black;
	uint32 SizeX = RecommendedMotionVectorImageRectWidth;
	uint32 SizeY = RecommendedMotionVectorImageRectHeight;

	uint32 MotionVectorTexFlags = 0;
	uint32 MotionVectorTargetableTextureFlags = 0;
	uint8 MotionVectorFormat = 0;
	MotionVectorTexFlags |= TexCreate_Dynamic;
	MotionVectorTargetableTextureFlags = TexCreate_ShaderResource | TexCreate_RenderTargetable;
	MotionVectorFormat = PF_FloatRGBA;

	// Motion vector texture
	FXRSwapChainPtr& Swapchain = PipelinedLayerStateRendering.MotionVectorSwapchain;
	const FRHITexture2D* const SwapchainTexture = Swapchain == nullptr ? nullptr : Swapchain->GetTexture2DArray() ? Swapchain->GetTexture2DArray() : Swapchain->GetTexture2D();
	if (Swapchain == nullptr || SwapchainTexture == nullptr)
	{
		Swapchain = RenderBridge->CreateSwapchain(Session, MotionVectorFormat, SizeX, SizeY, 2, 1, 1, (ETextureCreateFlags)MotionVectorTexFlags, (ETextureCreateFlags)MotionVectorTargetableTextureFlags, ClearColor);
		if (!Swapchain)
		{
			return false;
		}

		// Acquire the first swapchain image
		Swapchain->IncrementSwapChainIndex_RHIThread();
	}

	OutTexture = (FTexture2DRHIRef&)Swapchain->GetTextureRef();
	OutTextureSize = FIntPoint(SizeX, SizeY);

	uint32 MotionVectorDepthTexFlags = 0;
	uint32 MotionVectorDepthTargetableTextureFlags = 0;
	uint8 MotionVectorDepthFormat = 0;
	MotionVectorDepthTexFlags |= TexCreate_Dynamic;
	MotionVectorDepthTargetableTextureFlags = TexCreate_ShaderResource | TexCreate_DepthStencilTargetable;
	MotionVectorDepthFormat = PF_DepthStencil;

	// Motion vector depth texture
	FXRSwapChainPtr& DepthSwapchain = PipelinedLayerStateRendering.MotionVectorDepthSwapchain;
	const FRHITexture2D* const DepthSwapchainTexture = DepthSwapchain == nullptr ? nullptr : DepthSwapchain->GetTexture2DArray() ? DepthSwapchain->GetTexture2DArray() : DepthSwapchain->GetTexture2D();
	if (DepthSwapchain == nullptr || DepthSwapchainTexture == nullptr)
	{
		DepthSwapchain = RenderBridge->CreateSwapchain(Session, MotionVectorDepthFormat, SizeX, SizeY, 2, 1, 1, (ETextureCreateFlags)MotionVectorDepthTexFlags, (ETextureCreateFlags)MotionVectorDepthTargetableTextureFlags, ClearColor);
		if (!DepthSwapchain)
		{
			return false;
		}

		// Acquire the first swapchain image
		DepthSwapchain->IncrementSwapChainIndex_RHIThread();
	}

	OutDepthTexture = (FTexture2DRHIRef&)DepthSwapchain->GetTextureRef();
	OutDepthTextureSize = FIntPoint(SizeX, SizeY);

	bNeedReAllocatedMotionVector = false;

	return true;
}

#endif

void CreateNativeLayerSwapchain(FYvrXRLayer& Layer, TRefCountPtr<FYvrXRRenderBridge>& RenderBridge, XrSession Session, uint32 FaceCount = 1)
{
	auto CreateSwapchain = [&](FRHITexture* RHITexture, ETextureCreateFlags Flags)
	{
		FRHITexture2D* Texture2D = RHITexture->GetTexture2D();
		FRHITextureCube* TextureCube = RHITexture->GetTextureCube();

		return RenderBridge->CreateSwapchain(Session,
			PF_B8G8R8A8,
			Texture2D ? Texture2D->GetSizeX() : TextureCube->GetSize(),
			Texture2D ? Texture2D->GetSizeY() : TextureCube->GetSize(),
			1,
			RHITexture->GetNumMips(),
			RHITexture->GetNumSamples(),
			RHITexture->GetFlags() | Flags,
			TexCreate_RenderTargetable | TexCreate_SRGB,
			RHITexture->GetClearBinding(),
			FaceCount);
	};

	const ETextureCreateFlags Flags = Layer.Desc.Flags & IStereoLayers::LAYER_FLAG_TEX_CONTINUOUS_UPDATE ?
		TexCreate_Dynamic | TexCreate_SRGB : TexCreate_SRGB;

	if (Layer.NeedReallocateRightTexture())
	{
		FRHITexture2D* Texture2D = Layer.Desc.Texture->GetTexture2D();
		FRHITextureCube* TextureCube = Layer.Desc.Texture->GetTextureCube();

		Layer.RightEye.SetSwapchain(CreateSwapchain(Layer.Desc.Texture, Flags), Texture2D ? Texture2D->GetSizeXY() : TextureCube->GetSize());
	}

	if (Layer.NeedReallocateLeftTexture())
	{
		FRHITexture2D* Texture2D = Layer.Desc.LeftTexture->GetTexture2D();
		FRHITextureCube* TextureCube = Layer.Desc.LeftTexture->GetTextureCube();

		Layer.LeftEye.SetSwapchain(CreateSwapchain(Layer.Desc.Texture, Flags), Texture2D ? Texture2D->GetSizeXY() : TextureCube->GetSize());
	}
}


void FYvrXRHMD::ConfigureLayerSwapchain(FYvrXRLayer& Layer)
{
	if (Layer.Desc.HasShape<FCylinderLayer>() || Layer.Desc.HasShape<FEquirectLayer>() || Layer.Desc.HasShape<FQuadLayer>())
	{
		if (Layer.Desc.IsVisible())
		{
			CreateNativeLayerSwapchain(Layer, RenderBridge, Session);
		}
		else
		{
			// We retain references in FPipelinedLayerState to avoid premature destruction
			Layer.RightEye.Swapchain.Reset();
			Layer.LeftEye.Swapchain.Reset();
		}
	}
	else if (Layer.Desc.HasShape<FCubemapLayer>())
	{
		if (Layer.Desc.IsVisible())
		{
			CreateNativeLayerSwapchain(Layer, RenderBridge, Session, 6);
		}
		else
		{
			// We retain references in FPipelinedLayerState to avoid premature destruction
			Layer.RightEye.Swapchain.Reset();
			Layer.LeftEye.Swapchain.Reset();
		}
	}
}

void FYvrXRHMD::UpdateLayerSwapchainTexture(const FYvrXRLayer& Layer, FRHICommandListImmediate& RHICmdList)
{
	const bool bNoAlpha = Layer.Desc.Flags & IStereoLayers::LAYER_FLAG_TEX_NO_ALPHA_CHANNEL;

	// We need to copy each layer into an OpenXR swapchain so they can be displayed by the compositor.
	if (Layer.RightEye.Swapchain.IsValid() && Layer.Desc.Texture.IsValid())
	{
		if (Layer.RightEye.bUpdateTexture && bIsRunning)
		{
			FIntRect DstRect(FIntPoint(0, 0), Layer.RightEye.SwapchainSize.IntPoint());
			CopyTexture_RenderThread(RHICmdList, Layer.Desc.Texture, FIntRect(), Layer.RightEye.Swapchain, DstRect, false, bNoAlpha);
		}
		PipelinedLayerStateRendering.NativeOverlaySwapchains.Add(Layer.RightEye.Swapchain);
	}
	if (Layer.LeftEye.Swapchain.IsValid() && Layer.Desc.LeftTexture.IsValid())
	{
		if (Layer.LeftEye.bUpdateTexture && bIsRunning)
		{
			FIntRect DstRect(FIntPoint(0, 0), Layer.LeftEye.SwapchainSize.IntPoint());
			CopyTexture_RenderThread(RHICmdList, Layer.Desc.LeftTexture, FIntRect(), Layer.LeftEye.Swapchain, DstRect, false, bNoAlpha);
		}
		PipelinedLayerStateRendering.NativeOverlaySwapchains.Add(Layer.LeftEye.Swapchain);
	}
}

void FYvrXRHMD::SetupFrameLayers_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	ensure(IsInRenderingThread());

	const FTransform InvTrackingToWorld = GetTrackingToWorldTransform().Inverse();
	const float WorldToMeters = GetWorldToMetersScale();

	if (GetStereoLayersDirty())
	{
		// Go over the dirtied layers to bin them into either native or emulated
		ForEachLayer([&](uint32 LayerId, FYvrXRLayer& Layer)
		{
			ConfigureLayerSwapchain(Layer);
		});
	}

	TArray<FYvrXRLayer> NativeLayers;
	CopySortedAllLayers(NativeLayers);
	PipelinedLayerStateRendering.NativeOverlays.Reset(NativeLayers.Num());
	PipelinedLayerStateRendering.NativeOverlaySwapchains.Reset(NativeLayers.Num());

	// Set up our OpenXR info per native layer. Emulated layers have everything in FLayerDesc.
	for (const FYvrXRLayer& Layer : NativeLayers)
	{
		FReadScopeLock DeviceLock(DeviceMutex);

		XrSpace Space = Layer.Desc.PositionType == ELayerType::FaceLocked ?
			DeviceSpaces[HMDDeviceId].Space : PipelinedFrameStateRendering.TrackingSpace->Handle;

		TArray<FXrCompositionLayerUnion> Headers = Layer.CreateOpenXRLayer(InvTrackingToWorld, WorldToMeters, Space);
		PipelinedLayerStateRendering.NativeOverlays.Append(Headers);
		UpdateLayerSwapchainTexture(Layer, RHICmdList);
	}
}


void FYvrXRHMD::OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	ensure(IsInRenderingThread());
	FReadScopeLock DeviceLock(DeviceMutex);

	const float WorldToMeters = GetWorldToMetersScale();
	const FTransform InvTrackingToWorld = GetTrackingToWorldTransform().Inverse();

	PipelinedFrameStateRendering = PipelinedFrameStateGame;

	for (int32 ViewIndex = 0; ViewIndex < PipelinedLayerStateRendering.ProjectionLayers.Num(); ViewIndex++)
	{
		const XrView& View = PipelinedFrameStateRendering.Views[ViewIndex];
		FTransform EyePose = ToFTransform(View.pose, WorldToMeters);

		// Apply the base HMD pose to each eye pose, we will late update this pose for late update in another callback
		FTransform BasePose(ViewFamily.Views[ViewIndex]->BaseHmdOrientation, ViewFamily.Views[ViewIndex]->BaseHmdLocation);
		XrCompositionLayerProjectionView& Projection = PipelinedLayerStateRendering.ProjectionLayers[ViewIndex];
		FTransform BasePoseTransform = EyePose * BasePose;
		BasePoseTransform.NormalizeRotation();
		Projection.pose = ToXrPose(BasePoseTransform, WorldToMeters);
		Projection.fov = View.fov;

		if (bEnableSpaceWarp)
		{
			if (PipelinedFrameStateRendering.bIsSpaceWarpFrame)
			{
				XrCompositionLayerSpaceWarpInfoFB& MotionVectorLayer = PipelinedLayerStateRendering.MotionVectorLayers[ViewIndex];
				FTransform TrackingSpaceDeltaPose = PipelinedFrameStateRendering.TrackingToWorld * PipelinedFrameStateRendering.LastTrackingToWorld.Inverse();
				MotionVectorLayer.appSpaceDeltaPose = ToXrPose(TrackingSpaceDeltaPose, WorldToMeters);
			}
		}
	}

	SetupFrameLayers_RenderThread(RHICmdList);
	if (bIsRunning)
	{
		// Locate the views we will actually be rendering for.
		// This is required to support late-updating the field-of-view.
		LocateViews(PipelinedFrameStateRendering, false);

		SCOPED_NAMED_EVENT(EnqueueFrame, FColor::Red);

		// Reset the update flag on all layers
		ForEachLayer([&](uint32 /* unused */, FYvrXRLayer& Layer)
		{
			const bool bUpdateTexture = Layer.Desc.Flags & IStereoLayers::LAYER_FLAG_TEX_CONTINUOUS_UPDATE;
			Layer.RightEye.bUpdateTexture = bUpdateTexture;
			Layer.LeftEye.bUpdateTexture = bUpdateTexture;
		}, false);

		FXRSwapChainPtr ColorSwapchain = PipelinedLayerStateRendering.ColorSwapchain;
		FXRSwapChainPtr DepthSwapchain = PipelinedLayerStateRendering.DepthSwapchain;
		FXRSwapChainPtr MotionVectorSwapchain = PipelinedLayerStateRendering.MotionVectorSwapchain;
		FXRSwapChainPtr MotionVectorDepthSwapchain = PipelinedLayerStateRendering.MotionVectorDepthSwapchain;

		RHICmdList.EnqueueLambda([this, FrameState = PipelinedFrameStateRendering, ColorSwapchain, DepthSwapchain, MotionVectorSwapchain, MotionVectorDepthSwapchain](FRHICommandListImmediate& InRHICmdList)
		{
			OnBeginRendering_RHIThread(FrameState, ColorSwapchain, DepthSwapchain, MotionVectorSwapchain, MotionVectorDepthSwapchain);
		});
	}

	// Snapshot new poses for late update.
	UpdateDeviceLocations(false);
}

void FYvrXRHMD::LocateViews(FPipelinedFrameState& PipelineState, bool ResizeViewsArray)
{
	check(PipelineState.FrameState.predictedDisplayTime);
	FReadScopeLock DeviceLock(DeviceMutex);

	uint32_t ViewCount = 0;
	XrViewLocateInfo ViewInfo;
	ViewInfo.type = XR_TYPE_VIEW_LOCATE_INFO;
	ViewInfo.next = nullptr;
	ViewInfo.viewConfigurationType = SelectedViewConfigurationType;
	ViewInfo.space = DeviceSpaces[HMDDeviceId].Space;
	ViewInfo.displayTime = PipelineState.FrameState.predictedDisplayTime;

	XR_ENSURE(xrLocateViews(Session, &ViewInfo, &PipelineState.ViewState, 0, &ViewCount, nullptr));
	if (ResizeViewsArray)
	{
		PipelineState.Views.SetNum(ViewCount, false);
	}
	else
	{
		// PipelineState.Views.Num() can be greater than ViewCount if there is an IOpenXRExtensionPlugin
		// which appends more views with the GetViewLocations callback.
		ensure(PipelineState.Views.Num() >= (int32)ViewCount);
	}

	XR_ENSURE(xrLocateViews(Session, &ViewInfo, &PipelineState.ViewState, PipelineState.Views.Num(), &ViewCount, PipelineState.Views.GetData()));
}

#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
void FYvrXRHMD::OnLateUpdateApplied_RenderThread(FRHICommandListImmediate& RHICmdList, const FTransform& NewRelativeTransform)
{
	FHeadMountedDisplayBase::OnLateUpdateApplied_RenderThread(RHICmdList, NewRelativeTransform);
#else
void FYvrXRHMD::OnLateUpdateApplied_RenderThread(const FTransform& NewRelativeTransform)
{
	FHeadMountedDisplayBase::OnLateUpdateApplied_RenderThread(NewRelativeTransform);
#endif

	ensure(IsInRenderingThread());

	for (int32 ViewIndex = 0; ViewIndex < PipelinedLayerStateRendering.ProjectionLayers.Num(); ViewIndex++)
	{
		const XrView& View = PipelinedFrameStateRendering.Views[ViewIndex];
		XrCompositionLayerProjectionView& Projection = PipelinedLayerStateRendering.ProjectionLayers[ViewIndex];

		// Apply the new HMD orientation to each eye pose for the final pose
		FTransform EyePose = ToFTransform(View.pose, GetWorldToMetersScale());
		FTransform NewRelativePoseTransform = EyePose * NewRelativeTransform;
		NewRelativePoseTransform.NormalizeRotation();
		Projection.pose = ToXrPose(NewRelativePoseTransform, GetWorldToMetersScale());

		// Update the field-of-view to match the final projection matrix
		Projection.fov = View.fov;

		if (bEnableSpaceWarp)
		{
			if (PipelinedFrameStateRendering.bIsSpaceWarpFrame)
			{
				XrCompositionLayerSpaceWarpInfoFB& MotionVectorLayer = PipelinedLayerStateRendering.MotionVectorLayers[ViewIndex];
				FTransform TrackingSpaceDeltaPose = PipelinedFrameStateRendering.TrackingToWorld * PipelinedFrameStateRendering.LastTrackingToWorld.Inverse();
				MotionVectorLayer.appSpaceDeltaPose = ToXrPose(TrackingSpaceDeltaPose, GetWorldToMetersScale());
			}
		}
	}

	ExecuteOnRHIThread_DoNotWait([this, ProjectionLayers = PipelinedLayerStateRendering.ProjectionLayers]()
	{
		PipelinedLayerStateRHI.ProjectionLayers = ProjectionLayers;
	});
}

void FYvrXRHMD::OnBeginRendering_GameThread()
{
	FReadScopeLock Lock(SessionHandleMutex);
	if (!bIsReady || !bIsRunning)
	{
		// @todo: Sleep here?
		return;
	}

	ensure(IsInGameThread());

	SCOPED_NAMED_EVENT(WaitFrame, FColor::Red);

	XrFrameWaitInfo WaitInfo;
	WaitInfo.type = XR_TYPE_FRAME_WAIT_INFO;
	WaitInfo.next = nullptr;

	XrFrameState FrameState{ XR_TYPE_FRAME_STATE };
	XR_ENSURE(xrWaitFrame(Session, &WaitInfo, &FrameState));

	// The pipeline state on the game thread can only be safely modified after xrWaitFrame which will be unblocked by
	// the runtime when xrBeginFrame is called. The rendering thread will clone the game pipeline state before calling
	// xrBeginFrame so the game pipeline state can safely be modified after xrWaitFrame returns.
	FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();
	PipelineState.FrameState = FrameState;
	PipelineState.WorldToMetersScale = WorldToMetersScale;

	EnumerateViews(PipelineState);
}

bool FYvrXRHMD::ReadNextEvent(XrEventDataBuffer* buffer)
{
	// It is sufficient to clear just the XrEventDataBuffer header to XR_TYPE_EVENT_DATA_BUFFER
	XrEventDataBaseHeader* baseHeader = reinterpret_cast<XrEventDataBaseHeader*>(buffer);
	*baseHeader = { XR_TYPE_EVENT_DATA_BUFFER };
	const XrResult xr = xrPollEvent(Instance, buffer);
	XR_ENSURE(xr);
	if (xr == XR_SUCCESS)
	{
		return true;
	}

	return false;
}

bool FYvrXRHMD::OnStartGameFrame(FWorldContext& WorldContext)
{
	const AWorldSettings* const WorldSettings = WorldContext.World() ? WorldContext.World()->GetWorldSettings() : nullptr;
	if (WorldSettings)
	{
		WorldToMetersScale = WorldSettings->WorldToMeters;
	}

	// Only refresh this based on the game world.  When remoting there is also an editor world, which we do not want to have affect the transform.
	if (WorldContext.World()->IsGameWorld())
	{
		RefreshTrackingToWorldTransform(WorldContext);
	}

	// Process all pending messages.
	XrEventDataBuffer event;
	while (ReadNextEvent(&event))
	{
		switch (event.type)
		{
		case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
		{
			const XrEventDataSessionStateChanged& SessionState =
				reinterpret_cast<XrEventDataSessionStateChanged&>(event);

			CurrentSessionState = SessionState.state;

#if 0
			UE_LOG(LogHMD, Log, TEXT("Session state switching to %s"), OpenXRSessionStateToString(CurrentSessionState));
#endif

			if (SessionState.state == XR_SESSION_STATE_READY)
			{
				if (!GIsEditor)
				{
					GEngine->SetMaxFPS(0);
				}
				bIsReady = true;
				StartSession();
			}
			else if (SessionState.state == XR_SESSION_STATE_SYNCHRONIZED)
			{
				bIsSynchronized = true;
			}
			else if (SessionState.state == XR_SESSION_STATE_IDLE)
			{
				bIsSynchronized = false;
			}
			else if (SessionState.state == XR_SESSION_STATE_STOPPING)
			{
				if (!GIsEditor)
				{
					GEngine->SetMaxFPS(OPENXR_PAUSED_IDLE_FPS);
				}
				bIsReady = false;
				StopSession();
			}
			else if (SessionState.state == XR_SESSION_STATE_EXITING)
			{
				// We need to make sure we unlock the frame rate again when exiting stereo while idle
				if (!GIsEditor)
				{
					GEngine->SetMaxFPS(0);
				}
			}

			FApp::SetHasVRFocus(SessionState.state == XR_SESSION_STATE_FOCUSED);

			if (SessionState.state != XR_SESSION_STATE_EXITING && SessionState.state != XR_SESSION_STATE_LOSS_PENDING)
			{
				break;
			}
		}
		// Intentional fall-through
		case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
		{
#if WITH_EDITOR
			if (GIsEditor)
			{
				FSceneViewport* SceneVP = FindSceneViewport();
				if (SceneVP && SceneVP->IsStereoRenderingAllowed())
				{
					TSharedPtr<SWindow> Window = SceneVP->FindWindow();
					Window->RequestDestroyWindow();
				}
			}
			else
#endif//WITH_EDITOR
			{
				// ApplicationWillTerminateDelegate will fire from inside of the RequestExit
				FPlatformMisc::RequestExit(false);
			}

			DestroySession();

			break;
		}
		case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
		{
			const XrEventDataReferenceSpaceChangePending& SpaceChange =
				reinterpret_cast<XrEventDataReferenceSpaceChangePending&>(event);
			if (SpaceChange.referenceSpaceType == TrackingSpaceType)
			{
				OnTrackingOriginChanged();

				// Reset base orientation and position
				// TODO: If poseValid is true we can use poseInPreviousSpace to make the old base transform valid in the new space
				BaseOrientation = FQuat::Identity;
				BasePosition = FVector::ZeroVector;
			}
			break;
		}
		case XR_TYPE_EVENT_DATA_VISIBILITY_MASK_CHANGED_KHR:
		{
			break;
		}

		default:
		{
			PollEventDelegate.Broadcast(event);
			break;
		}
		}
	}

	// Add a display period to the simulation frame state so we're predicting poses for the new frame.
	FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();
	PipelineState.FrameState.predictedDisplayTime += PipelineState.FrameState.predictedDisplayPeriod;

	// Snapshot new poses for game simulation.
	UpdateDeviceLocations(true);
	UpdateStereoRenderingParams();

	return true;
}

bool FYvrXRHMD::OnEndGameFrame(FWorldContext& WorldContext)
{
	if (WorldContext.World()->IsGameWorld())
	{
		FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();
		FTransform SettingBasePose = ToFTransform(PipelineState.TrackingSpace->BasePose);
		PipelineState.TrackingToWorld = ComputeTrackingToWorldTransform(WorldContext) * SettingBasePose.Inverse();
		PipelineState.LastTrackingToWorld = LastTrackingToWorld;
		LastTrackingToWorld = PipelineState.TrackingToWorld;
	}
	return true;
}

void FYvrXRHMD::OnBeginRendering_RHIThread(const FPipelinedFrameState& InFrameState, FXRSwapChainPtr ColorSwapchain, FXRSwapChainPtr DepthSwapchain, FXRSwapChainPtr MotionVectorSwapchain, FXRSwapChainPtr MotionVectorDepthSwapchain)
{
	ensure(IsInRenderingThread() || IsInRHIThread());

	// TODO: Add a hook to resolve discarded frames before we start a new frame.
	checkSlow(!bIsRendering);

	SCOPED_NAMED_EVENT(BeginFrame, FColor::Red);

	FReadScopeLock Lock(SessionHandleMutex);
	if (!bIsRunning)
	{
		return;
	}

	// The layer state will be copied after SetFinalViewRect
	PipelinedFrameStateRHI = InFrameState;

	XrFrameBeginInfo BeginInfo;
	BeginInfo.type = XR_TYPE_FRAME_BEGIN_INFO;
	BeginInfo.next = nullptr;
	XrTime DisplayTime = InFrameState.FrameState.predictedDisplayTime;
	XrResult Result = xrBeginFrame(Session, &BeginInfo);
	if (XR_SUCCEEDED(Result))
	{
		PipelinedLayerStateRHI.ColorSwapchain = ColorSwapchain;
		PipelinedLayerStateRHI.DepthSwapchain = DepthSwapchain;
		PipelinedLayerStateRHI.MotionVectorSwapchain = MotionVectorSwapchain;
		PipelinedLayerStateRHI.MotionVectorDepthSwapchain = MotionVectorDepthSwapchain;

		// We need a new swapchain image unless we've already acquired one for rendering
		if (!bIsRendering && ColorSwapchain)
		{
			ColorSwapchain->IncrementSwapChainIndex_RHIThread();
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
			ColorSwapchain->WaitCurrentImage_RHIThread(OPENXR_SWAPCHAIN_WAIT_TIMEOUT);
#endif
			if (bDepthExtensionSupported && DepthSwapchain)
			{
				DepthSwapchain->IncrementSwapChainIndex_RHIThread();
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
				DepthSwapchain->WaitCurrentImage_RHIThread(OPENXR_SWAPCHAIN_WAIT_TIMEOUT);
#endif
			}

			if (bEnableSpaceWarp && MotionVectorSwapchain && MotionVectorDepthSwapchain)
			{
				MotionVectorSwapchain->IncrementSwapChainIndex_RHIThread();
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
				MotionVectorSwapchain->WaitCurrentImage_RHIThread(OPENXR_SWAPCHAIN_WAIT_TIMEOUT);
#endif

				MotionVectorDepthSwapchain->IncrementSwapChainIndex_RHIThread();
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
				MotionVectorDepthSwapchain->WaitCurrentImage_RHIThread(OPENXR_SWAPCHAIN_WAIT_TIMEOUT);
#endif
			}
		}

		// Update foveation
		{
			XrFoveationDynamicFB FFRDynamic = Settings->FFRDynamic? XR_FOVEATION_DYNAMIC_LEVEL_ENABLED_FB : XR_FOVEATION_DYNAMIC_DISABLED_FB;
			SetFoveation((XrFoveationLevelFB)Settings->FFRLevel, 0.0f, FFRDynamic);
		}

		bIsRendering = true;
	}
	else
	{
		static bool bLoggedBeginFrameFailure = false;
		if (!bLoggedBeginFrameFailure)
		{
			UE_LOG(LogHMD, Error, TEXT("Unexpected error on xrBeginFrame. Error code was %s."), OpenXRResultToString(Result));
			bLoggedBeginFrameFailure = true;
		}
	}
}

void FYvrXRHMD::OnFinishRendering_RHIThread()
{
	ensure(IsInRenderingThread() || IsInRHIThread());

	SCOPED_NAMED_EVENT(EndFrame, FColor::Red);

	if (!bIsRendering)
	{
		return;
	}

	// We need to ensure we release the swap chain images even if the session is not running.
	if (PipelinedLayerStateRHI.ColorSwapchain)
	{
		PipelinedLayerStateRHI.ColorSwapchain->ReleaseCurrentImage_RHIThread();

		if (bDepthExtensionSupported && PipelinedLayerStateRHI.DepthSwapchain)
		{
			PipelinedLayerStateRHI.DepthSwapchain->ReleaseCurrentImage_RHIThread();
		}

		if (bEnableSpaceWarp && PipelinedLayerStateRHI.MotionVectorSwapchain && PipelinedLayerStateRHI.MotionVectorDepthSwapchain)
		{
			PipelinedLayerStateRHI.MotionVectorSwapchain->ReleaseCurrentImage_RHIThread();
			PipelinedLayerStateRHI.MotionVectorDepthSwapchain->ReleaseCurrentImage_RHIThread();
		}
	}

	FReadScopeLock Lock(SessionHandleMutex);
	if (bIsRunning)
	{
		TArray<const XrCompositionLayerBaseHeader*> Headers;

		for (FXrCompositionLayerUnion& StereoLayer : PipelinedLayerStateRHI.NativeOverlays)
		{
			XrCompositionLayerBaseHeader* Layer = reinterpret_cast<XrCompositionLayerBaseHeader*>(&StereoLayer);

			if (Layer->type == XR_TYPE_COMPOSITION_LAYER_PROJECTION)
			{
				XrCompositionLayerProjection* Projection = (XrCompositionLayerProjection*)(Layer);

				if (IsBackgroundLayerVisible())
				{
					Projection->type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
					Projection->next = nullptr;
					Projection->layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
#if ENGINE_MAJOR_VERSION > 4
					Projection->layerFlags |= XR_COMPOSITION_LAYER_INVERTED_ALPHA_BIT_EXT;
#endif
					if (!bIsMobileHDREnabled)
					{
#if ENGINE_MAJOR_VERSION < 5
						Projection->layerFlags |= XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT;
#endif
					}
					Projection->space = PipelinedFrameStateRHI.TrackingSpace->Handle;
					Projection->viewCount = PipelinedLayerStateRHI.ProjectionLayers.Num();
					Projection->views = PipelinedLayerStateRHI.ProjectionLayers.GetData();
					Headers.Add(reinterpret_cast<const XrCompositionLayerBaseHeader*>(Projection));
				}
			}
			else if (Layer->type == XR_TYPE_COMPOSITION_LAYER_QUAD)
			{
				XrCompositionLayerQuad* Quad = (XrCompositionLayerQuad*)(Layer);
				Headers.Add(reinterpret_cast<const XrCompositionLayerBaseHeader*>(Quad));
			}
			else if (Layer->type == XR_TYPE_COMPOSITION_LAYER_CYLINDER_KHR)
			{
				XrCompositionLayerCylinderKHR* Cylinder = (XrCompositionLayerCylinderKHR*)(Layer);
				Headers.Add(reinterpret_cast<const XrCompositionLayerBaseHeader*>(Cylinder));
			}
			else if (Layer->type == XR_TYPE_COMPOSITION_LAYER_EQUIRECT_KHR)
			{
				XrCompositionLayerEquirectKHR* Equirect = (XrCompositionLayerEquirectKHR*)(Layer);
				Headers.Add(reinterpret_cast<const XrCompositionLayerBaseHeader*>(Equirect));
			}
			else if (Layer->type == XR_TYPE_COMPOSITION_LAYER_CUBE_KHR)
			{
				XrCompositionLayerCubeKHR* Cube = (XrCompositionLayerCubeKHR*)(Layer);
				Headers.Add(reinterpret_cast<const XrCompositionLayerBaseHeader*>(Cube));
			}
		}

		XrFrameEndInfo EndInfo;
		EndInfo.type = XR_TYPE_FRAME_END_INFO;
		EndInfo.next = nullptr;
		EndInfo.displayTime = PipelinedFrameStateRHI.FrameState.predictedDisplayTime;
		EndInfo.environmentBlendMode = SelectedEnvironmentBlendMode;
		EndInfo.layerCount = PipelinedFrameStateRHI.FrameState.shouldRender ? Headers.Num() : 0;
		EndInfo.layers = PipelinedFrameStateRHI.FrameState.shouldRender ? Headers.GetData() : nullptr;

		XR_ENSURE(xrEndFrame(Session, &EndInfo));
	}

	bIsRendering = false;
}

FIntPoint FYvrXRHMD::GetRenderTargetSize() const
{
	FIntPoint RenderTargetSize = GetIdealRenderTargetSize();

	const float RenderTargetScale = Settings->bPixelDensityAdaptive ? Settings->PixelDensityMax : Settings->PixelDensity;
	if (!FMath::IsNearlyEqual(RenderTargetScale, 1.0f))
	{
		RenderTargetSize = (FVector2D(RenderTargetSize) * RenderTargetScale).IntPoint();
	}

	return RenderTargetSize;
}

FIntPoint FYvrXRHMD::GetRenderViewportSize(const FIntPoint& RenderTargetSize) const
{
	FIntPoint RenderViewportSize = RenderTargetSize;

	if (Settings->bPixelDensityAdaptive)
	{
		const float RenderViewportScale = Settings->PixelDensity / Settings->PixelDensityMax;
		if (!FMath::IsNearlyEqual(RenderViewportScale, 1.0f))
		{
			RenderViewportSize = (FVector2D(RenderViewportSize) * RenderViewportScale).IntPoint();
		}
	}

	return RenderViewportSize;
}

FXRRenderBridge* FYvrXRHMD::GetActiveRenderBridge_GameThread(bool /* bUseSeparateRenderTarget */)
{
	return RenderBridge;
}

FIntPoint FYvrXRHMD::GetIdealRenderTargetSize() const
{
	const FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();

	FIntPoint Size(EForceInit::ForceInitToZero);
	for (int32 ViewIndex = 0; ViewIndex < PipelineState.ViewConfigs.Num(); ViewIndex++)
	{
		const XrViewConfigurationView& Config = PipelineState.ViewConfigs[ViewIndex];

		// If Mobile Multi-View is active the first two views will share the same position
		Size.X = bIsMobileMultiViewEnabled && ViewIndex < 2 ? FMath::Max(Size.X, (int)Config.recommendedImageRectWidth)
			: Size.X + (int)Config.recommendedImageRectWidth;
		Size.Y = FMath::Max(Size.Y, (int)Config.recommendedImageRectHeight);

		// We always prefer the nearest multiple of 4 for our buffer sizes. Make sure we round up here,
		// so we're consistent with the rest of the engine in creating our buffers.
		QuantizeSceneBufferSize(Size, Size);
	}

	return Size;
}

float FYvrXRHMD::GetPixelDenity() const
{
	return Settings->PixelDensity;
}

void FYvrXRHMD::SetPixelDensity(const float NewPixelDensity)
{
	if (Settings->bPixelDensityAdaptive)
	{
		Settings->PixelDensity = FMath::Clamp(NewPixelDensity, Settings->PixelDensityMin, Settings->PixelDensityMax);
	}
	else
	{
		Settings->PixelDensity = FMath::Clamp(NewPixelDensity, 0.5f, 2.0f);
	}
}

FIntRect FYvrXRHMD::GetFullFlatEyeRect_RenderThread(FTexture2DRHIRef EyeTexture) const
{
	FVector2D SrcNormRectMin(0.05f, 0.2f);
	FVector2D SrcNormRectMax(0.45f, 0.8f);
	if (GetDesiredNumberOfViews(bStereoEnabled) > 2)
	{
		SrcNormRectMin.X /= 2;
		SrcNormRectMax.X /= 2;
	}

	return FIntRect(EyeTexture->GetSizeX() * SrcNormRectMin.X, EyeTexture->GetSizeY() * SrcNormRectMin.Y, EyeTexture->GetSizeX() * SrcNormRectMax.X, EyeTexture->GetSizeY() * SrcNormRectMax.Y);
}

void FYvrXRHMD::CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture* SrcTexture, FIntRect SrcRect, FRHITexture* DstTexture, FIntRect DstRect, bool bClearBlack, bool bNoAlpha, ERenderTargetActions RTAction) const
{
	check(IsInRenderingThread());

	FRHITexture2D* DstTexture2D = DstTexture->GetTexture2D();
	FRHITextureCube* DstTextureCube = DstTexture->GetTextureCube();
	FRHITexture2D* SrcTexture2D = SrcTexture->GetTexture2DArray() ? SrcTexture->GetTexture2DArray() : SrcTexture->GetTexture2D();
	FRHITextureCube* SrcTextureCube = SrcTexture->GetTextureCube();
	FIntPoint DstSize;
	FIntPoint SrcSize;

	if (DstTexture2D && SrcTexture2D)
	{
		DstSize = FIntPoint(DstTexture2D->GetSizeX(), DstTexture2D->GetSizeY());
		SrcSize = FIntPoint(SrcTexture2D->GetSizeX(), SrcTexture2D->GetSizeY());
	}
	else if (DstTextureCube && SrcTextureCube)
	{
		DstSize = FIntPoint(DstTextureCube->GetSize(), DstTextureCube->GetSize());
		SrcSize = FIntPoint(SrcTextureCube->GetSize(), SrcTextureCube->GetSize());

	}
	else
	{
		return;
	}

	if (DstRect.IsEmpty())
	{
		DstRect = FIntRect(FIntPoint::ZeroValue, DstSize);
	}

	if (SrcRect.IsEmpty())
	{
		SrcRect = FIntRect(FIntPoint::ZeroValue, SrcSize);
	}

	const uint32 ViewportWidth = DstRect.Width();
	const uint32 ViewportHeight = DstRect.Height();
	const FIntPoint TargetSize(ViewportWidth, ViewportHeight);
	float U = SrcRect.Min.X / (float)SrcSize.X;
	float V = SrcRect.Min.Y / (float)SrcSize.Y;
	float USize = SrcRect.Width() / (float)SrcSize.X;
	float VSize = SrcRect.Height() / (float)SrcSize.Y;

	RHICmdList.Transition(FRHITransitionInfo(SrcTexture, ERHIAccess::Unknown, ERHIAccess::SRVMask));

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	GraphicsPSOInit.BlendState = bNoAlpha ? TStaticBlendState<>::GetRHI() : TStaticBlendState<CW_RGBA, BO_Add, BF_SourceAlpha, BF_InverseSourceAlpha, BO_Add, BF_One, BF_InverseSourceAlpha>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	const auto FeatureLevel = GMaxRHIFeatureLevel;
	auto ShaderMap = GetGlobalShaderMap(FeatureLevel);
	TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();

	if (DstTexture2D)
	{
		FRHIRenderPassInfo RenderPassInfo(DstTexture, RTAction);
		RHICmdList.BeginRenderPass(RenderPassInfo, TEXT("CopyTexture"));
		{
			if (bClearBlack)
			{
				RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0, DstRect.Max.X, DstRect.Max.Y, 1.0f);
				DrawClearQuad(RHICmdList, FLinearColor::Black);
			}

			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0, DstRect.Max.X, DstRect.Max.Y, 1.0f);

			TShaderMapRef<FScreenPS> PixelShader(ShaderMap);

			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

#if ENGINE_MAJOR_VERSION > 4
			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
#else
			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
#endif

			FRHISamplerState* SamplerState = DstRect.Size() == SrcRect.Size() ? TStaticSamplerState<SF_Point>::GetRHI() : TStaticSamplerState<SF_Bilinear>::GetRHI();
			PixelShader->SetParameters(RHICmdList, SamplerState, SrcTexture);

			RendererModule->DrawRectangle(
				RHICmdList,
				0, 0,
				ViewportWidth, ViewportHeight,
				U, V,
				USize, VSize,
				TargetSize,
				FIntPoint(1, 1),
				VertexShader,
				EDRF_Default);
		}
		RHICmdList.EndRenderPass();
	}
	else
	{
		for (int FaceIndex = 0; FaceIndex < 6; FaceIndex++)
		{
			FRHIRenderPassInfo RPInfo(DstTexture, ERenderTargetActions::Load_Store);

			RPInfo.ColorRenderTargets[0].ArraySlice = FaceIndex;

			RHICmdList.BeginRenderPass(RPInfo, TEXT("CopyTextureFace"));
			{
				if (bClearBlack)
				{
					RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0, DstRect.Max.X, DstRect.Max.Y, 1.0f);
					DrawClearQuad(RHICmdList, FLinearColor::Black);
				}

				RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

				RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0, DstRect.Max.X, DstRect.Max.Y, 1.0f);

				TShaderMapRef<FYvrCubemapPS> PixelShader(ShaderMap);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

				FRHISamplerState* SamplerState = DstRect.Size() == SrcRect.Size() ? TStaticSamplerState<SF_Point>::GetRHI() : TStaticSamplerState<SF_Bilinear>::GetRHI();

				PixelShader->SetParameters(RHICmdList, SamplerState, SrcTexture, FaceIndex);

				RendererModule->DrawRectangle(
					RHICmdList,
					0, 0, ViewportWidth, ViewportHeight,
					U, V, USize, VSize,
					TargetSize,
					FIntPoint(1, 1),
					VertexShader,
					EDRF_Default);
			}
			RHICmdList.EndRenderPass();
		}
	}
}

void FYvrXRHMD::CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture* SrcTexture, FIntRect SrcRect, const FXRSwapChainPtr& DstSwapChain, FIntRect DstRect, bool bClearBlack, bool bNoAlpha) const
{
	RHICmdList.EnqueueLambda([DstSwapChain](FRHICommandListImmediate& InRHICmdList)
	{
		DstSwapChain->IncrementSwapChainIndex_RHIThread();
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
		DstSwapChain->WaitCurrentImage_RHIThread(OPENXR_SWAPCHAIN_WAIT_TIMEOUT);
#endif
	});

	// Now that we've enqueued the swapchain wait we can add the commands to do the actual texture copy
	FRHITexture* DstTexture = DstSwapChain->GetTexture();
	CopyTexture_RenderThread(RHICmdList, SrcTexture, SrcRect, DstTexture, DstRect, bClearBlack, bNoAlpha, ERenderTargetActions::Clear_Store);

	// Enqueue a command to release the image after the copy is done
	RHICmdList.EnqueueLambda([DstSwapChain](FRHICommandListImmediate& InRHICmdList)
	{
		DstSwapChain->ReleaseCurrentImage_RHIThread();
	});
}

void FYvrXRHMD::SetFoveation(XrFoveationLevelFB Level, float VerticalOffset, XrFoveationDynamicFB Dynamic)
{
	PFN_xrCreateFoveationProfileFB CreateFoveationProfile;
	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrCreateFoveationProfileFB", (PFN_xrVoidFunction*)(&CreateFoveationProfile)));
	PFN_xrDestroyFoveationProfileFB DestroyFoveationProfile;
	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrDestroyFoveationProfileFB", (PFN_xrVoidFunction*)(&DestroyFoveationProfile)));
	PFN_xrUpdateSwapchainFB UpdateSwapchain;
	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrUpdateSwapchainFB", (PFN_xrVoidFunction*)(&UpdateSwapchain)));

	for (int ViewIndex = 0; ViewIndex < PipelinedLayerStateRHI.ProjectionLayers.Num(); ViewIndex++) {
		XrFoveationLevelProfileCreateInfoFB LevelProfileCreateInfo;
		memset(&LevelProfileCreateInfo, 0, sizeof(LevelProfileCreateInfo));
		LevelProfileCreateInfo.type = XR_TYPE_FOVEATION_LEVEL_PROFILE_CREATE_INFO_FB;
		LevelProfileCreateInfo.level = Level;
		LevelProfileCreateInfo.verticalOffset = VerticalOffset;
		LevelProfileCreateInfo.dynamic = Dynamic;

		XrFoveationProfileCreateInfoFB ProfileCreateInfo;
		memset(&ProfileCreateInfo, 0, sizeof(ProfileCreateInfo));
		ProfileCreateInfo.type = XR_TYPE_FOVEATION_PROFILE_CREATE_INFO_FB;
		ProfileCreateInfo.next = &LevelProfileCreateInfo;

		XrFoveationProfileFB FoveationProfile;

		CreateFoveationProfile(Session, &ProfileCreateInfo, &FoveationProfile);

		XrSwapchainStateFoveationFB FoveationUpdateState;
		memset(&FoveationUpdateState, 0, sizeof(FoveationUpdateState));
		FoveationUpdateState.type = XR_TYPE_SWAPCHAIN_STATE_FOVEATION_FB;
		FoveationUpdateState.profile = FoveationProfile;

		UpdateSwapchain(PipelinedLayerStateRHI.ColorImages[ViewIndex].swapchain, (XrSwapchainStateBaseHeaderFB*)(&FoveationUpdateState));

		DestroyFoveationProfile(FoveationProfile);
	}
}

void FYvrXRHMD::UpdateStereoRenderingParams()
{
	FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();

	if (Settings->bPixelDensityAdaptive)
	{
		PFN_xrGetRecommendedResolution GetRecommendedResolution;
		XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrGetRecommendedResolution", (PFN_xrVoidFunction*)(&GetRecommendedResolution)));

		XrExtent2Di RecommendedResolution = { 0, 0 };
		XrAdapterResolutionInfoYVR ResolutionInfo;
		ResolutionInfo.type = XR_TYPE_ADAPTER_RESOLUTIONINFO_YVR;
		ResolutionInfo.resolutionPolicy = (XrAdapterResolutionPolicyYVR)Settings->DynamicAResolutionLevel;
		GetRecommendedResolution(Session, &ResolutionInfo, &RecommendedResolution);

		int RecommendedWidth = bIsMobileMultiViewEnabled ? RecommendedResolution.width : RecommendedResolution.width * 2;
		int RecommendedHeight = RecommendedResolution.height;
		float NewPixelDensity = (float)RecommendedHeight / (float)GetIdealRenderTargetSize().Y;
		SetPixelDensity(NewPixelDensity);

		Settings->RenderTargetSize = GetRenderTargetSize();
		Settings->RenderTargetViewportSize = GetRenderViewportSize(Settings->RenderTargetSize);
	}
	else
	{
		// Update render target size
		static const auto PixelDensityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("vr.PixelDensity"));
		float PixelDensity = (PixelDensityCVar ? PixelDensityCVar->GetFloat() : 1.0f);
		SetPixelDensity(PixelDensity);

		Settings->RenderTargetSize = GetRenderTargetSize();
		Settings->RenderTargetViewportSize = Settings->RenderTargetSize;

	}

	if (bEnableSpaceWarp)
	{
		PipelineState.bIsSpaceWarpFrame = Settings->bUseSpaceWarp;
		CVarYvrEnableSpaceWarpInternal->Set(Settings->bUseSpaceWarp);
		// Support oculus
		CVarOculusEnableSpaceWarpInternal->Set(Settings->bUseSpaceWarp);
	}
}

void FYvrXRHMD::CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* SrcTexture, FIntRect SrcRect, FRHITexture2D* DstTexture, FIntRect DstRect, bool bClearBlack, bool bNoAlpha) const
{
	// FIXME: This should probably use the Load_Store action since the spectator controller does multiple overlaying copies.
	CopyTexture_RenderThread(RHICmdList, SrcTexture, SrcRect, DstTexture, DstRect, bClearBlack, bNoAlpha, ERenderTargetActions::DontLoad_Store);
}

#if ENGINE_MAJOR_VERSION > 4
bool FYvrXRHMD::GetRelativeEyePose(int32 InDeviceId, int32 ViewIndex, FQuat& OutOrientation, FVector& OutPosition)
#else
bool FYvrXRHMD::GetRelativeEyePose(int32 InDeviceId, EStereoscopicPass StereoPass, FQuat& OutOrientation, FVector& OutPosition)
#endif
{
	if (InDeviceId != IXRTrackingSystem::HMDDeviceId)
	{
		return false;
	}

#if ENGINE_MAJOR_VERSION > 4
#else
	uint32 ViewIndex = GetViewIndexForPass(StereoPass);
#endif

	const FPipelinedFrameState& FrameState = GetPipelinedFrameStateForThread();

	if (FrameState.ViewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT &&
		FrameState.ViewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT &&
		FrameState.Views.IsValidIndex(ViewIndex))
	{
		OutOrientation = ToFQuat(FrameState.Views[ViewIndex].pose.orientation);
		OutPosition = ToFVector(FrameState.Views[ViewIndex].pose.position, GetWorldToMetersScale());
		return true;
	}

	return false;
}

#if ENGINE_MAJOR_VERSION > 4
bool FYvrXRHMD::GetPoseForTime(int32 DeviceId, FTimespan Timespan, bool& OutTimeWasUsed, FQuat& Orientation, FVector& Position, bool& bProvidedLinearVelocity, FVector& LinearVelocity, bool& bProvidedAngularVelocity, FVector& AngularVelocityRadPerSec, bool& bProvidedLinearAcceleration, FVector& LinearAcceleration, float InWorldToMetersScale)
{
	const FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();

	FReadScopeLock DeviceLock(DeviceMutex);
	if (!DeviceSpaces.IsValidIndex(DeviceId))
	{
		return false;
	}

	XrTime TargetTime = ToXrTime(Timespan);

	// If TargetTime is zero just get the latest data (rather than the oldest).
	if (TargetTime == 0)
	{
		OutTimeWasUsed = false;
		TargetTime = GetDisplayTime();
	}
	else
	{
		OutTimeWasUsed = true;
	}

	const FDeviceSpace& DeviceSpace = DeviceSpaces[DeviceId];

	XrSpaceVelocity DeviceVelocity{ XR_TYPE_SPACE_VELOCITY };
	XrSpaceLocation DeviceLocation{ XR_TYPE_SPACE_LOCATION, &DeviceVelocity };

	XR_ENSURE(xrLocateSpace(DeviceSpace.Space, PipelineState.TrackingSpace->Handle, TargetTime, &DeviceLocation));

	if (DeviceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT &&
		DeviceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT)
	{
		Orientation = ToFQuat(DeviceLocation.pose.orientation);
		Position = ToFVector(DeviceLocation.pose.position, InWorldToMetersScale);

		if (DeviceVelocity.velocityFlags & XR_SPACE_VELOCITY_LINEAR_VALID_BIT)
		{
			bProvidedLinearVelocity = true;
			LinearVelocity = ToFVector(DeviceVelocity.linearVelocity, InWorldToMetersScale);
		}
		if (DeviceVelocity.velocityFlags & XR_SPACE_VELOCITY_ANGULAR_VALID_BIT)
		{
			bProvidedAngularVelocity = true;
			AngularVelocityRadPerSec = -ToFVector(DeviceVelocity.angularVelocity);
		}

		return true;
	}

	return false;
}
#else
bool FYvrXRHMD::GetPoseForTime(int32 DeviceId, FTimespan Timespan, FQuat& Orientation, FVector& Position, bool& bProvidedLinearVelocity, FVector& LinearVelocity, bool& bProvidedAngularVelocity, FVector& AngularVelocityRadPerSec)
{
	FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();

	FReadScopeLock DeviceLock(DeviceMutex);
	if (!DeviceSpaces.IsValidIndex(DeviceId))
	{
		return false;
	}

	XrTime TargetTime = ToXrTime(Timespan);

	const FDeviceSpace& DeviceSpace = DeviceSpaces[DeviceId];

	XrSpaceVelocity DeviceVelocity{ XR_TYPE_SPACE_VELOCITY };
	XrSpaceLocation DeviceLocation{ XR_TYPE_SPACE_LOCATION, &DeviceVelocity };

	XR_ENSURE(xrLocateSpace(DeviceSpace.Space, PipelineState.TrackingSpace->Handle, TargetTime, &DeviceLocation));

	if (DeviceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT &&
		DeviceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT)
	{
		Orientation = ToFQuat(DeviceLocation.pose.orientation);
		Position = ToFVector(DeviceLocation.pose.position, GetWorldToMetersScale());

		if (DeviceVelocity.velocityFlags & XR_SPACE_VELOCITY_LINEAR_VALID_BIT)
		{
			bProvidedLinearVelocity = true;
			LinearVelocity = ToFVector(DeviceVelocity.linearVelocity, GetWorldToMetersScale());
		}
		if (DeviceVelocity.velocityFlags & XR_SPACE_VELOCITY_ANGULAR_VALID_BIT)
		{
			bProvidedAngularVelocity = true;
			AngularVelocityRadPerSec = ToFVector(DeviceVelocity.angularVelocity);
		}

		return true;
	}

	return false;
}
#endif


#if ENGINE_MAJOR_VERSION > 4
void FYvrXRHMD::AdjustViewRect(int32 ViewIndex, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
#else
void FYvrXRHMD::AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
#endif
{
#if ENGINE_MAJOR_VERSION > 4
#else
	int32 ViewIndex = GetViewIndexForPass(StereoPass);
#endif

	const FPipelinedFrameState& PipelineState = GetPipelinedFrameStateForThread();
	const XrViewConfigurationView& Config = PipelineState.ViewConfigs[ViewIndex];
	FIntPoint ViewRectMin(EForceInit::ForceInitToZero);
	int32 ImageRectWidth = 0;
	int32 ImageRectHeight = 0;

	int32 ViewWidth = bIsMobileMultiViewEnabled ? Settings->RenderTargetViewportSize.X : Settings->RenderTargetViewportSize.X / 2;
	int32 ViewHeight = Settings->RenderTargetViewportSize.Y;

	// If Mobile Multi-View is active the first two views will share the same position
	// Thus the start index should be the second view if enabled
	for (int32 i = bIsMobileMultiViewEnabled ? 1 : 0; i < ViewIndex; ++i)
	{
		ViewRectMin.X += Settings->RenderTargetSize.X / 2;
	}
	QuantizeSceneBufferSize(ViewRectMin, ViewRectMin);

	X = ViewRectMin.X;
	Y = ViewRectMin.Y;
	SizeX = ViewWidth;
	SizeY = ViewHeight;
}

#if ENGINE_MAJOR_VERSION > 4
void FYvrXRHMD::SetFinalViewRect(FRHICommandListImmediate& RHICmdList, const int32 ViewIndex, const FIntRect& FinalViewRect)
#elif ENGINE_MINOR_VERSION > 26
void FYvrXRHMD::SetFinalViewRect(FRHICommandListImmediate& RHICmdList, const enum EStereoscopicPass StereoPass, const FIntRect& FinalViewRect)
#else
void FYvrXRHMD::SetFinalViewRect(const enum EStereoscopicPass StereoPass, const FIntRect& FinalViewRect)
#endif
{

#if ENGINE_MAJOR_VERSION > 4
	if (ViewIndex == INDEX_NONE || !PipelinedLayerStateRendering.ColorImages.IsValidIndex(ViewIndex))
	{
		return;
	}
#else
	if (StereoPass == eSSP_FULL)
	{
		return;
	}

	int32 ViewIndex = GetViewIndexForPass(StereoPass);
#endif
	float NearZ = GNearClippingPlane / GetWorldToMetersScale();

	XrSwapchainSubImage& ColorImage = PipelinedLayerStateRendering.ColorImages[ViewIndex];
	ColorImage.swapchain = PipelinedLayerStateRendering.ColorSwapchain.IsValid() ? static_cast<FYvrXRSwapchain*>(PipelinedLayerStateRendering.ColorSwapchain.Get())->GetHandle() : XR_NULL_HANDLE;
	ColorImage.imageArrayIndex = bIsMobileMultiViewEnabled && ViewIndex < 2 ? ViewIndex : 0;
	ColorImage.imageRect = {
		{ FinalViewRect.Min.X, FinalViewRect.Min.Y },
		{ FinalViewRect.Width(), FinalViewRect.Height() }
	};

	XrSwapchainSubImage& DepthImage = PipelinedLayerStateRendering.DepthImages[ViewIndex];
	if (bDepthExtensionSupported)
	{
		DepthImage.swapchain = PipelinedLayerStateRendering.DepthSwapchain.IsValid() ? static_cast<FYvrXRSwapchain*>(PipelinedLayerStateRendering.DepthSwapchain.Get())->GetHandle() : XR_NULL_HANDLE;
		DepthImage.imageArrayIndex = bIsMobileMultiViewEnabled && ViewIndex < 2 ? ViewIndex : 0;
		DepthImage.imageRect = ColorImage.imageRect;
	}

	XrSwapchainSubImage& MotionVectorImage = PipelinedLayerStateRendering.MotionVectorImages[ViewIndex];
	XrSwapchainSubImage& MotionVectorDepthImage = PipelinedLayerStateRendering.MotionVectorDepthImages[ViewIndex];
	if (bEnableSpaceWarp)
	{
		MotionVectorImage.swapchain = PipelinedLayerStateRendering.MotionVectorSwapchain.IsValid() ? static_cast<FYvrXRSwapchain*>(PipelinedLayerStateRendering.MotionVectorSwapchain.Get())->GetHandle() : XR_NULL_HANDLE;
		MotionVectorImage.imageArrayIndex = bIsMobileMultiViewEnabled && ViewIndex < 2 ? ViewIndex : 0;
		MotionVectorImage.imageRect.offset.x = 0;
		MotionVectorImage.imageRect.offset.y = 0;
		MotionVectorImage.imageRect.extent.width = RecommendedMotionVectorImageRectWidth;
		MotionVectorImage.imageRect.extent.height = RecommendedMotionVectorImageRectHeight;

		MotionVectorDepthImage.swapchain = PipelinedLayerStateRendering.MotionVectorDepthSwapchain.IsValid() ? static_cast<FYvrXRSwapchain*>(PipelinedLayerStateRendering.MotionVectorDepthSwapchain.Get())->GetHandle() : XR_NULL_HANDLE;
		MotionVectorDepthImage.imageArrayIndex = bIsMobileMultiViewEnabled && ViewIndex < 2 ? ViewIndex : 0;
		MotionVectorDepthImage.imageRect.offset.x = 0;
		MotionVectorDepthImage.imageRect.offset.y = 0;
		MotionVectorDepthImage.imageRect.extent.width = RecommendedMotionVectorImageRectWidth;
		MotionVectorDepthImage.imageRect.extent.height = RecommendedMotionVectorImageRectHeight;
	}

	if (!PipelinedFrameStateRendering.PluginViews.IsValidIndex(ViewIndex))
	{
		// This plugin is no longer providing this view.
		return;
	}

	if (PipelinedFrameStateRendering.PluginViews[ViewIndex])
	{
		// Defer to the plugin to handle submission
		return;
	}

	XrCompositionLayerProjectionView& Projection = PipelinedLayerStateRendering.ProjectionLayers[ViewIndex];
	Projection.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
	Projection.next = nullptr;
	Projection.subImage = ColorImage;

	if (bDepthExtensionSupported && PipelinedLayerStateRendering.DepthSwapchain.IsValid())
	{
		XrCompositionLayerDepthInfoKHR& DepthLayer = PipelinedLayerStateRendering.DepthLayers[ViewIndex];
		DepthLayer.type = XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR;
		DepthLayer.next = nullptr;
		DepthLayer.subImage = DepthImage;
		DepthLayer.minDepth = 0.0f;
		DepthLayer.maxDepth = 1.0f;
		DepthLayer.nearZ = FLT_MAX;
		DepthLayer.farZ = NearZ;
		DepthLayer.next = Projection.next;
		Projection.next = &DepthLayer;
	}

	if (bEnableSpaceWarp && PipelinedLayerStateRendering.MotionVectorSwapchain.IsValid() && PipelinedLayerStateRendering.MotionVectorDepthSwapchain.IsValid())
	{
		if (PipelinedFrameStateRendering.bIsSpaceWarpFrame)
		{
			XrCompositionLayerSpaceWarpInfoFB& MotionVectorLayer = PipelinedLayerStateRendering.MotionVectorLayers[ViewIndex];
			MotionVectorLayer.type = XR_TYPE_COMPOSITION_LAYER_SPACE_WARP_INFO_FB;
			MotionVectorLayer.next = nullptr;
			MotionVectorLayer.motionVectorSubImage = MotionVectorImage;
			MotionVectorLayer.depthSubImage = MotionVectorDepthImage;
			MotionVectorLayer.minDepth = 0.0f;
			MotionVectorLayer.maxDepth = 1.0f;
			MotionVectorLayer.nearZ = INFINITY;
			MotionVectorLayer.farZ = NearZ;
			MotionVectorLayer.next = Projection.next;
			Projection.next = &MotionVectorLayer;
		}
	}

	if ((uint8)Settings->SharpenType)
	{
		XrCompositionLayerSettingsFB& LayerSettings = PipelinedLayerStateRendering.LayerSettings[ViewIndex];
		LayerSettings.type = XR_TYPE_COMPOSITION_LAYER_SETTINGS_FB;
		LayerSettings.next = NULL;
		LayerSettings.layerFlags = (XrCompositionLayerSettingsFlagsFB)Settings->SharpenType;
		LayerSettings.next = Projection.next;
		Projection.next = &LayerSettings;
	}

	ExecuteOnRHIThread_DoNotWait([this, LayerState = PipelinedLayerStateRendering]()
	{
		PipelinedLayerStateRHI = LayerState;
	});
}

#if ENGINE_MAJOR_VERSION > 4
FMatrix FYvrXRHMD::GetStereoProjectionMatrix(const int32 ViewIndex) const
#else
FMatrix FYvrXRHMD::GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPass) const
#endif
{
#if ENGINE_MAJOR_VERSION > 4
#else
	int32 ViewIndex = GetViewIndexForPass(StereoPass);
#endif

	const FPipelinedFrameState& FrameState = GetPipelinedFrameStateForThread();
	XrFovf Fov = {};
	if (ViewIndex == -1)
	{
		// The monoscopic projection matrix uses the combined field-of-view of both eyes
		for (int32 Index = 0; Index < FrameState.Views.Num(); Index++)
		{
			const XrFovf& ViewFov = FrameState.Views[Index].fov;
			Fov.angleUp = FMath::Max(Fov.angleUp, ViewFov.angleUp);
			Fov.angleDown = FMath::Min(Fov.angleDown, ViewFov.angleDown);
			Fov.angleLeft = FMath::Min(Fov.angleLeft, ViewFov.angleLeft);
			Fov.angleRight = FMath::Max(Fov.angleRight, ViewFov.angleRight);
		}
	}
	else
	{
		Fov = (ViewIndex < FrameState.Views.Num()) ? FrameState.Views[ViewIndex].fov
			: XrFovf{ -PI / 4.0f, PI / 4.0f, PI / 4.0f, -PI / 4.0f };
	}

	Fov.angleUp = tan(Fov.angleUp);
	Fov.angleDown = tan(Fov.angleDown);
	Fov.angleLeft = tan(Fov.angleLeft);
	Fov.angleRight = tan(Fov.angleRight);

	float ZNear = GNearClippingPlane;
	float SumRL = (Fov.angleRight + Fov.angleLeft);
	float SumTB = (Fov.angleUp + Fov.angleDown);
	float InvRL = (1.0f / (Fov.angleRight - Fov.angleLeft));
	float InvTB = (1.0f / (Fov.angleUp - Fov.angleDown));

	FMatrix Mat = FMatrix(
		FPlane((2.0f * InvRL), 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, (2.0f * InvTB), 0.0f, 0.0f),
		FPlane((SumRL * -InvRL), (SumTB * -InvTB), 0.0f, 1.0f),
		FPlane(0.0f, 0.0f, ZNear, 0.0f)
	);

	return Mat;
}

#if ENGINE_MAJOR_VERSION > 4
void FYvrXRHMD::GetEyeRenderParams_RenderThread(const FHeadMountedDisplayPassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const
#else
void FYvrXRHMD::GetEyeRenderParams_RenderThread(const struct FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const
#endif
{
	EyeToSrcUVOffsetValue = FVector2D::ZeroVector;
	EyeToSrcUVScaleValue = FVector2D(1.0f, 1.0f);
}


//---------------------------------------------------
// OpenXR Action Space Implementation
//---------------------------------------------------

FYvrXRHMD::FDeviceSpace::FDeviceSpace(XrAction InAction, XrPath InPath)
	: Action(InAction)
	, Space(XR_NULL_HANDLE)
	, Path(InPath)
{
}

FYvrXRHMD::FDeviceSpace::~FDeviceSpace()
{
	DestroySpace();
}

bool FYvrXRHMD::FDeviceSpace::CreateSpace(XrSession InSession)
{
	if (Action == XR_NULL_HANDLE || Space != XR_NULL_HANDLE)
	{
		return false;
	}

	XrActionSpaceCreateInfo ActionSpaceInfo;
	ActionSpaceInfo.type = XR_TYPE_ACTION_SPACE_CREATE_INFO;
	ActionSpaceInfo.next = nullptr;
	ActionSpaceInfo.subactionPath = XR_NULL_PATH;
	ActionSpaceInfo.poseInActionSpace = ToXrPose(FTransform::Identity);
	ActionSpaceInfo.action = Action;
	return XR_ENSURE(xrCreateActionSpace(InSession, &ActionSpaceInfo, &Space));
}

void FYvrXRHMD::FDeviceSpace::DestroySpace()
{
	if (Space)
	{
		XR_ENSURE(xrDestroySpace(Space));
	}
	Space = XR_NULL_HANDLE;
}

//---------------------------------------------------
// OpenXR Tracking Space Implementation
//---------------------------------------------------


FYvrXRHMD::FTrackingSpace::FTrackingSpace(XrReferenceSpaceType InType)
	: FTrackingSpace(InType, ToXrPose(FTransform::Identity))
{
}

FYvrXRHMD::FTrackingSpace::FTrackingSpace(XrReferenceSpaceType InType, XrPosef InBasePose)
	: Type(InType)
	, Handle(XR_NULL_HANDLE)
	, BasePose(InBasePose)
{
}

FYvrXRHMD::FTrackingSpace::~FTrackingSpace()
{
	DestroySpace();
}

bool FYvrXRHMD::FTrackingSpace::CreateSpace(XrSession InSession)
{
	DestroySpace();

	XrReferenceSpaceCreateInfo SpaceInfo;
	SpaceInfo.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
	SpaceInfo.next = nullptr;
	SpaceInfo.referenceSpaceType = Type;
	SpaceInfo.poseInReferenceSpace = BasePose;
	return XR_ENSURE(xrCreateReferenceSpace(InSession, &SpaceInfo, &Handle));
}

void FYvrXRHMD::FTrackingSpace::DestroySpace()
{
	if (Handle)
	{
		XR_ENSURE(xrDestroySpace(Handle));
	}
	Handle = XR_NULL_HANDLE;
}

#undef LOCTEXT_NAMESPACE