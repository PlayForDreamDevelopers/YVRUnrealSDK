// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrPreviewInput.h"
#include "YvrPreviewManager.h"
#include "YVR_Lib_Types.h"

#define LOCTEXT_NAMESPACE "YvrInput"

#include "Logging/LogMacros.h"
DEFINE_LOG_CATEGORY(LogYvrPreview);

using namespace YvrInput;

//**************
//Yvr Key
//**************
//Button
const FKey FYvrKey::YvrButton_A("YvrButton_A");
const FKey FYvrKey::YvrButton_B("YvrButton_B");
const FKey FYvrKey::YvrButton_X("YvrButton_X");
const FKey FYvrKey::YvrButton_Y("YvrButton_Y");
const FKey FYvrKey::YvrButton_Start("YvrButton_Start");
const FKey FYvrKey::YvrButton_Reserved("YvrButton_Reserved");
const FKey FYvrKey::YvrButton_LIndexTrigger("YvrButton_LIndexTrigger");
const FKey FYvrKey::YvrButton_RIndexTrigger("YvrButton_RIndexTrigger");
const FKey FYvrKey::YvrButton_LHandTrigger("YvrButton_LHandTrigger");
const FKey FYvrKey::YvrButton_RHandTrigger("YvrButton_RHandTrigger");
const FKey FYvrKey::YvrButton_LThumbstick("YvrButton_LThumbstick");
const FKey FYvrKey::YvrButton_RThumbstick("YvrButton_RThumbstick");
const FKey FYvrKey::YvrButton_LThumbstickUp("YvrButton_LThumbstickUp");
const FKey FYvrKey::YvrButton_LThumbstickDown("YvrButton_LThumbstickDown");
const FKey FYvrKey::YvrButton_LThumbstickLeft("YvrButton_LThumbstickLeft");
const FKey FYvrKey::YvrButton_LThumbstickRight("YvrButton_LThumbstickRight");
const FKey FYvrKey::YvrButton_RThumbstickUp("YvrButton_RThumbstickUp");
const FKey FYvrKey::YvrButton_RThumbstickDown("YvrButton_RThumbstickDown");
const FKey FYvrKey::YvrButton_RThumbstickLeft("YvrButton_RThumbstickLeft");
const FKey FYvrKey::YvrButton_RThumbstickRight("YvrButton_RThumbstickRight");

//Touch
const FKey FYvrKey::YvrTouch_A("YvrTouch_A");
const FKey FYvrKey::YvrTouch_B("YvrTouch_B");
const FKey FYvrKey::YvrTouch_X("YvrTouch_X");
const FKey FYvrKey::YvrTouch_Y("YvrTouch_Y");
const FKey FYvrKey::YvrTouch_LIndexTrigger("YvrTouch_LIndexTrigger");
const FKey FYvrKey::YvrTouch_RIndexTrigger("YvrTouch_RIndexTrigger");
const FKey FYvrKey::YvrTouch_LThumbstick("YvrTouch_LThumbstick");
const FKey FYvrKey::YvrTouch_RThumbstick("YvrTouch_RThumbstick");

//Analog
const FKey FYvrKey::YvrButton_LIndexTrigger_Axis("YvrButton_LIndexTrigger_Axis");
const FKey FYvrKey::YvrButton_LThumbstick_X("YvrButton_LThumbstick_X");
const FKey FYvrKey::YvrButton_LThumbstick_Y("YvrButton_LThumbstick_Y");
const FKey FYvrKey::YvrButton_RIndexTrigger_Axis("YvrButton_RIndexTrigger_Axis");
const FKey FYvrKey::YvrButton_RThumbstick_X("YvrButton_RThumbstick_X");
const FKey FYvrKey::YvrButton_RThumbstick_Y("YvrButton_RThumbstick_Y");

//**************
//Yvr Key Name
//**************

