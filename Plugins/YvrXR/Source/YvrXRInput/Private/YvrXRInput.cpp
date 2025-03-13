// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRInput.h"
#include "YvrXRHMD.h"
#include "YvrXRCore.h"
#include "UObject/UObjectIterator.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"

#include "Components/SkeletalMeshComponent.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Model.h"
#include "YvrXRInputFunctionLibrary.h"

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "Editor.h"
#endif

#include "openxr.h"

#define LOCTEXT_NAMESPACE "YvrXRInputPlugin"

using namespace YvrXRInput;

//**************
//Yvr Key
//**************
const FKey FYvrKey::Yvr_Left_X_Click("Yvr_Left_X_Click");
const FKey FYvrKey::Yvr_Left_Y_Click("Yvr_Left_Y_Click");
const FKey FYvrKey::Yvr_Left_X_Touch("Yvr_Left_X_Touch");
const FKey FYvrKey::Yvr_Left_Y_Touch("Yvr_Left_Y_Touch");
const FKey FYvrKey::Yvr_Left_Menu_Click("Yvr_Left_Menu_Click");
const FKey FYvrKey::Yvr_Left_Grip_Click("Yvr_Left_Grip_Click");
const FKey FYvrKey::Yvr_Left_Grip_Axis("Yvr_Left_Grip_Axis");
const FKey FYvrKey::Yvr_Left_Trigger_Click("Yvr_Left_Trigger_Click");
const FKey FYvrKey::Yvr_Left_Trigger_Axis("Yvr_Left_Trigger_Axis");
const FKey FYvrKey::Yvr_Left_Trigger_Touch("Yvr_Left_Trigger_Touch");
const FKey FYvrKey::Yvr_Left_Thumbstick_X("Yvr_Left_Thumbstick_X");
const FKey FYvrKey::Yvr_Left_Thumbstick_Y("Yvr_Left_Thumbstick_Y");
const FKey FYvrKey::Yvr_Left_Thumbstick_Click("Yvr_Left_Thumbstick_Click");
const FKey FYvrKey::Yvr_Left_Thumbstick_Touch("Yvr_Left_Thumbstick_Touch");
const FKey FYvrKey::Yvr_Left_Thumbstick_Up("Yvr_Left_Thumbstick_Up");
const FKey FYvrKey::Yvr_Left_Thumbstick_Down("Yvr_Left_Thumbstick_Down");
const FKey FYvrKey::Yvr_Left_Thumbstick_Left("Yvr_Left_Thumbstick_Left");
const FKey FYvrKey::Yvr_Left_Thumbstick_Right("Yvr_Left_Thumbstick_Right");
const FKey FYvrKey::Yvr_Right_A_Click("Yvr_Right_A_Click");
const FKey FYvrKey::Yvr_Right_B_Click("Yvr_Right_B_Click");
const FKey FYvrKey::Yvr_Right_A_Touch("Yvr_Right_A_Touch");
const FKey FYvrKey::Yvr_Right_B_Touch("Yvr_Right_B_Touch");
const FKey FYvrKey::Yvr_Right_System_Click("Yvr_Right_System_Click");
const FKey FYvrKey::Yvr_Right_Grip_Click("Yvr_Right_Grip_Click");
const FKey FYvrKey::Yvr_Right_Grip_Axis("Yvr_Right_Grip_Axis");
const FKey FYvrKey::Yvr_Right_Trigger_Click("Yvr_Right_Trigger_Click");
const FKey FYvrKey::Yvr_Right_Trigger_Axis("Yvr_Right_Trigger_Axis");
const FKey FYvrKey::Yvr_Right_Trigger_Touch("Yvr_Right_Trigger_Touch");
const FKey FYvrKey::Yvr_Right_Thumbstick_X("Yvr_Right_Thumbstick_X");
const FKey FYvrKey::Yvr_Right_Thumbstick_Y("Yvr_Right_Thumbstick_Y");
const FKey FYvrKey::Yvr_Right_Thumbstick_Click("Yvr_Right_Thumbstick_Click");
const FKey FYvrKey::Yvr_Right_Thumbstick_Touch("Yvr_Right_Thumbstick_Touch");
const FKey FYvrKey::Yvr_Right_Thumbstick_Up("Yvr_Right_Thumbstick_Up");
const FKey FYvrKey::Yvr_Right_Thumbstick_Down("Yvr_Right_Thumbstick_Down");
const FKey FYvrKey::Yvr_Right_Thumbstick_Left("Yvr_Right_Thumbstick_Left");
const FKey FYvrKey::Yvr_Right_Thumbstick_Right("Yvr_Right_Thumbstick_Right");
const FKey FYvrKey::Yvr_Left_Thumbstick_2D("Yvr_Left_Thumbstick_2D");
const FKey FYvrKey::Yvr_Right_Thumbstick_2D("Yvr_Right_Thumbstick_2D");

const FName FYvrKeyNames::Yvr_Left_X_Click("Yvr_Left_X_Click");
const FName FYvrKeyNames::Yvr_Left_Y_Click("Yvr_Left_Y_Click");
const FName FYvrKeyNames::Yvr_Left_X_Touch("Yvr_Left_X_Touch");
const FName FYvrKeyNames::Yvr_Left_Y_Touch("Yvr_Left_Y_Touch");
const FName FYvrKeyNames::Yvr_Left_Menu_Click("Yvr_Left_Menu_Click");
const FName FYvrKeyNames::Yvr_Left_Grip_Click("Yvr_Left_Grip_Click");
const FName FYvrKeyNames::Yvr_Left_Grip_Axis("Yvr_Left_Grip_Axis");
const FName FYvrKeyNames::Yvr_Left_Trigger_Click("Yvr_Left_Trigger_Click");
const FName FYvrKeyNames::Yvr_Left_Trigger_Axis("Yvr_Left_Trigger_Axis");
const FName FYvrKeyNames::Yvr_Left_Trigger_Touch("Yvr_Left_Trigger_Touch");
const FName FYvrKeyNames::Yvr_Left_Thumbstick_X("Yvr_Left_Thumbstick_X");
const FName FYvrKeyNames::Yvr_Left_Thumbstick_Y("Yvr_Left_Thumbstick_Y");
const FName FYvrKeyNames::Yvr_Left_Thumbstick_Click("Yvr_Left_Thumbstick_Click");
const FName FYvrKeyNames::Yvr_Left_Thumbstick_Touch("Yvr_Left_Thumbstick_Touch");
const FName FYvrKeyNames::Yvr_Left_Thumbstick_Up("Yvr_Left_Thumbstick_Up");
const FName FYvrKeyNames::Yvr_Left_Thumbstick_Down("Yvr_Left_Thumbstick_Down");
const FName FYvrKeyNames::Yvr_Left_Thumbstick_Left("Yvr_Left_Thumbstick_Left");
const FName FYvrKeyNames::Yvr_Left_Thumbstick_Right("Yvr_Left_Thumbstick_Right");
const FName FYvrKeyNames::Yvr_Right_A_Click("Yvr_Right_A_Click");
const FName FYvrKeyNames::Yvr_Right_B_Click("Yvr_Right_B_Click");
const FName FYvrKeyNames::Yvr_Right_A_Touch("Yvr_Right_A_Touch");
const FName FYvrKeyNames::Yvr_Right_B_Touch("Yvr_Right_B_Touch");
const FName FYvrKeyNames::Yvr_Right_System_Click("Yvr_Right_System_Click");
const FName FYvrKeyNames::Yvr_Right_Grip_Click("Yvr_Right_Grip_Click");
const FName FYvrKeyNames::Yvr_Right_Grip_Axis("Yvr_Right_Grip_Axis");
const FName FYvrKeyNames::Yvr_Right_Trigger_Click("Yvr_Right_Trigger_Click");
const FName FYvrKeyNames::Yvr_Right_Trigger_Axis("Yvr_Right_Trigger_Axis");
const FName FYvrKeyNames::Yvr_Right_Trigger_Touch("Yvr_Right_Trigger_Touch");
const FName FYvrKeyNames::Yvr_Right_Thumbstick_X("Yvr_Right_Thumbstick_X");
const FName FYvrKeyNames::Yvr_Right_Thumbstick_Y("Yvr_Right_Thumbstick_Y");
const FName FYvrKeyNames::Yvr_Right_Thumbstick_Click("Yvr_Right_Thumbstick_Click");
const FName FYvrKeyNames::Yvr_Right_Thumbstick_Touch("Yvr_Right_Thumbstick_Touch");
const FName FYvrKeyNames::Yvr_Right_Thumbstick_Up("Yvr_Right_Thumbstick_Up");
const FName FYvrKeyNames::Yvr_Right_Thumbstick_Down("Yvr_Right_Thumbstick_Down");
const FName FYvrKeyNames::Yvr_Right_Thumbstick_Left("Yvr_Right_Thumbstick_Left");
const FName FYvrKeyNames::Yvr_Right_Thumbstick_Right("Yvr_Right_Thumbstick_Right");
const FName FYvrKeyNames::Yvr_Left_Thumbstick_2D("Yvr_Left_Thumbstick_2D");
const FName FYvrKeyNames::Yvr_Right_Thumbstick_2D("Yvr_Right_Thumbstick_2D");

