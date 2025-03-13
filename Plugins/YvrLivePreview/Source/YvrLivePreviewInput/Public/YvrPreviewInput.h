// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GenericPlatform/IInputInterface.h"
#include "XRMotionControllerBase.h"
#include "InputDevice.h"
#include "IHapticDevice.h"
#include "YvrInputState.h"
#include "YvrPreviewHMD.h"

using namespace YvrInput;

//class FYvrPreviewInput
class FYvrPreviewInput :public IInputDevice, public FXRMotionControllerBase, public IHapticDevice, public TSharedFromThis<FYvrPreviewInput>
{
public:
	FYvrPreviewInput();
	~FYvrPreviewInput();

	// IInputDevice overrides
	virtual void Tick(float DeltaTime) override { };
	virtual void SendControllerEvents() override;
	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values) override;

	// IMotionController overrides
	virtual FName GetMotionControllerDeviceTypeName() const override;
	virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const override;
	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const override;

	// IHapticDevice overrides
	IHapticDevice* GetHapticDevice() override { return (IHapticDevice*)this; }
	virtual void SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values) override;
	virtual void GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const override;
	virtual float GetHapticAmplitudeScale() const override;

	void AddKeysToEngine();

	// parameters
	FYvrPreviewHMD* yvrPreviewHMD = nullptr;
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
	YvrInput::FYvrControllerState CachedLeftControllerState;
	YvrInput::FYvrControllerState CachedRightControllerState;
};