//Button
const FName FYvrKeyNames::YvrButton_A("YvrButton_A");
const FName FYvrKeyNames::YvrButton_B("YvrButton_B");
const FName FYvrKeyNames::YvrButton_X("YvrButton_X");
const FName FYvrKeyNames::YvrButton_Y("YvrButton_Y");
const FName FYvrKeyNames::YvrButton_Start("YvrButton_Start");
const FName FYvrKeyNames::YvrButton_Reserved("YvrButton_Reserved");
const FName FYvrKeyNames::YvrButton_LIndexTrigger("YvrButton_LIndexTrigger");
const FName FYvrKeyNames::YvrButton_RIndexTrigger("YvrButton_RIndexTrigger");
const FName FYvrKeyNames::YvrButton_LHandTrigger("YvrButton_LHandTrigger");
const FName FYvrKeyNames::YvrButton_RHandTrigger("YvrButton_RHandTrigger");
const FName FYvrKeyNames::YvrButton_LThumbstick("YvrButton_LThumbstick");
const FName FYvrKeyNames::YvrButton_RThumbstick("YvrButton_RThumbstick");
const FName FYvrKeyNames::YvrButton_LThumbstickUp("YvrButton_LThumbstickUp");
const FName FYvrKeyNames::YvrButton_LThumbstickDown("YvrButton_LThumbstickDown");
const FName FYvrKeyNames::YvrButton_LThumbstickLeft("YvrButton_LThumbstickLeft");
const FName FYvrKeyNames::YvrButton_LThumbstickRight("YvrButton_LThumbstickRight");
const FName FYvrKeyNames::YvrButton_RThumbstickUp("YvrButton_RThumbstickUp");
const FName FYvrKeyNames::YvrButton_RThumbstickDown("YvrButton_RThumbstickDown");
const FName FYvrKeyNames::YvrButton_RThumbstickLeft("YvrButton_RThumbstickLeft");
const FName FYvrKeyNames::YvrButton_RThumbstickRight("YvrButton_RThumbstickRight");

//Touch
const FName FYvrKeyNames::YvrTouch_A("YvrTouch_A");
const FName FYvrKeyNames::YvrTouch_B("YvrTouch_B");
const FName FYvrKeyNames::YvrTouch_X("YvrTouch_X");
const FName FYvrKeyNames::YvrTouch_Y("YvrTouch_Y");
const FName FYvrKeyNames::YvrTouch_LIndexTrigger("YvrTouch_LIndexTrigger");
const FName FYvrKeyNames::YvrTouch_RIndexTrigger("YvrTouch_RIndexTrigger");
const FName FYvrKeyNames::YvrTouch_LThumbstick("YvrTouch_LThumbstick");
const FName FYvrKeyNames::YvrTouch_RThumbstick("YvrTouch_RThumbstick");

FYvrPreviewInput::FYvrPreviewInput()
	:MessageHandler(new FGenericApplicationMessageHandler())
{
	AddKeysToEngine();
	IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);

	// set window size
	if (yvrPreviewHMD == nullptr)
	{
		static FName SystemName(TEXT("YvrPreviewHMD"));
		if (GEngine)
		{
			if (GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == SystemName))
			{
				yvrPreviewHMD = static_cast<FYvrPreviewHMD*>(GEngine->XRSystem.Get());
				if (yvrPreviewHMD)
				{
					if (yvrPreviewHMD->InitializePreviewWindows())
					{
						UE_LOG(LogYvrPreview, Log, TEXT("InitializePreviewWindows Success"));
					}
				}
			}
		}
	}
}

FYvrPreviewInput::~FYvrPreviewInput()
{
	IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);
}

