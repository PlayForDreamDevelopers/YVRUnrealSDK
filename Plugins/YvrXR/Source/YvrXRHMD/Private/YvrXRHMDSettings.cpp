#include "YvrXRHMDSettings.h"
#include "Engine/RendererSettings.h"

UYvrXRHMDSettings::UYvrXRHMDSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, FFRLevel(EYvrFixedFoveatedRenderingLevel::FFR_OFF)
	, CPULevel(EYvrPerformanceLevel::LEVEL_BOOST)
	, GPULevel(EYvrPerformanceLevel::LEVEL_BOOST)
	, RenderTargetSize(FIntPoint(0, 0))
	, RenderTargetViewportSize(FIntPoint(0, 0))
	, PixelDensity(1.0f)
	, DisplayRefreshRate(90.0f)
	, BasePosition(FVector::ZeroVector)
	, BaseOrientation(FQuat::Identity)
	, bApplyColorScaleAndOffsetToAllLayers(false)
	, ColorScale(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
	, ColorOffset(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
	, bUseHWsRGBEncoding(false)
	, bUseSpaceWarp(false)
	, HandTrackingSupport(EYvrXRHandTrackingSupport::ControllersOnly)
	, OSSplashScreen(FFilePath())
	, bUsingOSSplash(false)
	, FFRDynamic(false)
	, SharpenType(EYvrLayerSharpenType::NONE)
	, bApplySharpenTypeToAllLayers(false)
	, bPixelDensityAdaptive(false)
	, DynamicAResolutionLevel(EYvrDynamicResolutionSetting::BALANCED)
	, PixelDensityMin(0.8f)
	, PixelDensityMax(1.2f)
	, bAnchorSupportEnabled(false)
	, bSceneSupportEnabled(false)
{
#if WITH_EDITOR
	ResetsRGBConfig();
#endif
}

void UYvrXRHMDSettings::PostInitProperties()
{
	Super::PostInitProperties();
	bUseHWsRGBEncoding = true;
}

#if WITH_EDITOR
void UYvrXRHMDSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	Super::UpdateDefaultConfigFile();
	HandlesRGBHWSupport();

	bUsingOSSplash = FPaths::FileExists(OSSplashScreen.FilePath) ? true : false;
	FString Dst = FPaths::ProjectDir() / TEXT("vr_splash.png");

	if (bUsingOSSplash)
	{
		IFileManager::Get().Copy(*Dst, *OSSplashScreen.FilePath, true);
	}
	else
	{
		IFileManager::Get().Delete(*Dst, true);
	}
}

void UYvrXRHMDSettings::HandlesRGBHWSupport()
{
	URendererSettings* const Settings = GetMutableDefault<URendererSettings>();
	static auto* MobileUseHWsRGBEncodingCVAR = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Mobile.UseHWsRGBEncoding"));

	if (bUseHWsRGBEncoding != Settings->bMobileUseHWsRGBEncoding)
	{
		Settings->bMobileUseHWsRGBEncoding = bUseHWsRGBEncoding;
		Settings->UpdateSinglePropertyInConfigFile(Settings->GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(URendererSettings, bMobileUseHWsRGBEncoding)), GetDefaultConfigFilename());
	}

	if (MobileUseHWsRGBEncodingCVAR && MobileUseHWsRGBEncodingCVAR->GetInt() != (int)bUseHWsRGBEncoding)
	{
		MobileUseHWsRGBEncodingCVAR->Set((int)bUseHWsRGBEncoding);
	}
}

void UYvrXRHMDSettings::ResetsRGBConfig()
{
	// To Prevent sRGB from being turned off by AndroidSetting
	FEditorDelegates::RefreshAllBrowsers.AddUObject(this, &UYvrXRHMDSettings::HandlesRGBHWSupport);
}

#endif