const float TriggerThreshold = 0.5f;

namespace OpenXRSourceNames
{
	static const FName AnyHand("AnyHand");
	static const FName Left("Left");
	static const FName Right("Right");
	static const FName LeftGrip("LeftGrip");
	static const FName RightGrip("RightGrip");
	static const FName LeftAim("LeftAim");
	static const FName RightAim("RightAim");
}

FORCEINLINE XrPath GetPath(XrInstance Instance, const char* PathString)
{
	XrPath Path = XR_NULL_PATH;
	XrResult Result = xrStringToPath(Instance, PathString, &Path);
	check(XR_SUCCEEDED(Result));
	return Path;
}

FORCEINLINE XrPath GetPath(XrInstance Instance, const FString& PathString)
{
	return GetPath(Instance, (ANSICHAR*)StringCast<ANSICHAR>(*PathString).Get());
}

FORCEINLINE void FilterActionName(const char* InActionName, char* OutActionName)
{
	// Ensure the action name is a well-formed path
	size_t i;
	for (i = 0; i < XR_MAX_ACTION_NAME_SIZE - 1 && InActionName[i] != '\0'; i++)
	{
		unsigned char c = InActionName[i];
		OutActionName[i] = (c == ' ') ? '-' : isalnum(c) ? tolower(c) : '_';
	}
	OutActionName[i] = '\0';
}

TSharedPtr< class IInputDevice > FYvrXRInputPlugin::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	if (InputDevice)
		InputDevice->SetMessageHandler(InMessageHandler);
	return InputDevice;
}

IMPLEMENT_MODULE(FYvrXRInputPlugin, YvrXRInput)

FYvrXRInputPlugin::FYvrXRInputPlugin()
	: InputDevice()
{
}

FYvrXRInputPlugin::~FYvrXRInputPlugin()
{
}

FYvrXRHMD* FYvrXRInputPlugin::GetOpenXRHMD() const
{
	static FName SystemName(TEXT("YvrXRHMD"));
	if (GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == SystemName))
	{
		return static_cast<FYvrXRHMD*>(GEngine->XRSystem.Get());
	}

	return nullptr;
}

void FYvrXRInputPlugin::StartupModule()
{
	IYvrXRInputPlugin::StartupModule();

	FYvrXRHMD* OpenXRHMD = GetOpenXRHMD();
	// Note: OpenXRHMD may be null, for example in the editor.  But we still need the input device to enumerate sources.
	InputDevice = MakeShared<FYvrXRInput>(OpenXRHMD);
}

FYvrXRInputPlugin::FOpenXRAction::FOpenXRAction(XrActionSet InActionSet, XrActionType InActionType, const FName& InName)
	: Set(InActionSet)
	, Type(InActionType)
	, Name(InName)
	, Handle(XR_NULL_HANDLE)
{
	char ActionName[NAME_SIZE];
	Name.GetPlainANSIString(ActionName);

	XrActionCreateInfo Info;
	Info.type = XR_TYPE_ACTION_CREATE_INFO;
	Info.next = nullptr;
	FilterActionName(ActionName, Info.actionName);
	Info.actionType = Type;
	Info.countSubactionPaths = 0;
	Info.subactionPaths = nullptr;
;	FCStringAnsi::Strcpy(Info.localizedActionName, XR_MAX_LOCALIZED_ACTION_NAME_SIZE, ActionName);
	XR_ENSURE(xrCreateAction(Set, &Info, &Handle));
}

FYvrXRInputPlugin::FOpenXRController::FOpenXRController(XrActionSet InActionSet, XrPath InUserPath, const char* InName)
	: ActionSet(InActionSet)
	, UserPath(InUserPath)
	, GripAction(XR_NULL_HANDLE)
	, AimAction(XR_NULL_HANDLE)
	, VibrationAction(XR_NULL_HANDLE)
	, GripDeviceId(-1)
	, AimDeviceId(-1)
{
	XrActionCreateInfo Info;
	Info.type = XR_TYPE_ACTION_CREATE_INFO;
	Info.next = nullptr;
	Info.countSubactionPaths = 0;
	Info.subactionPaths = nullptr;

	FCStringAnsi::Strcpy(Info.localizedActionName, XR_MAX_ACTION_NAME_SIZE, InName);
	FCStringAnsi::Strcat(Info.localizedActionName, XR_MAX_ACTION_NAME_SIZE, " Grip Pose");
	FilterActionName(Info.localizedActionName, Info.actionName);
	Info.actionType = XR_ACTION_TYPE_POSE_INPUT;
	XR_ENSURE(xrCreateAction(ActionSet, &Info, &GripAction));

	FCStringAnsi::Strcpy(Info.localizedActionName, XR_MAX_ACTION_NAME_SIZE, InName);
	FCStringAnsi::Strcat(Info.localizedActionName, XR_MAX_ACTION_NAME_SIZE, " Aim Pose");
	FilterActionName(Info.localizedActionName, Info.actionName);
	Info.actionType = XR_ACTION_TYPE_POSE_INPUT;
	XR_ENSURE(xrCreateAction(ActionSet, &Info, &AimAction));

	FCStringAnsi::Strcpy(Info.localizedActionName, XR_MAX_ACTION_NAME_SIZE, InName);
	FCStringAnsi::Strcat(Info.localizedActionName, XR_MAX_ACTION_NAME_SIZE, " Vibration");
	FilterActionName(Info.localizedActionName, Info.actionName);
	Info.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
	XR_ENSURE(xrCreateAction(ActionSet, &Info, &VibrationAction));
}

void FYvrXRInputPlugin::FOpenXRController::AddActionDevices(FYvrXRHMD* HMD)
{
	if (HMD)
	{
		GripDeviceId = HMD->AddActionDevice(GripAction, UserPath);
		AimDeviceId = HMD->AddActionDevice(AimAction, UserPath);
	}
}

FYvrXRInputPlugin::FInteractionProfile::FInteractionProfile(XrPath InProfile, bool InHasHaptics)
	: HasHaptics(InHasHaptics)
	, Path(InProfile)
	, Bindings()
{
}

FYvrXRInputPlugin::FYvrXRInput::FYvrXRInput(FYvrXRHMD* HMD)
	: OpenXRHMD(HMD)
	, ActionSets()
	, ControllerActions()
	, Controllers()
	, bActionsBound(false)
	, MessageHandler(new FGenericApplicationMessageHandler())
{
	AddKeysToEngine();
	IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);

	CachedControllerState[(int32)EControllerHand::Left] = FYvrControllerState(EControllerHand::Left);
	CachedControllerState[(int32)EControllerHand::Right] = FYvrControllerState(EControllerHand::Right);

	// If there is no HMD then this module is not active, but it still needs to exist so we can EnumerateMotionSources from it.
	if (OpenXRHMD)
	{
		// Note: AnyHand needs special handling because it tries left then falls back to right in each call.
		MotionSourceToControllerHandMap.Add(OpenXRSourceNames::Left, EControllerHand::Left);
		MotionSourceToControllerHandMap.Add(OpenXRSourceNames::Right, EControllerHand::Right);
		MotionSourceToControllerHandMap.Add(OpenXRSourceNames::LeftGrip, EControllerHand::Left);
		MotionSourceToControllerHandMap.Add(OpenXRSourceNames::RightGrip, EControllerHand::Right);
		MotionSourceToControllerHandMap.Add(OpenXRSourceNames::LeftAim, EControllerHand::Left);
		MotionSourceToControllerHandMap.Add(OpenXRSourceNames::RightAim, EControllerHand::Right);

		// Map the legacy hand enum values that openxr supports
		MotionSourceToControllerHandMap.Add(TEXT("EControllerHand::Left"), EControllerHand::Left);
		MotionSourceToControllerHandMap.Add(TEXT("EControllerHand::Right"), EControllerHand::Right);
		MotionSourceToControllerHandMap.Add(TEXT("EControllerHand::AnyHand"), EControllerHand::AnyHand);

		BuildActions();
	}
}

XrAction FYvrXRInputPlugin::FYvrXRInput::GetActionForMotionSource(FName MotionSource) const
{
	const FOpenXRController& Controller = Controllers[MotionSourceToControllerHandMap.FindChecked(MotionSource)];
	if (MotionSource == OpenXRSourceNames::LeftAim || MotionSource == OpenXRSourceNames::RightAim)
	{
		return Controller.AimAction;
	}
	else
	{
		return Controller.GripAction;
	}
}