void FYvrPreviewInput::SendControllerEvents()
{
	{
		// left controller
		yvrControllerState controllerState;
		bool controllerConnect;
		YvrPreviewManager::GetInstance()->GetControllerState(EControllerHand::Left, &controllerState, &controllerConnect);

		uint32_t RemoteButton = controllerState.Buttons;
		uint32_t RemoteTouch = controllerState.Touches;
		float RemoteIndexTrigger = controllerState.IndexTrigger;
		float RemoteThumbstickX = controllerState.Thumbstick[0];
		float RemoteThumbstickY = controllerState.Thumbstick[1];

		bool bButtonPressed = false;
		bool bTouchPressed = false;
		float IndexTriggerAxis = 0.0;
		float ThumbstickAxisX = 0.0;
		float ThumbstickAxisY = 0.0;

		//simulate left index trigger pressed event
		if (RemoteIndexTrigger >= 0.5)
		{
			RemoteButton |= (1 << EYvrControllerButton::LIndexTrigger);
		}

		for (uint32_t Index = 0; Index < EYvrControllerButton::Total; ++Index)
		{
			//Button
			FYvrButtonState& ButtonState = CachedLeftControllerState.Buttons[Index];
			bButtonPressed = RemoteButton & (1 << Index);
			if (bButtonPressed != ButtonState.bIsPressed)
			{
				ButtonState.bIsPressed = bButtonPressed;

				if (ButtonState.bIsPressed)
				{
					MessageHandler->OnControllerButtonPressed(ButtonState.Key, 0, false);
					UE_LOG(LogYvrPreview, Log, TEXT("Button %s pressed"), *ButtonState.Key.ToString());
				}
				else
				{
					MessageHandler->OnControllerButtonReleased(ButtonState.Key, 0, false);
					UE_LOG(LogYvrPreview, Log, TEXT("Button %s released"), *ButtonState.Key.ToString());
				}
			}

			//Touch
			FYvrButtonState& TouchState = CachedLeftControllerState.Touches[Index];
			bTouchPressed = RemoteTouch & (1 << Index);

			if (bTouchPressed != TouchState.bIsPressed)
			{
				TouchState.bIsPressed = bTouchPressed;
				if (TouchState.bIsPressed)
				{
					MessageHandler->OnControllerButtonPressed(TouchState.Key, 0, false);
					UE_LOG(LogYvrPreview, Log, TEXT("Touch %s pressed"), *TouchState.Key.ToString());
				}
				else
				{
					MessageHandler->OnControllerButtonReleased(TouchState.Key, 0, false);
					UE_LOG(LogYvrPreview, Log, TEXT("Touch %s released"), *TouchState.Key.ToString());
				}
			}

			//Analog
			IndexTriggerAxis = RemoteIndexTrigger;
			if (IndexTriggerAxis != CachedLeftControllerState.IndexTriggerAxis)
			{
				CachedLeftControllerState.IndexTriggerAxis = IndexTriggerAxis;
				MessageHandler->OnControllerAnalog(FYvrKey::YvrButton_LIndexTrigger_Axis.GetFName(), 0, IndexTriggerAxis);
				UE_LOG(LogYvrPreview, Log, TEXT("IndexTriggerAxis %f "), IndexTriggerAxis);
			}

			ThumbstickAxisX = RemoteThumbstickX;
			if (ThumbstickAxisX != CachedLeftControllerState.ThumbstickAxis[0])
			{
				CachedLeftControllerState.ThumbstickAxis[0] = ThumbstickAxisX;
				MessageHandler->OnControllerAnalog(FYvrKey::YvrButton_LThumbstick_X.GetFName(), 0, ThumbstickAxisX);
				UE_LOG(LogYvrPreview, Log, TEXT("Left ThumbstickAxisX %f "), ThumbstickAxisX);
			}

			ThumbstickAxisY = RemoteThumbstickY;
			if (ThumbstickAxisY != CachedLeftControllerState.ThumbstickAxis[1])
			{
				CachedLeftControllerState.ThumbstickAxis[1] = ThumbstickAxisY;
				MessageHandler->OnControllerAnalog(FYvrKey::YvrButton_LThumbstick_Y.GetFName(), 0, ThumbstickAxisY);
				UE_LOG(LogYvrPreview, Log, TEXT("Left ThumbstickAxisY %f "), ThumbstickAxisY);
			}
		}

	}

	{
		// right controller
		yvrControllerState rightControllerState;
		bool rightControllerConnect;
		YvrPreviewManager::GetInstance()->GetControllerState(EControllerHand::Right, &rightControllerState, &rightControllerConnect);

		uint32_t RemoteButton = rightControllerState.Buttons;
		uint32_t RemoteTouch = rightControllerState.Touches;
		float RemoteIndexTrigger = rightControllerState.IndexTrigger;
		float RemoteThumbstickX = rightControllerState.Thumbstick[0];
		float RemoteThumbstickY = rightControllerState.Thumbstick[1];

		bool bButtonPressed = false;
		bool bTouchPressed = false;
		float IndexTriggerAxis = 0.0;
		float ThumbstickAxisX = 0.0;
		float ThumbstickAxisY = 0.0;

		//simulate right index trigger pressed event
		if (RemoteIndexTrigger >= 0.5)
		{
			RemoteButton |= (1 << EYvrControllerButton::RIndexTrigger);
		}

		for (uint32_t Index = 0; Index < EYvrControllerButton::Total; ++Index)
		{
			//Button
			FYvrButtonState& ButtonState = CachedRightControllerState.Buttons[Index];
			bButtonPressed = RemoteButton & (1 << Index);
			if (bButtonPressed != ButtonState.bIsPressed)
			{
				ButtonState.bIsPressed = bButtonPressed;

				if (ButtonState.bIsPressed)
				{
					MessageHandler->OnControllerButtonPressed(ButtonState.Key, 0, false);
					UE_LOG(LogYvrPreview, Log, TEXT("Button %s pressed"), *ButtonState.Key.ToString());
				}
				else
				{
					MessageHandler->OnControllerButtonReleased(ButtonState.Key, 0, false);
					UE_LOG(LogYvrPreview, Log, TEXT("Button %s released"), *ButtonState.Key.ToString());
				}
			}

			//Touch
			FYvrButtonState& TouchState = CachedRightControllerState.Touches[Index];
			bTouchPressed = RemoteTouch & (1 << Index);

			if (bTouchPressed != TouchState.bIsPressed)
			{
				TouchState.bIsPressed = bTouchPressed;
				if (TouchState.bIsPressed)
				{
					MessageHandler->OnControllerButtonPressed(TouchState.Key, 0, false);
					UE_LOG(LogYvrPreview, Log, TEXT("Touch %s pressed"), *TouchState.Key.ToString());
				}
				else
				{
					MessageHandler->OnControllerButtonReleased(TouchState.Key, 0, false);
					UE_LOG(LogYvrPreview, Log, TEXT("Touch %s released"), *TouchState.Key.ToString());
				}
			}

			//Analog
			IndexTriggerAxis = RemoteIndexTrigger;
			if (IndexTriggerAxis != CachedRightControllerState.IndexTriggerAxis)
			{
				CachedRightControllerState.IndexTriggerAxis = IndexTriggerAxis;
				MessageHandler->OnControllerAnalog(FYvrKey::YvrButton_RIndexTrigger_Axis.GetFName(), 0, IndexTriggerAxis);
				UE_LOG(LogYvrPreview, Log, TEXT("IndexTriggerAxis %f "), IndexTriggerAxis);
			}

			ThumbstickAxisX = RemoteThumbstickX;
			if (ThumbstickAxisX != CachedRightControllerState.ThumbstickAxis[0])
			{
				CachedRightControllerState.ThumbstickAxis[0] = ThumbstickAxisX;
				MessageHandler->OnControllerAnalog(FYvrKey::YvrButton_RThumbstick_X.GetFName(), 0, ThumbstickAxisX);
				UE_LOG(LogYvrPreview, Log, TEXT("Right ThumbstickAxisX %f "), ThumbstickAxisX);
			}

			ThumbstickAxisY = RemoteThumbstickY;
			if (ThumbstickAxisY != CachedRightControllerState.ThumbstickAxis[1])
			{
				CachedRightControllerState.ThumbstickAxis[1] = ThumbstickAxisY;
				MessageHandler->OnControllerAnalog(FYvrKey::YvrButton_RThumbstick_Y.GetFName(), 0, ThumbstickAxisY);
				UE_LOG(LogYvrPreview, Log, TEXT("Right ThumbstickAxisY %f "), ThumbstickAxisY);
			}
		}
	}
}

