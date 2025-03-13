// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "HeadMountedDisplayTypes.h"
#include "YvrXRCore.h"

enum class EYvrXRHandType : uint8;
enum class EYvrXRHandJoint : uint8;
enum class EYvrXRHandTrackingConfidence : uint8;
enum class EYvrXRActiveInputDevice : uint8;
enum class EYvrXRHandFinger : uint8;

class IYvrXRHandTracking
{
public:
	virtual ~IYvrXRHandTracking() {}

	virtual FQuat GetBoneRotation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId) = 0;
	virtual FVector GetBoneLocation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId) = 0;
	virtual float GetHandScale(const EYvrXRHandType DeviceHand) = 0;
	virtual float GetBoneRadii(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId) = 0;
	virtual bool IsBoneOrientationValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId) = 0;
	virtual bool IsBonePositionValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId) = 0;
	virtual bool IsBoneOrientationTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId) = 0;
	virtual bool IsBonePositionTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId) = 0;
	virtual EYvrXRHandTrackingConfidence GetTrackingConfidence(const EYvrXRHandType DeviceHand) = 0;
	virtual FTransform GetPointerPose(const EYvrXRHandType DeviceHand) = 0;

	virtual bool IsComputed(const EYvrXRHandType DeviceHand) = 0;
	virtual bool IsAimValid(const EYvrXRHandType DeviceHand) = 0;
	virtual bool IsSystemGesture(const EYvrXRHandType DeviceHand) = 0;
	virtual bool IsDominantHand(const EYvrXRHandType DeviceHand) = 0;
	virtual bool IsMenuPressed(const EYvrXRHandType DeviceHand) = 0;

	virtual bool GetFingerIsPinching(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger) = 0;
	virtual float GetFingerPinchStrength(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger) = 0;

	virtual bool IsHandTrackingEnabled() = 0;
	virtual FString GetBoneName(uint8 Bone) = 0;
};