int32 FYvrXRInputPlugin::FYvrXRInput::GetDeviceIDForMotionSource(FName MotionSource) const
{
	const FOpenXRController& Controller = Controllers[MotionSourceToControllerHandMap.FindChecked(MotionSource)];
	if (MotionSource == OpenXRSourceNames::LeftAim || MotionSource == OpenXRSourceNames::RightAim)
	{
		return Controller.AimDeviceId;
	}
	else
	{
		return Controller.GripDeviceId;
	}
}

XrPath FYvrXRInputPlugin::FYvrXRInput::GetUserPathForMotionSource(FName MotionSource) const
{
	const FOpenXRController& Controller = Controllers[MotionSourceToControllerHandMap.FindChecked(MotionSource)];
	return Controller.UserPath;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsOpenXRInputSupportedMotionSource(const FName MotionSource) const
{
	return MotionSource == OpenXRSourceNames::AnyHand || MotionSourceToControllerHandMap.Contains(MotionSource);
}

FYvrXRInputPlugin::FYvrXRInput::~FYvrXRInput()
{
	DestroyActions();
}

void FYvrXRInputPlugin::FYvrXRInput::BuildActions()
{
	if ((bActionsBound) || (OpenXRHMD == nullptr))
	{
		return;
	}
	

	XrInstance Instance = OpenXRHMD->GetInstance();
	check(Instance);
	DestroyActions();

	XrActionSet ActionSet = XR_NULL_HANDLE;
	XrActionSetCreateInfo SetInfo;
	SetInfo.type = XR_TYPE_ACTION_SET_CREATE_INFO;
	SetInfo.next = nullptr;
	FCStringAnsi::Strcpy(SetInfo.actionSetName, XR_MAX_ACTION_SET_NAME_SIZE, "ue4");
	FCStringAnsi::Strcpy(SetInfo.localizedActionSetName, XR_MAX_ACTION_SET_NAME_SIZE, "Unreal Engine 4");
	SetInfo.priority = 0;
	XR_ENSURE(xrCreateActionSet(Instance, &SetInfo, &ActionSet));

	XrPath LeftHand = GetPath(Instance, "/user/hand/left");
	XrPath RightHand = GetPath(Instance, "/user/hand/right");

	// Controller poses
	OpenXRHMD->ResetActionDevices();
	Controllers.Add(EControllerHand::Left, FOpenXRController(ActionSet, LeftHand, "Left Controller"));
	Controllers.Add(EControllerHand::Right, FOpenXRController(ActionSet, RightHand, "Right Controller"));

	// Generate a map of all supported interaction profiles
	XrPath SimpleControllerPath = GetPath(Instance, "/interaction_profiles/khr/simple_controller");
	Profiles.Add("SimpleController", FInteractionProfile(SimpleControllerPath, true));
	Profiles.Add("Yvr", FInteractionProfile(GetPath(Instance, "/interaction_profiles/yvr/touch_controller"), true));

	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_X_Click, "/user/hand/left/input/x/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Y_Click, "/user/hand/left/input/y/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Menu_Click, "/user/hand/left/input/menu/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Grip_Click, "/user/hand/left/input/squeeze/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Thumbstick_Click, "/user/hand/left/input/thumbstick/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_X_Touch, "/user/hand/left/input/x/touch");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Y_Touch, "/user/hand/left/input/y/touch");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Trigger_Touch, "/user/hand/left/input/trigger/touch");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Thumbstick_Touch, "/user/hand/left/input/thumbstick/touch");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_FLOAT_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Trigger_Axis, "/user/hand/left/input/trigger/value");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_FLOAT_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Thumbstick_X, "/user/hand/left/input/thumbstick/x");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_FLOAT_INPUT, EControllerHand::Left, FYvrKeyNames::Yvr_Left_Thumbstick_Y, "/user/hand/left/input/thumbstick/y");

	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_A_Click, "/user/hand/right/input/a/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_B_Click, "/user/hand/right/input/b/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_System_Click, "/user/hand/right/input/system/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_Grip_Click, "/user/hand/right/input/squeeze/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_Thumbstick_Click, "/user/hand/right/input/thumbstick/click");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_A_Touch, "/user/hand/right/input/a/touch");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_B_Touch, "/user/hand/right/input/b/touch");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_Trigger_Touch, "/user/hand/right/input/trigger/touch");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_Thumbstick_Touch, "/user/hand/right/input/thumbstick/touch");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_FLOAT_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_Trigger_Axis, "/user/hand/right/input/trigger/value");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_FLOAT_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_Thumbstick_X, "/user/hand/right/input/thumbstick/x");
	AddYvrAction(Instance, ActionSet, XR_ACTION_TYPE_FLOAT_INPUT, EControllerHand::Right, FYvrKeyNames::Yvr_Right_Thumbstick_Y, "/user/hand/right/input/thumbstick/y");

	for (TPair<FString, FInteractionProfile>& Pair : Profiles)
	{
		FInteractionProfile& Profile = Pair.Value;

		// Only suggest interaction profile bindings if the developer has provided bindings for them
		// An exception is made for the Simple Controller Profile which is always bound as a fallback
		if (Profile.Bindings.Num() > 0 || Profile.Path == SimpleControllerPath)
		{
			// Add the bindings for the controller pose and haptics
			Profile.Bindings.Add(XrActionSuggestedBinding {
				Controllers[EControllerHand::Left].GripAction, GetPath(Instance, "/user/hand/left/input/grip/pose")
				});
			Profile.Bindings.Add(XrActionSuggestedBinding {
				Controllers[EControllerHand::Right].GripAction, GetPath(Instance, "/user/hand/right/input/grip/pose")
				});
			Profile.Bindings.Add(XrActionSuggestedBinding{
				Controllers[EControllerHand::Left].AimAction, GetPath(Instance, "/user/hand/left/input/aim/pose")
				});
			Profile.Bindings.Add(XrActionSuggestedBinding{
				Controllers[EControllerHand::Right].AimAction, GetPath(Instance, "/user/hand/right/input/aim/pose")
				});

			if (Profile.HasHaptics)
			{
				Profile.Bindings.Add(XrActionSuggestedBinding{
					Controllers[EControllerHand::Left].VibrationAction, GetPath(Instance, "/user/hand/left/output/haptic")
					});
				Profile.Bindings.Add(XrActionSuggestedBinding{
					Controllers[EControllerHand::Right].VibrationAction, GetPath(Instance, "/user/hand/right/output/haptic")
					});
			}

			XrInteractionProfileSuggestedBinding InteractionProfile;
			InteractionProfile.type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING;
			InteractionProfile.next = nullptr;
			InteractionProfile.interactionProfile = Profile.Path;
			InteractionProfile.countSuggestedBindings = Profile.Bindings.Num();
			InteractionProfile.suggestedBindings = Profile.Bindings.GetData();
			XR_ENSURE(xrSuggestInteractionProfileBindings(Instance, &InteractionProfile));
		}
	}

	Controllers[EControllerHand::Left].AddActionDevices(OpenXRHMD);
	Controllers[EControllerHand::Right].AddActionDevices(OpenXRHMD);

	{
		XrActiveActionSet ActiveSet;
		ActiveSet.actionSet = ActionSet;
		ActiveSet.subactionPath = XR_NULL_PATH;
		ActionSets.Add(ActiveSet);
	}

	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrCreateHandTrackerEXT", (PFN_xrVoidFunction*)&xrCreateHandTrackerEXT));
	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrLocateHandJointsEXT", (PFN_xrVoidFunction*)&xrLocateHandJointsEXT));
	XR_ENSURE(xrGetInstanceProcAddr(Instance, "xrDestroyHandTrackerEXT", (PFN_xrVoidFunction*)&xrDestroyHandTrackerEXT));
}

void FYvrXRInputPlugin::FYvrXRInput::BuildHands()
{
	if (OpenXRHMD == nullptr && !OpenXRHMD->IsHandTrackingEnabled())
	{
		return;
	}

	XrSession Session = OpenXRHMD->GetSession();

	// Create a hand tracker for left hand that tracks default set of hand joints.
	FYvrXRHandState& LeftHandState = GetLeftHandState();
	{
		XrHandTrackerCreateInfoEXT CreateInfo{ XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT };
		CreateInfo.hand = XR_HAND_LEFT_EXT;
		CreateInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;

		XR_ENSURE(xrCreateHandTrackerEXT(Session, &CreateInfo, &LeftHandState.HandTracker));
	}

	// Create a hand tracker for left hand that tracks default set of hand joints.
	FYvrXRHandState& RightHandState = GetRightHandState();
	{
		XrHandTrackerCreateInfoEXT CreateInfo{ XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT };
		CreateInfo.hand = XR_HAND_RIGHT_EXT;
		CreateInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;
		XR_ENSURE(xrCreateHandTrackerEXT(Session, &CreateInfo, &RightHandState.HandTracker));
	}
}

