// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeadMountedDisplayBase.h"

#include "YvrXRFunctionLibrary.generated.h"

class FYvrXRHMD;

UENUM(BlueprintType)
enum class EYvrFixedFoveatedRenderingLevel : uint8
{
	FFR_OFF = 0,
	FFR_LOW = 1,
	FFR_MEDIUM = 2,
	FFR_HIGH = 3
};

UENUM(BlueprintType)
enum class EYvrPerformanceLevel : uint8
{
	LEVEL_POWER_SAVINGS = 0,
	LEVEL_SUSTAINED_LOW = 25,
	LEVEL_SUSTAINED_HIGH = 50,
	LEVEL_BOOST = 75,
	LEVEL_MAX = 100
};

UENUM()
enum class EYvrXRHandTrackingSupport : uint8
{
	ControllersOnly,
	ControllersAndHands,
	HandsOnly
};


UENUM(BlueprintType)
enum class EYvrLayerSharpenType : uint8
{
	NONE = 0,
	NORMAL_SUPER_SAMPING = 1,
	QUALITY_SUPER_SAMPLING = 2,
	NORMAL_SHARPENING = 4,
	QUALITY_SHARPENING = 8,
};


UENUM(BlueprintType)
enum class EYvrDynamicResolutionSetting : uint8
{
	HIGH_QUALITY,
	BALANCED,
	BATTERY_SAVING,
};

UCLASS()
class YVRXRHMD_API UYvrXRFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static void GetPose(FRotator& DeviceRotation, FVector& DevicePosition, FVector& NeckPosition, bool bUseOrienationForPlayerCamera = false, bool bUsePositionForPlayerCamera = false, const FVector PositionScale = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void SetCPUAndGPULevels(int CPULevel, int GPULevel);

	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static bool HasInputFocus();

	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static bool HasSystemOverlayPresent();

	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static EYvrFixedFoveatedRenderingLevel GetFixedFoveatedRenderingLevel();

	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void SetFixedFoveatedRenderingLevel(EYvrFixedFoveatedRenderingLevel Level);

	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static TArray<float> GetAvailableDisplayFrequencies();

	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static float GetCurrentDisplayFrequency();

	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void SetDisplayFrequency(float RequestedFrequency);

	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void SetColorScaleAndOffset(FLinearColor ColorScale, FLinearColor ColorOffset, bool bApplyToAllLayers = false);

	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void SetSpaceWarpEnabled(bool bIsEnabled);

	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void SetSeeThroughBackgroundEnabled(bool bIsEnabled);

	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void SetBaseRotationAndBaseOffsetInMeters(FRotator Rotation, FVector BaseOffsetInMeters, EOrientPositionSelector::Type Options);

	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static void GetBaseRotationAndBaseOffsetInMeters(FRotator& OutRotation, FVector& OutBaseOffsetInMeters);

	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void SetSharpenType(EYvrLayerSharpenType SharpenType, bool bApplyToAllLayers = false);

protected:
	static class FYvrXRHMD* GetYvrHMD();
};