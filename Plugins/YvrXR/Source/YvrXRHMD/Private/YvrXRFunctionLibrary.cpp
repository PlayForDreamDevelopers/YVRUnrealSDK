// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRFunctionLibrary.h"
#include "YvrXRHMD.h"

UYvrXRFunctionLibrary::UYvrXRFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FYvrXRHMD* UYvrXRFunctionLibrary::GetYvrHMD()
{
	if (GEngine && GEngine->XRSystem.IsValid())
	{
		if (GEngine->XRSystem->GetSystemName() == FName("YvrXRHMD"))
		{
			return static_cast<FYvrXRHMD*>(GEngine->XRSystem.Get());
		}
	}
	return nullptr;
}

void UYvrXRFunctionLibrary::GetPose(FRotator& DeviceRotation, FVector& DevicePosition, FVector& NeckPosition, bool bUseOrienationForPlayerCamera, bool bUsePositionForPlayerCamera, const FVector PositionScale)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		FQuat HeadOrientation = FQuat::Identity;
		FVector HeadPosition = FVector::ZeroVector;

		YvrHMD->GetCurrentPose(YvrHMD->HMDDeviceId, HeadOrientation, HeadPosition);

		DeviceRotation = HeadOrientation.Rotator();
		DevicePosition = HeadPosition;
		NeckPosition = FVector::ZeroVector;
	}
	else
	{
		DeviceRotation = FRotator::ZeroRotator;
		DevicePosition = FVector::ZeroVector;
		NeckPosition = FVector::ZeroVector;
	}
}

void UYvrXRFunctionLibrary::SetCPUAndGPULevels(int CPULevel, int GPULevel)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		YvrHMD->SetCpuLevel((uint8)CPULevel);
		YvrHMD->SetGpuLevel((uint8)GPULevel);
	}
}

bool UYvrXRFunctionLibrary::HasInputFocus()
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		return YvrHMD->IsFocused();
	}
	return false;
}

bool UYvrXRFunctionLibrary::HasSystemOverlayPresent()
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		return !YvrHMD->IsFocused();
	}
	return false;
}

EYvrFixedFoveatedRenderingLevel UYvrXRFunctionLibrary::GetFixedFoveatedRenderingLevel()
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		return (EYvrFixedFoveatedRenderingLevel)YvrHMD->GetFFRLevel();
	}
	return EYvrFixedFoveatedRenderingLevel::FFR_OFF;
}

void UYvrXRFunctionLibrary::SetFixedFoveatedRenderingLevel(EYvrFixedFoveatedRenderingLevel Level)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		YvrHMD->SetFFRLevel((uint8)Level);
	}
}

TArray<float> UYvrXRFunctionLibrary::GetAvailableDisplayFrequencies()
{
	return TArray<float>({ 72.0f, 90.0f });
}

float UYvrXRFunctionLibrary::GetCurrentDisplayFrequency()
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		return YvrHMD->GetDisplayRefreshRate();
	}
	return 0.0f;
}

void UYvrXRFunctionLibrary::SetDisplayFrequency(float RequestedFrequency)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		YvrHMD->SetDisplayRefreshRate(RequestedFrequency);
	}
}

void UYvrXRFunctionLibrary::SetColorScaleAndOffset(FLinearColor ColorScale, FLinearColor ColorOffset, bool bApplyToAllLayers)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		YvrHMD->SetColorScaleAndOffset(ColorScale, ColorOffset, bApplyToAllLayers);
	}
}

void UYvrXRFunctionLibrary::SetBaseRotationAndBaseOffsetInMeters(FRotator Rotation, FVector BaseOffsetInMeters, EOrientPositionSelector::Type Options)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		if ((Options == EOrientPositionSelector::Orientation) || (Options == EOrientPositionSelector::OrientationAndPosition))
		{
			YvrHMD->SetBaseRotation(Rotation);
		}
		if ((Options == EOrientPositionSelector::Position) || (Options == EOrientPositionSelector::OrientationAndPosition))
		{
			YvrHMD->SetBasePosition(BaseOffsetInMeters * YvrHMD->GetWorldToMetersScale());
		}
	}
}

void UYvrXRFunctionLibrary::GetBaseRotationAndBaseOffsetInMeters(FRotator& OutRotation, FVector& OutBaseOffsetInMeters)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		OutRotation = YvrHMD->GetBaseRotation();
		OutBaseOffsetInMeters = YvrHMD->GetBasePosition() / YvrHMD->GetWorldToMetersScale();
	}
	else
	{
		OutRotation = FRotator::ZeroRotator;
		OutBaseOffsetInMeters = FVector::ZeroVector;
	}
}

void UYvrXRFunctionLibrary::SetSharpenType(EYvrLayerSharpenType SharpenType, bool bApplyToAllLayers)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		YvrHMD->SetSharpenType((uint8)SharpenType, bApplyToAllLayers);
	}
}

void UYvrXRFunctionLibrary::SetSpaceWarpEnabled(bool bIsEnabled)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		YvrHMD->SetSpaceWarpEnabled(bIsEnabled);
	}
}

void UYvrXRFunctionLibrary::SetSeeThroughBackgroundEnabled(bool bIsEnabled)
{
	FYvrXRHMD* YvrHMD = GetYvrHMD();
	if (YvrHMD != nullptr)
	{
		YvrHMD->SetSeeThroughBackgroundEnabled(bIsEnabled);
	}
}
