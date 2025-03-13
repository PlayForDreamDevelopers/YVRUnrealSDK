// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "YvrXRFunctionLibrary.h"
#include "YvrXRHMDSettings.generated.h"

/**
* Implements the settings for the YvrVR plugin.
*/
UCLASS(config = Engine, defaultconfig)
class YVRXRHMD_API UYvrXRHMDSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Whether the Splash screen is enabled. */
	//UPROPERTY(config, EditAnywhere, Category = SplashScreen)
	//	bool bAutoEnabled;

	/** An array of splash screen descriptors listing textures to show and their positions. */
	//UPROPERTY(config, EditAnywhere, Category = SplashScreen)
	//	TArray<FYvrSplashDesc> SplashDescs;

	/** If enabled, the target HMD will perform a color space transformation */
	//UPROPERTY(config, EditAnywhere, Category = General)
	//	bool bEnableSpecificColorGamut;

	/** The target color space */
	//UPROPERTY(config, EditAnywhere, Category = General)
		//EYvrColorSpace	ColorSpace;

	/** Default CPU level controlling CPU frequency on the mobile device */
	UPROPERTY(config, EditAnywhere, Category = General)
		EYvrPerformanceLevel CPULevel;

	/** Default GPU level controlling GPU frequency on the mobile device */
	UPROPERTY(config, EditAnywhere, Category = General)
		EYvrPerformanceLevel GPULevel;

	UPROPERTY(Config, EditAnywhere, Category = Render)
		bool bUseHWsRGBEncoding;

	/** Default Fixed Foveated Rendering level for Yvr device*/
	UPROPERTY(config, EditAnywhere, Category = "Render|Fixed Foveated Rendering")
		EYvrFixedFoveatedRenderingLevel FFRLevel;

	UPROPERTY(config, EditAnywhere, Category = "Render|Fixed Foveated Rendering")
		bool FFRDynamic;

	UPROPERTY(Config, EditAnywhere, Category = "Render|Space Warp")
		bool bUseSpaceWarp;

	UPROPERTY(Config, EditAnywhere, Category = "Render|Sharpen")
		EYvrLayerSharpenType SharpenType;

	UPROPERTY(Config, EditAnywhere, Category = "Render|Sharpen")
		bool bApplySharpenTypeToAllLayers;

	UPROPERTY(Config, EditAnywhere, Category = "Render|Dynamic Resolution")
		bool bPixelDensityAdaptive;

	UPROPERTY(Config, EditAnywhere, Category = "Render|Dynamic Resolution")
		EYvrDynamicResolutionSetting DynamicAResolutionLevel;

	/** Minimum allowed pixel density. */
	UPROPERTY(config, EditAnywhere, Category = "Render|Dynamic Resolution")
		float PixelDensityMin;

	/** Maximum allowed pixel density. */
	UPROPERTY(config, EditAnywhere, Category = "Render|Dynamic Resolution")
		float PixelDensityMax;

	UPROPERTY(config, EditAnywhere, Category = General, meta = (DisplayName = "OS Splash Screen", FilePathFilter = "png"))
		FFilePath OSSplashScreen;

	UPROPERTY(config)
		bool bUsingOSSplash;

	/** Whether controllers and/or hands can be used with the app */
	UPROPERTY(Config, EditAnywhere, Category = Controller)
		EYvrXRHandTrackingSupport HandTrackingSupport;

	UPROPERTY(Config, EditAnywhere, Category = "MR")
		bool bAnchorSupportEnabled;

	UPROPERTY(Config, EditAnywhere, Category = "MR")
		bool bSceneSupportEnabled;

	//Base Settings
	FIntPoint RenderTargetSize;
	FIntPoint RenderTargetViewportSize;
	float PixelDensity;
	float DisplayRefreshRate;
	FVector BasePosition;
	FQuat BaseOrientation;
	bool bApplyColorScaleAndOffsetToAllLayers;
	FLinearColor ColorScale;
	FLinearColor ColorOffset;

	virtual void PostInitProperties() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void HandlesRGBHWSupport();
private:
	void ResetsRGBConfig();
#endif
};
