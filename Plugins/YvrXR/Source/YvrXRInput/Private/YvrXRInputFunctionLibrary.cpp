// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRInputFunctionLibrary.h"
#include "Logging/MessageLog.h"
#include "YvrXRInput.h"

//-------------------------------------------------------------------------------------------------
// UYvrXRHandTrackingFunctionLibrary
//-------------------------------------------------------------------------------------------------
UYvrXRInputFunctionLibrary::UYvrXRInputFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


FYvrXRInputPlugin::FYvrXRInput* GetYvrXRInput()
{
	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	for (auto MotionController : MotionControllers)
	{
		if (MotionController != nullptr && MotionController->GetMotionControllerDeviceTypeName() == FName(TEXT("YvrXRInput")))
		{
			return (FYvrXRInputPlugin::FYvrXRInput*)(MotionController);
		}
	}
	return nullptr;
}

FQuat UYvrXRInputFunctionLibrary::GetBoneRotation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->GetBoneRotation(DeviceHand, BoneId);
	}

	return FQuat::Identity;
}

FVector UYvrXRInputFunctionLibrary::GetBoneLocation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->GetBoneLocation(DeviceHand, BoneId);
	}

	return FVector::ZeroVector;
}

float UYvrXRInputFunctionLibrary::GetHandScale(const EYvrXRHandType DeviceHand)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->GetHandScale(DeviceHand);
	}

	return 1.0f;
}

float UYvrXRInputFunctionLibrary::GetBoneRadii(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->GetBoneRadii(DeviceHand, BoneId);
	}

	return 1.0f;
}

bool UYvrXRInputFunctionLibrary::IsBoneOrientationValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsBoneOrientationValid(DeviceHand, BoneId);
	}

	return false;
}

bool UYvrXRInputFunctionLibrary::IsBonePositionValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsBonePositionValid(DeviceHand, BoneId);
	}

	return false;
}

bool UYvrXRInputFunctionLibrary::IsBoneOrientationTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsBoneOrientationTracked(DeviceHand, BoneId);
	}

	return false;
}

bool UYvrXRInputFunctionLibrary::IsBonePositionTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsBonePositionTracked(DeviceHand, BoneId);
	}

	return false;
}

EYvrXRHandTrackingConfidence UYvrXRInputFunctionLibrary::GetTrackingConfidence(const EYvrXRHandType DeviceHand)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->GetTrackingConfidence(DeviceHand);
	}

	return EYvrXRHandTrackingConfidence::Low;
}

bool UYvrXRInputFunctionLibrary::IsComputed(const EYvrXRHandType DeviceHand)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsComputed(DeviceHand);
	}

	return false;
}

bool UYvrXRInputFunctionLibrary::IsAimValid(const EYvrXRHandType DeviceHand)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsAimValid(DeviceHand);
	}

	return false;
}

bool UYvrXRInputFunctionLibrary::IsSystemGesture(const EYvrXRHandType DeviceHand)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsSystemGesture(DeviceHand);
	}

	return false;
}

bool UYvrXRInputFunctionLibrary::IsDominantHand(const EYvrXRHandType DeviceHand)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsDominantHand(DeviceHand);
	}

	return false;
}

bool UYvrXRInputFunctionLibrary::IsMenuPressed(const EYvrXRHandType DeviceHand)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsMenuPressed(DeviceHand);
	}

	return false;
}

bool UYvrXRInputFunctionLibrary::GetFingerIsPinching(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->GetFingerIsPinching(DeviceHand, Finger);
	}

	return false;
}

float UYvrXRInputFunctionLibrary::GetFingerPinchStrength(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->GetFingerPinchStrength(DeviceHand, Finger);
	}

	return 0.0f;
}

bool UYvrXRInputFunctionLibrary::IsHandTrackingEnabled()
{
	// Todo
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->IsHandTrackingEnabled();
	}

	return false;
}

FTransform UYvrXRInputFunctionLibrary::GetPointerPose(const EYvrXRHandType DeviceHand)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->GetPointerPose(DeviceHand);
	}

	return FTransform::Identity;
}

FString UYvrXRInputFunctionLibrary::GetBoneName(EYvrXRHandJoint BoneId)
{
	FYvrXRInputPlugin::FYvrXRInput* YvrXRInput = GetYvrXRInput();
	if (YvrXRInput != nullptr)
	{
		return YvrXRInput->GetBoneName((uint8)BoneId);
	}

	return FString("Invalid");
}