void FYvrPreviewInput::SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

bool FYvrPreviewInput::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FYvrPreviewInput::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{

}

void FYvrPreviewInput::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values)
{

}

FName FYvrPreviewInput::GetMotionControllerDeviceTypeName() const
{
	return FName(TEXT("YvrPreviewInput"));
}

bool FYvrPreviewInput::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const
{
	yvrPoseState controllerPose;
	bool isConnected;
	if (yvrPreviewHMD && YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_CONNECTED)
	{
		YvrPreviewManager::GetInstance()->GetControllerPose(DeviceHand, &controllerPose, &isConnected);
		if (isConnected)
		{
			OutOrientation = FQuat((float)-controllerPose.pose.rotation.z, (float)controllerPose.pose.rotation.x, (float)controllerPose.pose.rotation.y, (float)-controllerPose.pose.rotation.w).Rotator();
			OutPosition = FVector((float)-controllerPose.pose.position.z * WorldToMetersScale, (float)controllerPose.pose.position.x * WorldToMetersScale, (float)controllerPose.pose.position.y * WorldToMetersScale);
		}
		return isConnected;
	}
	return false;
}

ETrackingStatus FYvrPreviewInput::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const
{
	if (ControllerIndex == 0 && (DeviceHand == EControllerHand::Left || DeviceHand == EControllerHand::Right || DeviceHand == EControllerHand::AnyHand))
	{
		return ETrackingStatus::Tracked;
	}
	return ETrackingStatus::NotTracked;
}


void FYvrPreviewInput::SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values)
{

}

void FYvrPreviewInput::GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const
{
	MinFrequency = 0.f;
	MaxFrequency = 1.f;
}

float FYvrPreviewInput::GetHapticAmplitudeScale() const
{
	return 1.0f;
}

