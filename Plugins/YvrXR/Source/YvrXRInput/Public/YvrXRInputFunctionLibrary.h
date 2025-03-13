// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "YvrXRInputFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EYvrXRHandType : uint8
{
	None,
	HandLeft,
	HandRight
};

UENUM(BlueprintType)
enum class EYvrXRHandFinger :uint8
{
	None,
	Index,
	Middle,
	Ring,
	Pinky
};

UENUM(BlueprintType)
enum class EYvrXRHandJoint : uint8
{
	Palm UMETA(DisplayName = "Palm"),
	Wrist UMETA(DisplayName = "Wrist"),
	ThumbMetacarpal UMETA(DisplayName = "Thumb Metacarpal"),
	ThumbProximal UMETA(DisplayName = "Thumb Proximal"),
	ThumbDistal UMETA(DisplayName = "Thumb Distal"),
	ThumbTip UMETA(DisplayName = "Thumb Tip"),
	IndexMetacarpal UMETA(DisplayName = "Index Metacarpal"),
	IndexProximal UMETA(DisplayName = "Index Proximal"),
	IndexIntermediate UMETA(DisplayName = "Index Intermediate"),
	IndexDistal UMETA(DisplayName = "Index Distal"),
	IndexTip UMETA(DisplayName = "Index Tip"),
	MiddleMetacarpal UMETA(DisplayName = "Middle Metacarpal"),
	MiddleProximal UMETA(DisplayName = "Middle Proximal"),
	MiddleIntermediate UMETA(DisplayName = "Middle Intermediate"),
	MiddleDistal UMETA(DisplayName = "Middle Distal"),
	MiddleTip UMETA(DisplayName = "Middle Tip"),
	RingMetacarpal UMETA(DisplayName = "Ring Metacarpal"),
	RingProximal UMETA(DisplayName = "Ring Proximal"),
	RingIntermediate UMETA(DisplayName = "Ring Intermediate"),
	RingDistal UMETA(DisplayName = "Ring Distal"),
	RingTip UMETA(DisplayName = "Ring Tip"),
	LittleMetacarpal UMETA(DisplayName = "Little Metacarpal"),
	LittleProximal UMETA(DisplayName = "Little Proximal"),
	LittleIntermediate UMETA(DisplayName = "Little Intermediate"),
	LittleDistal UMETA(DisplayName = "Little Distal"),
	LittleTip UMETA(DisplayName = "Little Tip"),
	HandJointMax UMETA(DisplayName = "Hand Joint Max")
};

UENUM(BlueprintType)
enum class EYvrXRHandTrackingConfidence : uint8
{
	Low,
	High
};

UCLASS()
class YVRXRINPUT_API UYvrXRInputFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static FQuat GetBoneRotation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static FVector GetBoneLocation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static float GetHandScale(const EYvrXRHandType DeviceHand);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static float GetBoneRadii(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsBoneOrientationValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsBonePositionValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsBoneOrientationTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsBonePositionTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static EYvrXRHandTrackingConfidence GetTrackingConfidence(const EYvrXRHandType DeviceHand);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static FTransform GetPointerPose(const EYvrXRHandType DeviceHand);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsComputed(const EYvrXRHandType DeviceHand);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsAimValid(const EYvrXRHandType DeviceHand);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsSystemGesture(const EYvrXRHandType DeviceHand);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsDominantHand(const EYvrXRHandType DeviceHand);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsMenuPressed(const EYvrXRHandType DeviceHand);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool GetFingerIsPinching(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static float GetFingerPinchStrength(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static bool IsHandTrackingEnabled();

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|HandTracking")
	static FString GetBoneName(EYvrXRHandJoint BoneId);
};