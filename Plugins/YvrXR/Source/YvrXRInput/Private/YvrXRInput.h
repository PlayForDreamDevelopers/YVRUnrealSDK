// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "YvrXRCore.h"
#include "YvrXRInputState.h"
#include "GenericPlatform/IInputInterface.h"
#include "XRMotionControllerBase.h"
#include "IInputDevice.h"
#include "IHapticDevice.h"
#include "HeadMountedDisplayTypes.h"

#include "IYvrXRInputPlugin.h"
#include "IYvrXRHandTracking.h"

#define YVRXR_INPUT_SUPPORTED_PLATFORMS (PLATFORM_ANDROID_ARM || PLATFORM_ANDROID_ARM64)

class FYvrXRHMD;
struct FInputActionKeyMapping;
struct FInputAxisKeyMapping;

class FYvrXRInputPlugin : public IYvrXRInputPlugin
{
public:
	struct FOpenXRAction
	{
		XrActionSet		Set;
		XrActionType	Type;
		FName			Name;
		XrAction		Handle;

		FOpenXRAction(XrActionSet InActionSet, XrActionType InActionType, const FName& InName);
	};

	struct FOpenXRController
	{
		XrActionSet		ActionSet;
		XrPath			UserPath;
		XrAction		GripAction;
		XrAction		AimAction;
		XrAction		VibrationAction;
		int32			GripDeviceId;
		int32			AimDeviceId;

		FOpenXRController(XrActionSet InActionSet, XrPath InUserPath, const char* InName);

		void AddActionDevices(FYvrXRHMD* HMD);
	};

	struct FInteractionProfile
	{
	public:
		bool HasHaptics;
		XrPath Path;
		TArray<XrActionSuggestedBinding> Bindings;

		FInteractionProfile(XrPath InProfile, bool InHasHaptics);
	};

	struct FYvrXRHandState : public FNoncopyable
	{
		FYvrXRHandState()
		{
			// Velocity
			Velocities.jointCount = XR_HAND_JOINT_COUNT_EXT;
			Velocities.jointVelocities = JointVelocities;

			// Aim
			AimState.next = nullptr;
			// Scale
			Scale.next = &AimState;
			Scale.sensorOutput = 1.0f;
			Scale.currentOutput = 1.0f;
			Scale.overrideHandScale = XR_TRUE;
			Scale.overrideValueInput = 1.00f;

			Locations.next = &Scale;
			Locations.jointCount = XR_HAND_JOINT_COUNT_EXT;
			Locations.jointLocations = JointLocations;


		}

		XrHandTrackerEXT HandTracker{};
		XrHandJointLocationEXT JointLocations[XR_HAND_JOINT_COUNT_EXT];
		XrHandJointVelocityEXT JointVelocities[XR_HAND_JOINT_COUNT_EXT];
		XrHandJointVelocitiesEXT Velocities{ XR_TYPE_HAND_JOINT_VELOCITIES_EXT };
		XrHandJointLocationsEXT Locations{ XR_TYPE_HAND_JOINT_LOCATIONS_EXT };
		XrHandTrackingAimStateFB AimState{ XR_TYPE_HAND_TRACKING_AIM_STATE_FB };
		XrHandTrackingScaleFB Scale{ XR_TYPE_HAND_TRACKING_SCALE_FB };

		// Transforms are cached in Unreal Tracking Space
		FTransform KeypointTransforms[EHandKeypointCount];
		float Radii[EHandKeypointCount];
		float HandScale;
		bool ReceivedJointPoses = false;


		bool GetTransform(EHandKeypoint Keypoint, FTransform& OutTransform) const
		{
			check((int32)Keypoint < EHandKeypointCount);
			OutTransform = KeypointTransforms[(uint32)Keypoint];

			return ReceivedJointPoses;
		}

		const FTransform& GetTransform(EHandKeypoint Keypoint) const
		{
			check((int32)Keypoint < EHandKeypointCount);
			return KeypointTransforms[(uint32)Keypoint];
		}
	};
	

	class FYvrXRInput : public IInputDevice, public FXRMotionControllerBase, public IHapticDevice, public IYvrXRHandTracking, public TSharedFromThis<FYvrXRInput>
	{
	public:
		FYvrXRInput(FYvrXRHMD* HMD);
		virtual ~FYvrXRInput();

		// IInputDevice overrides
		virtual void Tick(float DeltaTime) override;
		virtual void SendControllerEvents() override;
		virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;
		virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
		virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
		virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) override;

		// IMotionController overrides
		virtual FName GetMotionControllerDeviceTypeName() const override;
		virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const FName MotionSource, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const override;
#if ENGINE_MAJOR_VERSION > 4
		virtual bool GetControllerOrientationAndPositionForTime(const int32 ControllerIndex, const FName MotionSource, FTimespan Time, bool& OutTimeWasUsed, FRotator& OutOrientation, FVector& OutPosition, bool& OutbProvidedLinearVelocity, FVector& OutLinearVelocity, bool& OutbProvidedAngularVelocity, FVector& OutAngularVelocityRadPerSec, bool& OutbProvidedLinearAcceleration, FVector& OutLinearAcceleration, float WorldToMetersScale) const override;
#else
		virtual bool GetControllerOrientationAndPositionForTime(const int32 ControllerIndex, const FName MotionSource, FTimespan Time, bool& OutTimeWasUsed, FRotator& OutOrientation, FVector& OutPosition, bool& OutbProvidedLinearVelocity, FVector& OutLinearVelocity, bool& OutbProvidedAngularVelocity, FVector& OutAngularVelocityRadPerSec, float WorldToMetersScale) const override;
#endif

		virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const FName MotionSource) const override;
		virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const FName MotionSource, FRotator& OutOrientation, FVector& OutPosition, bool& OutbProvidedLinearVelocity, FVector& OutLinearVelocity, bool& OutbProvidedAngularVelocity, FVector& OutAngularVelocityAsAxisAndLength, bool& OutbProvidedLinearAcceleration, FVector& OutLinearAcceleration, float WorldToMetersScale) const override;
		virtual void EnumerateSources(TArray<FMotionControllerSource>& SourcesOut) const override;

		// IHapticDevice overrides
		IHapticDevice* GetHapticDevice() override { return (IHapticDevice*)this; }
		virtual void SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values) override;

		virtual void GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const override;
		virtual float GetHapticAmplitudeScale() const override;

		// IYvrXRHandTracking overrides
		virtual FString GetBoneName(uint8 Bone);
		virtual bool IsHandTrackingEnabled();

		virtual FQuat GetBoneRotation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);
		virtual FVector GetBoneLocation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);
		virtual float GetHandScale(const EYvrXRHandType DeviceHand);
		virtual float GetBoneRadii(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);
		virtual bool IsBoneOrientationValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);
		virtual bool IsBonePositionValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);
		virtual bool IsBoneOrientationTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);
		virtual bool IsBonePositionTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId);
		virtual EYvrXRHandTrackingConfidence GetTrackingConfidence(const EYvrXRHandType DeviceHand);
		virtual FTransform GetPointerPose(const EYvrXRHandType DeviceHand);

		virtual bool IsComputed(const EYvrXRHandType DeviceHand);
		virtual bool IsAimValid(const EYvrXRHandType DeviceHand);
		virtual bool IsSystemGesture(const EYvrXRHandType DeviceHand);
		virtual bool IsDominantHand(const EYvrXRHandType DeviceHand);
		virtual bool IsMenuPressed(const EYvrXRHandType DeviceHand);

		virtual bool GetFingerIsPinching(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger);
		virtual float GetFingerPinchStrength(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger);

		const FYvrXRHandState& GetLeftHandState() const;
		const FYvrXRHandState& GetRightHandState() const;

	private:
		static const XrDuration MaxFeedbackDuration = 2500000000; // 2.5 seconds

		FYvrXRHMD* OpenXRHMD;

		TArray<XrActiveActionSet> ActionSets;
		TMap<FString, FInteractionProfile> Profiles;
		TArray<FOpenXRAction> ControllerActions[2];
		TMap<EControllerHand, FOpenXRController> Controllers;
		TMap<FName, EControllerHand> MotionSourceToControllerHandMap;
		XrAction GetActionForMotionSource(FName MotionSource) const;
		int32 GetDeviceIDForMotionSource(FName MotionSource) const;
		XrPath GetUserPathForMotionSource(FName MotionSource) const;
		bool IsOpenXRInputSupportedMotionSource(const FName MotionSource) const;
		bool bActionsBound;

		PFN_xrCreateHandTrackerEXT xrCreateHandTrackerEXT = nullptr;
		PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerEXT = nullptr;
		PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT = nullptr;
		FYvrXRHandState HandStates[2];

		TArray<XrVector3f> vertexPositions;
		TArray<XrVector3f> vertexNormals;
		TArray<XrVector2f> vertexUVs;
		TArray<XrVector4sYVR> vertexBlendIndices;
		TArray<XrVector4f> vertexBlendWeights;
		TArray<int16_t> indices;

		TArray<XrPosef> jointBindPoses;
		TArray<XrHandJointEXT> jointParents;
		TArray<float> jointRadii;

		void BuildActions();
		void BuildHands();
		void DestroyActions();
		void AddKeysToEngine();

		void AddYvrAction(XrInstance Instance, XrActionSet ActionSet, XrActionType ActionType, EControllerHand Hand, const FName& Name, const FString& ActionPath, bool bIsTrigger = false);

		void UpdateHandState();

		FYvrXRHandState& GetLeftHandState();
		FYvrXRHandState& GetRightHandState();

		/** handler to send all messages to */
		TSharedRef<FGenericApplicationMessageHandler> MessageHandler;

		YvrXRInput::FYvrControllerState CachedControllerState[2];
	};

	FYvrXRInputPlugin();
	virtual ~FYvrXRInputPlugin();

	virtual void StartupModule() override;
	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;

private:
	FYvrXRHMD* GetOpenXRHMD() const;

private:
	TSharedPtr<FYvrXRInput> InputDevice;
};