// Helper Functions
void FYvrPreviewInput::AddKeysToEngine()
{
	// Register the FKeys
	//Button
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_A, LOCTEXT("YvrButton_A", "Yvr Preview Button (R) A"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_B, LOCTEXT("YvrButton_B", "Yvr Preview Button (R) B"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_X, LOCTEXT("YvrButton_X", "Yvr Preview Button (L) X"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_Y, LOCTEXT("YvrButton_Y", "Yvr Preview Button (L) Y"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_Start, LOCTEXT("YvrButton_Start", "Yvr Preview Button (L) Start"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_Reserved, LOCTEXT("YvrButton_Reserved", "Yvr Preview Button Reserved"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LIndexTrigger, LOCTEXT("YvrButton_LIndexTrigger", "Yvr Preview Button (L) Index Trigger"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RIndexTrigger, LOCTEXT("YvrButton_RIndexTrigger", "Yvr Preview Button (R) Index Trigger"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LHandTrigger, LOCTEXT("YvrButton_LHandTrigger", "Yvr Preview Button (L) Hand Trigger"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RHandTrigger, LOCTEXT("YvrButton_RHandTrigger", "Yvr Preview Button (R) Hand Trigger"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LThumbstick, LOCTEXT("YvrButton_LThumbstick", "Yvr Preview Button (L) Thumbstick"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RThumbstick, LOCTEXT("YvrButton_RThumbstick", "Yvr Preview Button (R) Thumbstick"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LThumbstickUp, LOCTEXT("YvrButton_LThumbstickUp", "Yvr Preview Button (L) Thumbstick Up"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LThumbstickDown, LOCTEXT("YvrButton_LThumbstickDown", "Yvr Preview Button (L) Thumbstick Down"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LThumbstickLeft, LOCTEXT("YvrButton_LThumbstickLeft", "Yvr Preview Button (L) Thumbstick Left"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LThumbstickRight, LOCTEXT("YvrButton_LThumbstickRight", "Yvr Preview Button (L) Thumbstick Right"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RThumbstickUp, LOCTEXT("YvrButton_RThumbstickUp", "Yvr Preview Button (R) Thumbstick Up"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RThumbstickDown, LOCTEXT("YvrButton_RThumbstickDown", "Yvr Preview Button (R) Thumbstick Down"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RThumbstickLeft, LOCTEXT("YvrButton_RThumbstickLeft", "Yvr Preview Button (R) Thumbstick Left"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RThumbstickRight, LOCTEXT("YvrButton_RThumbstickRight", "Yvr Preview Button (R) Thumbstick Right"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	//Touch
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrTouch_A, LOCTEXT("YvrTouch_A", "Yvr Preview Touch (R) A"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrTouch_B, LOCTEXT("YvrTouch_B", "Yvr Preview Touch (R) B"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrTouch_X, LOCTEXT("YvrTouch_X", "Yvr Preview Touch (L) X"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrTouch_Y, LOCTEXT("YvrTouch_Y", "Yvr Preview Touch (L) Y"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrTouch_LIndexTrigger, LOCTEXT("YvrTouch_LIndexTrigger", "Yvr Preview Touch (L) Index Trigger"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrTouch_RIndexTrigger, LOCTEXT("YvrTouch_RIndexTrigger", "Yvr Preview Touch (R) Index Trigger"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrTouch_LThumbstick, LOCTEXT("YvrTouch_LThumbstick", "Yvr Preview Touch (L) Thumbstick"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrTouch_RThumbstick, LOCTEXT("YvrTouch_RThumbstick", "Yvr Preview Touch (R) Thumbstick"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	//Analog
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LIndexTrigger_Axis, LOCTEXT("YvrButton_LIndexTrigger_Axis", "Yvr Preview Button (L) Index Trigger Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LThumbstick_X, LOCTEXT("YvrButton_LThumbstick_X", "Yvr Preview Button (L) Thumbstick X"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_LThumbstick_Y, LOCTEXT("YvrButton_LThumbstick_Y", "Yvr Preview Button (L) Thumbstick Y"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RIndexTrigger_Axis, LOCTEXT("YvrButton_RIndexTrigger_Axis", "Yvr Preview Button (R) Index Trigger Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RThumbstick_X, LOCTEXT("YvrButton_RThumbstick_X", "Yvr Preview Button (R) Thumbstick X"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::YvrButton_RThumbstick_Y, LOCTEXT("YvrButton_RThumbstick_Y", "Yvr Preview Button (R) Thumbstick Y"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
}

#undef LOCTEXT_NAMESPACE