void FYvrXRInputPlugin::FYvrXRInput::DestroyActions()
{
	// Destroying an action set will also destroy all actions in the set
	for (XrActiveActionSet& ActionSet : ActionSets)
	{
		xrDestroyActionSet(ActionSet.actionSet);
	}

	for (TArray<FOpenXRAction> Actions : ControllerActions)
	{
		Actions.Reset();
	}

	Controllers.Reset();
	ActionSets.Reset();
}

void FYvrXRInputPlugin::FYvrXRInput::AddKeysToEngine()
{
	// Register the FKeys
	//Button
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_A_Click, LOCTEXT("Yvr_Right_A_Click", "Yvr Button (R) A"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_B_Click, LOCTEXT("Yvr_Right_B_Click", "Yvr Button (R) B"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_X_Click, LOCTEXT("Yvr_Left_X_Click", "Yvr Button (L) X"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Y_Click, LOCTEXT("Yvr_Left_Y_Click", "Yvr Button (L) Y"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Menu_Click, LOCTEXT("Yvr_Left_Menu_Click", "Yvr Button (L) Start"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_System_Click, LOCTEXT("Yvr_Right_System_Click", "Yvr Button (R) Reserved"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Trigger_Click, LOCTEXT("Yvr_Left_Trigger_Click", "Yvr Button (L) Index Trigger"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Trigger_Click, LOCTEXT("Yvr_Right_Trigger_Click", "Yvr Button (R) Index Trigger"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Grip_Click, LOCTEXT("Yvr_Left_Grip_Click", "Yvr Button (L) Hand Trigger"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Grip_Click, LOCTEXT("Yvr_Right_Grip_Click", "Yvr Button (R) Hand Trigger"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Thumbstick_Click, LOCTEXT("Yvr_Left_Thumbstick_Click", "Yvr Button (L) Thumbstick"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Thumbstick_Click, LOCTEXT("Yvr_Right_Thumbstick_Click", "Yvr Button (R) Thumbstick"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Thumbstick_Up, LOCTEXT("Yvr_Left_Thumbstick_Up", "Yvr Button (L) Thumbstick Up"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Thumbstick_Down, LOCTEXT("Yvr_Left_Thumbstick_Down", "Yvr Button (L) Thumbstick Down"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Thumbstick_Left, LOCTEXT("Yvr_Left_Thumbstick_Left", "Yvr Button (L) Thumbstick Left"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Thumbstick_Right, LOCTEXT("Yvr_Left_Thumbstick_Right", "Yvr Button (L) Thumbstick Right"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Thumbstick_Up, LOCTEXT("Yvr_Right_Thumbstick_Up", "Yvr Button (R) Thumbstick Up"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Thumbstick_Down, LOCTEXT("Yvr_Right_Thumbstick_Down", "Yvr Button (R) Thumbstick Down"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Thumbstick_Left, LOCTEXT("Yvr_Right_Thumbstick_Left", "Yvr Button (R) Thumbstick Left"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Thumbstick_Right, LOCTEXT("Yvr_Right_Thumbstick_Right", "Yvr Button (R) Thumbstick Right"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	//Touch
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_A_Touch, LOCTEXT("Yvr_Right_A_Touch", "Yvr Touch (R) A"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_B_Touch, LOCTEXT("Yvr_Right_B_Touch", "Yvr Touch (R) B"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_X_Touch, LOCTEXT("Yvr_Left_X_Touch", "Yvr Touch (L) X"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Y_Touch, LOCTEXT("Yvr_Left_Y_Touch", "Yvr Touch (L) Y"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Trigger_Touch, LOCTEXT("Yvr_Left_Trigger_Touch", "Yvr Touch (L) Index Trigger"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Trigger_Touch, LOCTEXT("Yvr_Right_Trigger_Touch", "Yvr Touch (R) Index Trigger"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Thumbstick_Touch, LOCTEXT("Yvr_Left_Thumbstick_Touch", "Yvr Touch (L) Thumbstick"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Thumbstick_Touch, LOCTEXT("Yvr_Right_Thumbstick_Touch", "Yvr Touch (R) Thumbstick"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	//Analog
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Trigger_Axis, LOCTEXT("Yvr_Left_Trigger_Axis", "Yvr Button (L) Index Trigger Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Thumbstick_X, LOCTEXT("Yvr_Left_Thumbstick_X", "Yvr Button (L) Thumbstick X"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Left_Thumbstick_Y, LOCTEXT("Yvr_Left_Thumbstick_Y", "Yvr Button (L) Thumbstick Y"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Trigger_Axis, LOCTEXT("Yvr_Right_Trigger_Axis", "Yvr Button (R) Index Trigger Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Thumbstick_X, LOCTEXT("Yvr_Right_Thumbstick_X", "Yvr Button (R) Thumbstick X"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddKey(FKeyDetails(FYvrKey::Yvr_Right_Thumbstick_Y, LOCTEXT("Yvr_Right_Thumbstick_Y", "Yvr Button (R) Thumbstick Y"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D | FKeyDetails::NotBlueprintBindableKey, "Yvr"));
	EKeys::AddPairedKey(FKeyDetails(FYvrKey::Yvr_Left_Thumbstick_2D, LOCTEXT("Yvr_Left_Thumbstick_2D", "Yvr Button (L) Thumbstick 2D-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis2D | FKeyDetails::NotBlueprintBindableKey, "Yvr"), FYvrKey::Yvr_Left_Thumbstick_X, FYvrKey::Yvr_Left_Thumbstick_Y);
	EKeys::AddPairedKey(FKeyDetails(FYvrKey::Yvr_Right_Thumbstick_2D, LOCTEXT("Yvr_Right_Thumbstick_2D", "Yvr Button (R) Thumbstick 2D-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis2D | FKeyDetails::NotBlueprintBindableKey, "Yvr"), FYvrKey::Yvr_Right_Thumbstick_X, FYvrKey::Yvr_Right_Thumbstick_Y);
}

void FYvrXRInputPlugin::FYvrXRInput::AddYvrAction(XrInstance Instance, XrActionSet ActionSet, XrActionType ActionType, EControllerHand Hand, const FName& Name, const FString& ActionPath, bool bIsTrigger)
{
	FOpenXRAction Action(ActionSet, ActionType, Name);
	FInteractionProfile* Profile = Profiles.Find("Yvr");
	Profile->Bindings.Add(XrActionSuggestedBinding{ Action.Handle, GetPath(Instance, ActionPath) });
	ControllerActions[(int32)Hand].Add(Action);
}

void FYvrXRInputPlugin::FYvrXRInput::UpdateHandState()
{
	if (OpenXRHMD == nullptr && !OpenXRHMD->IsHandTrackingEnabled())
	{
		return;
	}

	XrSpace TrackingSpace = OpenXRHMD->GetTrackingSpace();
	XrTime DisplayTime = OpenXRHMD->GetDisplayTime();

	XrHandJointsLocateInfoEXT LocateInfo{ XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT };
	LocateInfo.baseSpace = TrackingSpace;
	LocateInfo.time = DisplayTime;

	const float WorldToMetersScale = OpenXRHMD->GetWorldToMetersScale();

	for (int i = 0; i < 2; ++i)
	{
		FYvrXRHandState& HandState = HandStates[i];

		XR_ENSURE(xrLocateHandJointsEXT(HandState.HandTracker, &LocateInfo, &HandState.Locations));

		HandState.ReceivedJointPoses = HandState.Locations.isActive == XR_TRUE;
		if (HandState.ReceivedJointPoses) {

			HandState.HandScale = HandState.Scale.sensorOutput;

			static_assert(XR_HAND_JOINT_PALM_EXT == 0 && XR_HAND_JOINT_LITTLE_TIP_EXT == XR_HAND_JOINT_COUNT_EXT - 1, "XrHandJointEXT enum is not as assumed for the following loop!");
			for (int j = 0; j < XR_HAND_JOINT_COUNT_EXT; ++j)
			{
				const XrHandJointLocationEXT& JoinLocation = HandState.JointLocations[j];
				const XrPosef& Pose = JoinLocation.pose;
				const FTransform Transform = ToFTransform(Pose, WorldToMetersScale);
				HandState.KeypointTransforms[j] = Transform;
				HandState.Radii[j] = JoinLocation.radius * WorldToMetersScale;
				// Todo: Velocity

			}
		}
	}

}

FYvrXRInputPlugin::FYvrXRHandState& FYvrXRInputPlugin::FYvrXRInput::GetLeftHandState()
{
	return HandStates[0];
}

FYvrXRInputPlugin::FYvrXRHandState& FYvrXRInputPlugin::FYvrXRInput::GetRightHandState()
{
	return HandStates[1];
}

const FYvrXRInputPlugin::FYvrXRHandState& FYvrXRInputPlugin::FYvrXRInput::GetLeftHandState() const
{
	return HandStates[0];
}

const FYvrXRInputPlugin::FYvrXRHandState& FYvrXRInputPlugin::FYvrXRInput::GetRightHandState() const
{
	return HandStates[1];
}

void FYvrXRInputPlugin::FYvrXRInput::Tick(float DeltaTime)
{
	if (OpenXRHMD == nullptr)
	{
		// In the editor, when we are not actually running OpenXR, but the IInputDevice exists so it can enumerate its motion sources.
		return;
	}

	XrSession Session = OpenXRHMD->GetSession();
	if (Session != XR_NULL_HANDLE)
	{
		if (!bActionsBound)
		{
			TArray<XrActionSet> BindActionSets;
			for (auto && BindActionSet : ActionSets)
				BindActionSets.Add(BindActionSet.actionSet);

			XrSessionActionSetsAttachInfo SessionActionSetsAttachInfo;
			SessionActionSetsAttachInfo.type = XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO;
			SessionActionSetsAttachInfo.next = nullptr;
			SessionActionSetsAttachInfo.countActionSets = BindActionSets.Num();
			SessionActionSetsAttachInfo.actionSets = BindActionSets.GetData();
			XR_ENSURE(xrAttachSessionActionSets(Session, &SessionActionSetsAttachInfo));

			BuildHands();

			bActionsBound = true;
		}
	}
	else if (bActionsBound)
	{
		// If the session shut down, clean up.
		bActionsBound = false;
	}

	if (OpenXRHMD->IsFocused())
	{
		XrActionsSyncInfo SyncInfo;
		SyncInfo.type = XR_TYPE_ACTIONS_SYNC_INFO;
		SyncInfo.next = nullptr;
		SyncInfo.countActiveActionSets = ActionSets.Num();
		SyncInfo.activeActionSets = ActionSets.GetData();
		XR_ENSURE(xrSyncActions(Session, &SyncInfo));
	}
}

namespace OpenXRInputNamespace
{
	FXRTimedInputActionDelegate* GetTimedInputActionDelegate(FName ActionName)
	{
		FXRTimedInputActionDelegate* XRTimedInputActionDelegate = UHeadMountedDisplayFunctionLibrary::OnXRTimedInputActionDelegateMap.Find(ActionName);
		if (XRTimedInputActionDelegate && !XRTimedInputActionDelegate->IsBound())
		{
			XRTimedInputActionDelegate = nullptr;
		}
		return XRTimedInputActionDelegate;
	}
}

void FYvrXRInputPlugin::FYvrXRInput::SendControllerEvents()
{
	if (!bActionsBound)
	{
		return;
	}

	if (OpenXRHMD == nullptr)
	{
		return;
	}

	XrSession Session = OpenXRHMD->GetSession();

	for (int32 ControllerIndex = 0; ControllerIndex < 2; ++ControllerIndex)
	{
		uint32_t RemoteButton = 0;
		float RemoteIndexTrigger = 0.0f;
		float RemoteThumbstickX = 0.0f;
		float RemoteThumbstickY = 0.0f;

		// Controller
		for(int32 ActionIndex = 0; ActionIndex < ControllerActions[ControllerIndex].Num(); ++ActionIndex)
		{
			FOpenXRAction& Action = ControllerActions[ControllerIndex][ActionIndex];
			XrActionStateGetInfo GetInfo;
			GetInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
			GetInfo.next = nullptr;
			GetInfo.subactionPath = XR_NULL_PATH;
			GetInfo.action = Action.Handle;

			FName* ActionKey = &Action.Name;
			switch (Action.Type)
			{
			case XR_ACTION_TYPE_BOOLEAN_INPUT:
			{
				XrActionStateBoolean State;
				State.type = XR_TYPE_ACTION_STATE_BOOLEAN;
				State.next = nullptr;
				XrResult Result = xrGetActionStateBoolean(Session, &GetInfo, &State);
				if (XR_SUCCEEDED(Result))
				{
					if (State.isActive && State.currentState)
					{
						RemoteButton |= (1 << ActionIndex);
					}
				}
			}
			break;
			case XR_ACTION_TYPE_FLOAT_INPUT:
			{
				XrActionStateFloat State;
				State.type = XR_TYPE_ACTION_STATE_FLOAT;
				State.next = nullptr;
				XrResult Result = xrGetActionStateFloat(Session, &GetInfo, &State);
				if (XR_SUCCEEDED(Result))
				{
					if (State.isActive)
					{
						// Trigger Axis value
						if (*ActionKey == FYvrKeyNames::Yvr_Left_Trigger_Axis || *ActionKey == FYvrKeyNames::Yvr_Right_Trigger_Axis)
						{
							RemoteIndexTrigger = State.currentState;
						}
						else if (*ActionKey == FYvrKeyNames::Yvr_Left_Thumbstick_X || *ActionKey == FYvrKeyNames::Yvr_Right_Thumbstick_X)
						{
							RemoteThumbstickX = State.currentState;
						}
						else if (*ActionKey == FYvrKeyNames::Yvr_Left_Thumbstick_Y || *ActionKey == FYvrKeyNames::Yvr_Right_Thumbstick_Y)
						{
							RemoteThumbstickY = State.currentState;
						}
					}
				}
			}
			break;
			default:
				// Other action types are currently unsupported.
				break;
			}
		}

		FYvrControllerState& State = CachedControllerState[ControllerIndex];
		if (RemoteIndexTrigger != State.TriggerAxis)
		{
			State.TriggerAxis = RemoteIndexTrigger;
			MessageHandler->OnControllerAnalog(ControllerIndex ? FYvrKeyNames::Yvr_Right_Trigger_Axis : FYvrKeyNames::Yvr_Left_Trigger_Axis, 0, State.TriggerAxis);
			UE_LOG(LogHMD, Log, TEXT("Analog IndexTriggerAxis %f "), State.TriggerAxis);
		}

		float ThumbstickValueX = RemoteThumbstickX;
		float ThumbstickValueY = RemoteThumbstickY;

		if (ThumbstickValueX != State.ThumbstickAxes.X)
		{
			State.ThumbstickAxes.X = ThumbstickValueX;
			MessageHandler->OnControllerAnalog(ControllerIndex ? FYvrKeyNames::Yvr_Right_Thumbstick_X : FYvrKeyNames::Yvr_Left_Thumbstick_X, 0, State.ThumbstickAxes.X);
			UE_LOG(LogHMD, Log, TEXT("Analog ThumbstickAxisX %f "), State.ThumbstickAxes.X);
		}

		if (ThumbstickValueY != State.ThumbstickAxes.Y)
		{
			State.ThumbstickAxes.Y = ThumbstickValueY;
			MessageHandler->OnControllerAnalog(ControllerIndex ? FYvrKeyNames::Yvr_Right_Thumbstick_Y : FYvrKeyNames::Yvr_Left_Thumbstick_Y, 0, State.ThumbstickAxes.Y);
			UE_LOG(LogHMD, Log, TEXT("Analog ThumbstickAxisY %f "), State.ThumbstickAxes.Y);
		}

		for (int32 ButtonIndex = 0; ButtonIndex < (int32)EYvrControllerButton::TotalButtonCount; ++ButtonIndex)
		{

			FYvrButtonState& ButtonState = State.Buttons[ButtonIndex];

			bool bButtonPressed = false;

			switch ((EYvrControllerButton)ButtonIndex)
			{
			case EYvrControllerButton::Trigger:
				bButtonPressed = RemoteIndexTrigger >= TriggerThreshold;
				break;

			case EYvrControllerButton::Grip:
				bButtonPressed = (RemoteButton & (1 << (uint32)EYvrRemoteControllerButton::Grip)) != 0;
				break;

			case EYvrControllerButton::XA:
				bButtonPressed = (RemoteButton & (1 << (uint32)EYvrRemoteControllerButton::XA)) != 0;
				break;

			case EYvrControllerButton::YB:
				bButtonPressed = (RemoteButton & (1 << (uint32)EYvrRemoteControllerButton::YB)) != 0;
				break;

			case EYvrControllerButton::Thumbstick:
				bButtonPressed = (RemoteButton & (1 << (uint32)EYvrRemoteControllerButton::Thumbstick)) != 0;
				break;

			case EYvrControllerButton::Thumbstick_Up:
				if (State.Buttons[(int)EYvrControllerButton::Thumbstick].bIsPressed && State.ThumbstickAxes.Size() > 0.5f)
				{
					float Angle = FMath::Atan2(State.ThumbstickAxes.Y, State.ThumbstickAxes.X);
					bButtonPressed = Angle >= (1.0f / 8.0f) * PI && Angle <= (7.0f / 8.0f) * PI;
				}
				break;

			case EYvrControllerButton::Thumbstick_Down:
				if (State.Buttons[(int)EYvrControllerButton::Thumbstick].bIsPressed && State.ThumbstickAxes.Size() > 0.5f)
				{
					float Angle = FMath::Atan2(State.ThumbstickAxes.Y, State.ThumbstickAxes.X);
					bButtonPressed = Angle >= (-7.0f / 8.0f) * PI && Angle <= (-1.0f / 8.0f) * PI;
				}
				break;

			case EYvrControllerButton::Thumbstick_Left:
				if (State.Buttons[(int)EYvrControllerButton::Thumbstick].bIsPressed && State.ThumbstickAxes.Size() > 0.5f)
				{
					float Angle = FMath::Atan2(State.ThumbstickAxes.Y, State.ThumbstickAxes.X);
					bButtonPressed = Angle <= (-5.0f / 8.0f) * PI || Angle >= (5.0f / 8.0f) * PI;
				}
				break;

			case EYvrControllerButton::Thumbstick_Right:
				if (State.Buttons[(int)EYvrControllerButton::Thumbstick].bIsPressed && State.ThumbstickAxes.Size() > 0.5f)
				{
					float Angle = FMath::Atan2(State.ThumbstickAxes.Y, State.ThumbstickAxes.X);
					bButtonPressed = Angle >= (-3.0f / 8.0f) * PI && Angle <= (3.0f / 8.0f) * PI;
				}
				break;

			case EYvrControllerButton::Menu:
				bButtonPressed = (RemoteButton & (1 << (uint32)EYvrRemoteControllerButton::Menu));
				break;

			case EYvrControllerButton::Thumbstick_Touch:
				bButtonPressed = (RemoteButton & (1 << (uint32)EYvrRemoteControllerButton::Thumbstick_Touch));
				break;

			case EYvrControllerButton::Trigger_Touch:
				bButtonPressed = (RemoteButton & (1 << (uint32)EYvrRemoteControllerButton::Trigger_Touch));
				break;

			case EYvrControllerButton::XA_Touch:
				bButtonPressed = (RemoteButton & (1 << (uint32)EYvrRemoteControllerButton::XA_Touch));
				break;

			case EYvrControllerButton::YB_Touch:
				bButtonPressed = (RemoteButton & (1 << (uint32)EYvrRemoteControllerButton::YB_Touch));
				break;

			default:
				check(0);
				break;
			}

			if (bButtonPressed != ButtonState.bIsPressed)
			{
				ButtonState.bIsPressed = bButtonPressed;

				if (ButtonState.bIsPressed)
				{
					MessageHandler->OnControllerButtonPressed(ButtonState.Key, 0, false);
					UE_LOG(LogHMD, Log, TEXT("Button %s pressed"), *ButtonState.Key.ToString());
				}
				else
				{
					MessageHandler->OnControllerButtonReleased(ButtonState.Key, 0, false);
					UE_LOG(LogHMD, Log, TEXT("Button %s released"), *ButtonState.Key.ToString());
				}
			}
		}
	}

	// Hand
	UpdateHandState();
}

void FYvrXRInputPlugin::FYvrXRInput::SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	MessageHandler = InMessageHandler;
#if WITH_EDITOR
	FEditorDelegates::OnActionAxisMappingsChanged.AddSP(this, &FYvrXRInputPlugin::FYvrXRInput::BuildActions);
#endif
}

bool FYvrXRInputPlugin::FYvrXRInput::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FYvrXRInputPlugin::FYvrXRInput::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	// Large channel type maps to amplitude. We are interested in amplitude.
	if ((ChannelType == FForceFeedbackChannelType::LEFT_LARGE) ||
		(ChannelType == FForceFeedbackChannelType::RIGHT_LARGE))
	{
		FHapticFeedbackValues Values(XR_FREQUENCY_UNSPECIFIED, Value);
		SetHapticFeedbackValues(ControllerId, ChannelType == FForceFeedbackChannelType::LEFT_LARGE ? (int32)EControllerHand::Left : (int32)EControllerHand::Right, Values);
	}
}

void FYvrXRInputPlugin::FYvrXRInput::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values)
{
	FHapticFeedbackValues leftHaptics = FHapticFeedbackValues(
		values.LeftSmall,		// frequency
		values.LeftLarge);		// amplitude
	FHapticFeedbackValues rightHaptics = FHapticFeedbackValues(
		values.RightSmall,		// frequency
		values.RightLarge);		// amplitude

	SetHapticFeedbackValues(
		ControllerId,
		(int32)EControllerHand::Left,
		leftHaptics);

	SetHapticFeedbackValues(
		ControllerId,
		(int32)EControllerHand::Right,
		rightHaptics);
}

FName FYvrXRInputPlugin::FYvrXRInput::GetMotionControllerDeviceTypeName() const
{
	return FName(TEXT("YvrXRInput"));
}


bool FYvrXRInputPlugin::FYvrXRInput::GetControllerOrientationAndPosition(const int32 ControllerIndex, const FName MotionSource, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const
{
	if (OpenXRHMD == nullptr)
	{
		return false;
	}

	if (ControllerIndex == 0 && IsOpenXRInputSupportedMotionSource(MotionSource))
	{
		if (MotionSource == OpenXRSourceNames::AnyHand)
		{
			return GetControllerOrientationAndPosition(ControllerIndex, OpenXRSourceNames::LeftGrip, OutOrientation, OutPosition, WorldToMetersScale)
				|| GetControllerOrientationAndPosition(ControllerIndex, OpenXRSourceNames::RightGrip, OutOrientation, OutPosition, WorldToMetersScale);
		}

		if (MotionSource == OpenXRSourceNames::Left)
		{
			return GetControllerOrientationAndPosition(ControllerIndex, OpenXRSourceNames::LeftGrip, OutOrientation, OutPosition, WorldToMetersScale);
		}

		if (MotionSource == OpenXRSourceNames::Right)
		{
			return GetControllerOrientationAndPosition(ControllerIndex, OpenXRSourceNames::RightGrip, OutOrientation, OutPosition, WorldToMetersScale);
		}

		XrSession Session = OpenXRHMD->GetSession();

		if (Session == XR_NULL_HANDLE)
		{
			return false;
		}

		XrActionStateGetInfo GetInfo;
		GetInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
		GetInfo.next = nullptr;
		GetInfo.subactionPath = XR_NULL_PATH;
		GetInfo.action = GetActionForMotionSource(MotionSource);

		XrActionStatePose State;
		State.type = XR_TYPE_ACTION_STATE_POSE;
		State.next = nullptr;
		XrResult Result = xrGetActionStatePose(Session, &GetInfo, &State);
		if (Result >= XR_SUCCESS && State.isActive)
		{
			FQuat Orientation;
			OpenXRHMD->GetCurrentPose(GetDeviceIDForMotionSource(MotionSource), Orientation, OutPosition);
			OutOrientation = FRotator(Orientation);

			return true;
		}
	}

	return false;
}

bool FYvrXRInputPlugin::FYvrXRInput::GetControllerOrientationAndPosition(const int32 ControllerIndex, const FName MotionSource, FRotator& OutOrientation, FVector& OutPosition, bool& OutbProvidedLinearVelocity, FVector& OutLinearVelocity, bool& OutbProvidedAngularVelocity, FVector& OutAngularVelocityAsAxisAndLength, bool& OutbProvidedLinearAcceleration, FVector& OutLinearAcceleration, float WorldToMetersScale) const
{
	// the implementation at OpenXR, not work
	// FTimespan initializes to 0 and GetControllerOrientationAndPositionForTime with time 0 will return the latest data.
	FTimespan Time;
	bool OutTimeWasUsed = false;
	return GetControllerOrientationAndPositionForTime(ControllerIndex, MotionSource, Time, OutTimeWasUsed, OutOrientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityAsAxisAndLength, OutbProvidedLinearAcceleration, OutLinearAcceleration, WorldToMetersScale);
}

#if ENGINE_MAJOR_VERSION > 4
bool FYvrXRInputPlugin::FYvrXRInput::GetControllerOrientationAndPositionForTime(const int32 ControllerIndex, const FName MotionSource, FTimespan Time, bool& OutTimeWasUsed, FRotator& OutOrientation, FVector& OutPosition, bool& OutbProvidedLinearVelocity, FVector& OutLinearVelocity, bool& OutbProvidedAngularVelocity, FVector& OutAngularVelocityRadPerSec, bool& OutbProvidedLinearAcceleration, FVector& OutLinearAcceleration, float WorldToMetersScale) const
{
	if (OpenXRHMD == nullptr)
	{
		return false;
	}

	if (ControllerIndex == 0 && IsOpenXRInputSupportedMotionSource(MotionSource))
	{
		if (MotionSource == OpenXRSourceNames::AnyHand)
		{
			return GetControllerOrientationAndPositionForTime(ControllerIndex, OpenXRSourceNames::LeftGrip, Time, OutTimeWasUsed, OutOrientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec, OutbProvidedLinearAcceleration, OutLinearAcceleration, WorldToMetersScale)
				|| GetControllerOrientationAndPositionForTime(ControllerIndex, OpenXRSourceNames::RightGrip, Time, OutTimeWasUsed, OutOrientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec, OutbProvidedLinearAcceleration, OutLinearAcceleration, WorldToMetersScale);
		}

		if (MotionSource == OpenXRSourceNames::Left)
		{
			return GetControllerOrientationAndPositionForTime(ControllerIndex, OpenXRSourceNames::LeftGrip, Time, OutTimeWasUsed, OutOrientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec, OutbProvidedLinearAcceleration, OutLinearAcceleration, WorldToMetersScale);
		}

		if (MotionSource == OpenXRSourceNames::Right)
		{
			return GetControllerOrientationAndPositionForTime(ControllerIndex, OpenXRSourceNames::RightGrip, Time, OutTimeWasUsed, OutOrientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec, OutbProvidedLinearAcceleration, OutLinearAcceleration, WorldToMetersScale);
		}
	}

	XrSession Session = OpenXRHMD->GetSession();

	if (Session == XR_NULL_HANDLE)
	{
		return false;
	}

	XrActionStateGetInfo GetInfo;
	GetInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
	GetInfo.next = nullptr;
	GetInfo.subactionPath = XR_NULL_PATH;
	GetInfo.action = GetActionForMotionSource(MotionSource);

	if (GetInfo.action == XR_NULL_HANDLE)
	{
		UE_LOG(LogHMD, Warning, TEXT("GetControllerOrientationAndPositionForTime called with motion source %s which is unknown.  Cannot get pose."), *MotionSource.ToString());

		return false;
	}

	XrActionStatePose State;
	State.type = XR_TYPE_ACTION_STATE_POSE;
	State.next = nullptr;
	XrResult Result = xrGetActionStatePose(Session, &GetInfo, &State);
	if (Result >= XR_SUCCESS && State.isActive)
	{
		FQuat Orientation;
		OpenXRHMD->GetPoseForTime(GetDeviceIDForMotionSource(MotionSource), Time, OutTimeWasUsed, Orientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec, OutbProvidedLinearAcceleration, OutLinearAcceleration, WorldToMetersScale);
		OutOrientation = FRotator(Orientation);
		return true;
	}

	return false;
}
#else
bool FYvrXRInputPlugin::FYvrXRInput::GetControllerOrientationAndPositionForTime(const int32 ControllerIndex, const FName MotionSource, FTimespan Time, bool& OutTimeWasUsed, FRotator& OutOrientation, FVector& OutPosition, bool& OutbProvidedLinearVelocity, FVector& OutLinearVelocity, bool& OutbProvidedAngularVelocity, FVector& OutAngularVelocityRadPerSec, float WorldToMetersScale) const
{
	OutTimeWasUsed = true;

	if (OpenXRHMD == nullptr)
	{
		return false;
	}

	if (ControllerIndex == 0 && IsOpenXRInputSupportedMotionSource(MotionSource))
	{
		if (MotionSource == OpenXRSourceNames::AnyHand)
		{
			return GetControllerOrientationAndPositionForTime(ControllerIndex, OpenXRSourceNames::LeftGrip, Time, OutTimeWasUsed, OutOrientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec, WorldToMetersScale)
				|| GetControllerOrientationAndPositionForTime(ControllerIndex, OpenXRSourceNames::RightGrip, Time, OutTimeWasUsed, OutOrientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec, WorldToMetersScale);
		}

		if (MotionSource == OpenXRSourceNames::Left)
		{
			return GetControllerOrientationAndPositionForTime(ControllerIndex, OpenXRSourceNames::LeftGrip, Time, OutTimeWasUsed, OutOrientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec, WorldToMetersScale);
		}

		if (MotionSource == OpenXRSourceNames::Right)
		{
			return GetControllerOrientationAndPositionForTime(ControllerIndex, OpenXRSourceNames::RightGrip, Time, OutTimeWasUsed, OutOrientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec, WorldToMetersScale);
		}
	}

	XrSession Session = OpenXRHMD->GetSession();

	if (Session == XR_NULL_HANDLE)
	{
		return false;
	}

	XrActionStateGetInfo GetInfo;
	GetInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
	GetInfo.next = nullptr;
	GetInfo.subactionPath = XR_NULL_PATH;
	GetInfo.action = GetActionForMotionSource(MotionSource);

	XrActionStatePose State;
	State.type = XR_TYPE_ACTION_STATE_POSE;
	State.next = nullptr;
	XrResult Result = xrGetActionStatePose(Session, &GetInfo, &State);
	if (Result >= XR_SUCCESS && State.isActive)
	{
		FQuat Orientation;
		OpenXRHMD->GetPoseForTime(GetDeviceIDForMotionSource(MotionSource), Time, Orientation, OutPosition, OutbProvidedLinearVelocity, OutLinearVelocity, OutbProvidedAngularVelocity, OutAngularVelocityRadPerSec);
		OutOrientation = FRotator(Orientation);
		return true;
	}

	return false;
}
#endif

ETrackingStatus FYvrXRInputPlugin::FYvrXRInput::GetControllerTrackingStatus(const int32 ControllerIndex, const FName MotionSource) const
{
	if (OpenXRHMD == nullptr)
	{
		return ETrackingStatus::NotTracked;
	}

	if (ControllerIndex == 0 && IsOpenXRInputSupportedMotionSource(MotionSource))
	{
		if (MotionSource == OpenXRSourceNames::AnyHand)
		{
			if (GetControllerTrackingStatus(ControllerIndex, OpenXRSourceNames::LeftGrip) == ETrackingStatus::Tracked)
			{
				return ETrackingStatus::Tracked;
			}
			else
			{
				return GetControllerTrackingStatus(ControllerIndex, OpenXRSourceNames::RightGrip);
			}
		}

		if (MotionSource == OpenXRSourceNames::Left)
		{
			return GetControllerTrackingStatus(ControllerIndex, OpenXRSourceNames::LeftGrip);
		}

		if (MotionSource == OpenXRSourceNames::Right)
		{
			return GetControllerTrackingStatus(ControllerIndex, OpenXRSourceNames::RightGrip);
		}

		XrSession Session = OpenXRHMD->GetSession();

		if (Session == XR_NULL_HANDLE)
		{
			return ETrackingStatus::NotTracked;
		}

		XrActionStateGetInfo GetInfo;
		GetInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
		GetInfo.next = nullptr;
		GetInfo.subactionPath = XR_NULL_PATH;
		GetInfo.action = GetActionForMotionSource(MotionSource);

		XrActionStatePose State;
		State.type = XR_TYPE_ACTION_STATE_POSE;
		State.next = nullptr;
		XrResult Result = xrGetActionStatePose(Session, &GetInfo, &State);
		if (XR_SUCCEEDED(Result) && State.isActive)
		{
			FQuat Orientation;
			bool bIsTracked = OpenXRHMD->GetIsTracked(GetDeviceIDForMotionSource(MotionSource));
			return bIsTracked ? ETrackingStatus::Tracked : ETrackingStatus::NotTracked;
		}
	}

	return ETrackingStatus::NotTracked;
}

void FYvrXRInputPlugin::FYvrXRInput::EnumerateSources(TArray<FMotionControllerSource>& SourcesOut) const
{
	check(IsInGameThread());

	SourcesOut.Add(OpenXRSourceNames::AnyHand);
	SourcesOut.Add(OpenXRSourceNames::Left);
	SourcesOut.Add(OpenXRSourceNames::Right);
	SourcesOut.Add(OpenXRSourceNames::LeftGrip);
	SourcesOut.Add(OpenXRSourceNames::RightGrip);
	SourcesOut.Add(OpenXRSourceNames::LeftAim);
	SourcesOut.Add(OpenXRSourceNames::RightAim);
}

void FYvrXRInputPlugin::FYvrXRInput::SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values)
{
	if (OpenXRHMD == nullptr)
	{
		return;
	}

	XrSession Session = OpenXRHMD->GetSession();

	if (Session == XR_NULL_HANDLE)
	{
		return;
	}

	XrHapticVibration HapticValue;
	HapticValue.type = XR_TYPE_HAPTIC_VIBRATION;
	HapticValue.next = nullptr;
	HapticValue.duration = MaxFeedbackDuration;
	HapticValue.frequency = Values.Frequency;
	HapticValue.amplitude = Values.Amplitude;

	if (ControllerId == 0)
	{
		if ((Hand == (int32)EControllerHand::Left || Hand == (int32)EControllerHand::AnyHand) && Controllers.Contains(EControllerHand::Left))
		{
			XrHapticActionInfo HapticActionInfo;
			HapticActionInfo.type = XR_TYPE_HAPTIC_ACTION_INFO;
			HapticActionInfo.next = nullptr;
			HapticActionInfo.subactionPath = XR_NULL_PATH;
			HapticActionInfo.action = Controllers[EControllerHand::Left].VibrationAction;
			if (Values.Amplitude <= 0.0f || Values.Frequency < XR_FREQUENCY_UNSPECIFIED)
			{
				XR_ENSURE(xrStopHapticFeedback(Session, &HapticActionInfo));
			}
			else
			{
				XR_ENSURE(xrApplyHapticFeedback(Session, &HapticActionInfo, (const XrHapticBaseHeader*)&HapticValue));
			}
		}
		if ((Hand == (int32)EControllerHand::Right || Hand == (int32)EControllerHand::AnyHand) && Controllers.Contains(EControllerHand::Right))
		{
			XrHapticActionInfo HapticActionInfo;
			HapticActionInfo.type = XR_TYPE_HAPTIC_ACTION_INFO;
			HapticActionInfo.next = nullptr;
			HapticActionInfo.subactionPath = XR_NULL_PATH;
			HapticActionInfo.action = Controllers[EControllerHand::Right].VibrationAction;
			if (Values.Amplitude <= 0.0f || Values.Frequency < XR_FREQUENCY_UNSPECIFIED)
			{
				XR_ENSURE(xrStopHapticFeedback(Session, &HapticActionInfo));
			}
			else
			{
				XR_ENSURE(xrApplyHapticFeedback(Session, &HapticActionInfo, (const XrHapticBaseHeader*)&HapticValue));
			}
		}
	}
}

void FYvrXRInputPlugin::FYvrXRInput::GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const
{
	MinFrequency = XR_FREQUENCY_UNSPECIFIED;
	MaxFrequency = XR_FREQUENCY_UNSPECIFIED;
}

float FYvrXRInputPlugin::FYvrXRInput::GetHapticAmplitudeScale() const
{
	return 1.0f;
}

FString FYvrXRInputPlugin::FYvrXRInput::GetBoneName(uint8 Bone)
{
	uint8 HandBone = Bone;

	UEnum* BoneEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EYvrXRHandJoint"), true);
	if (BoneEnum)
	{
		return BoneEnum->GetDisplayNameTextByValue(HandBone).ToString();
	}
	return FString("Invalid");
}

bool FYvrXRInputPlugin::FYvrXRInput::IsHandTrackingEnabled()
{
	bool HandTrackingEnabled = false;
	if (OpenXRHMD != nullptr)
	{
		HandTrackingEnabled = OpenXRHMD->IsHandTrackingEnabled();
	}

	return HandTrackingEnabled;
}

FQuat FYvrXRInputPlugin::FYvrXRInput::GetBoneRotation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	FQuat Rotation = FQuat::Identity;

	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		const FTransform& BoneTransform = HandState.KeypointTransforms[(int32)BoneId];
		Rotation = BoneTransform.GetRotation();
	}

	return Rotation;
}

FVector FYvrXRInputPlugin::FYvrXRInput::GetBoneLocation(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	FVector Location = FVector::ZeroVector;

	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		const FTransform& BoneTransform = HandState.KeypointTransforms[(int32)BoneId];
		Location = BoneTransform.GetLocation();
	}

	return Location;
}

float FYvrXRInputPlugin::FYvrXRInput::GetHandScale(const EYvrXRHandType DeviceHand)
{
	float Scale = 1.0f;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		Scale = HandState.HandScale;
	}

	return Scale;
}

float FYvrXRInputPlugin::FYvrXRInput::GetBoneRadii(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	float Radii = 1.0f;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		Radii = HandState.Radii[(int32)BoneId];
	}

	return Radii;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsBoneOrientationValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	bool BoneOrientationValid = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		const XrSpaceLocationFlags& BoneLocationFLags = HandState.JointLocations[(int32)BoneId].locationFlags;
		BoneOrientationValid = BoneLocationFLags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
	}

	return BoneOrientationValid;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsBonePositionValid(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	bool BonePositionValid = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		const XrSpaceLocationFlags& BoneLocationFLags = HandState.JointLocations[(int32)BoneId].locationFlags;
		BonePositionValid = BoneLocationFLags & XR_SPACE_LOCATION_POSITION_VALID_BIT;
	}

	return BonePositionValid;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsBoneOrientationTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	bool BoneOrientationTracked = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		const XrSpaceLocationFlags& BoneLocationFLags = HandState.JointLocations[(int32)BoneId].locationFlags;
		BoneOrientationTracked = BoneLocationFLags & XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
	}

	return BoneOrientationTracked;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsBonePositionTracked(const EYvrXRHandType DeviceHand, const EYvrXRHandJoint BoneId)
{
	bool BonePositionTracked = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		const XrSpaceLocationFlags& BoneLocationFLags = HandState.JointLocations[(int32)BoneId].locationFlags;
		BonePositionTracked = BoneLocationFLags & XR_SPACE_LOCATION_POSITION_TRACKED_BIT;
	}

	return BonePositionTracked;
}

EYvrXRHandTrackingConfidence FYvrXRInputPlugin::FYvrXRInput::GetTrackingConfidence(const EYvrXRHandType DeviceHand)
{
	EYvrXRHandTrackingConfidence Confidence = EYvrXRHandTrackingConfidence::Low;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		Confidence = HandState.ReceivedJointPoses ? EYvrXRHandTrackingConfidence::High : EYvrXRHandTrackingConfidence::Low;
	}
	return Confidence;
}

FTransform FYvrXRInputPlugin::FYvrXRInput::GetPointerPose(const EYvrXRHandType DeviceHand)
{
	FTransform PointerPose = FTransform::Identity;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		PointerPose = ToFTransform(HandState.AimState.aimPose);
	}
	return PointerPose;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsComputed(const EYvrXRHandType DeviceHand)
{
	bool Computed = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		Computed = (HandState.AimState.status & XR_HAND_TRACKING_AIM_COMPUTED_BIT_FB) != 0;
	}

	return Computed;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsAimValid(const EYvrXRHandType DeviceHand)
{
	bool AimValid = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		AimValid = (HandState.AimState.status & XR_HAND_TRACKING_AIM_VALID_BIT_FB) != 0;
	}

	return AimValid;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsSystemGesture(const EYvrXRHandType DeviceHand)
{
	bool SystemGesture = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		SystemGesture = (HandState.AimState.status & XR_HAND_TRACKING_AIM_SYSTEM_GESTURE_BIT_FB) != 0;
	}

	return SystemGesture;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsDominantHand(const EYvrXRHandType DeviceHand)
{
	bool DominantHand = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		DominantHand = (HandState.AimState.status & XR_HAND_TRACKING_AIM_DOMINANT_HAND_BIT_FB) != 0;
	}

	return DominantHand;
}

bool FYvrXRInputPlugin::FYvrXRInput::IsMenuPressed(const EYvrXRHandType DeviceHand)
{
	bool MenuPressed = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		MenuPressed = (HandState.AimState.status & XR_HAND_TRACKING_AIM_MENU_PRESSED_BIT_FB) != 0;
	}

	return MenuPressed;
}

bool FYvrXRInputPlugin::FYvrXRInput::GetFingerIsPinching(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger)
{
	bool FingerIsPinching = false;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		switch (Finger)
		{
		case EYvrXRHandFinger::None: 
			break;
		case EYvrXRHandFinger::Index:
		{
			FingerIsPinching = (HandState.AimState.status & XR_HAND_TRACKING_AIM_INDEX_PINCHING_BIT_FB) != 0;
			break;
		}
		case EYvrXRHandFinger::Middle:
		{
			FingerIsPinching = (HandState.AimState.status & XR_HAND_TRACKING_AIM_MIDDLE_PINCHING_BIT_FB) != 0;
			break;
		}
		case EYvrXRHandFinger::Ring:
		{
			FingerIsPinching = (HandState.AimState.status & XR_HAND_TRACKING_AIM_RING_PINCHING_BIT_FB) != 0;
			break;
		}
		case EYvrXRHandFinger::Pinky:
		{
			FingerIsPinching = (HandState.AimState.status & XR_HAND_TRACKING_AIM_LITTLE_PINCHING_BIT_FB) != 0;
			break;
		}
		default:
			break;
		}
	}
	return FingerIsPinching;
}

float FYvrXRInputPlugin::FYvrXRInput::GetFingerPinchStrength(const EYvrXRHandType DeviceHand, EYvrXRHandFinger Finger)
{
	float FingerPinchStrength = 0.0f;
	if (DeviceHand != EYvrXRHandType::None)
	{
		const FYvrXRHandState& HandState = HandStates[(int32)DeviceHand - 1];
		switch (Finger)
		{
		case EYvrXRHandFinger::None:
			break;
		case EYvrXRHandFinger::Index:
		{
			FingerPinchStrength = HandState.AimState.pinchStrengthIndex;
			break;
		}
		case EYvrXRHandFinger::Middle:
		{
			FingerPinchStrength = HandState.AimState.pinchStrengthMiddle;
			break;
		}
		case EYvrXRHandFinger::Ring:
		{
			FingerPinchStrength = HandState.AimState.pinchStrengthRing;
			break;
		}
		case EYvrXRHandFinger::Pinky:
		{
			FingerPinchStrength = HandState.AimState.pinchStrengthLittle;
			break;
		}
		default:
			break;
		}
	}
	return FingerPinchStrength;
}

#undef LOCTEXT_NAMESPACE // "OpenXRInputPlugin"
