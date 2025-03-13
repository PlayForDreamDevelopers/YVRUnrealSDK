// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "IMotionController.h"
#include "InputCoreTypes.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"

namespace YvrXRInput
{
	enum class EYvrRemoteControllerButton
	{
		XA,
		YB,
		Menu,
		Grip,
		Thumbstick,
		XA_Touch,
		YB_Touch,
		Trigger_Touch,
		Thumbstick_Touch,

		TotalButtonCount
	};

	enum class EYvrControllerButton
	{
		Trigger,
		Grip,

		XA,
		YB,
		Thumbstick,

		Thumbstick_Up,
		Thumbstick_Down,
		Thumbstick_Left,
		Thumbstick_Right,

		Menu,

		Thumbstick_Touch,
		Trigger_Touch,
		XA_Touch,
		YB_Touch,

		TotalButtonCount
	};

	struct FYvrKey
	{
		static const FKey Yvr_Left_X_Click;
		static const FKey Yvr_Left_Y_Click;
		static const FKey Yvr_Left_X_Touch;
		static const FKey Yvr_Left_Y_Touch;
		static const FKey Yvr_Left_Menu_Click;
		static const FKey Yvr_Left_Grip_Click;
		static const FKey Yvr_Left_Grip_Axis;
		static const FKey Yvr_Left_Trigger_Click;
		static const FKey Yvr_Left_Trigger_Axis;
		static const FKey Yvr_Left_Trigger_Touch;
		static const FKey Yvr_Left_Thumbstick_X;
		static const FKey Yvr_Left_Thumbstick_Y;
		static const FKey Yvr_Left_Thumbstick_Click;
		static const FKey Yvr_Left_Thumbstick_Touch;
		static const FKey Yvr_Left_Thumbstick_Up;
		static const FKey Yvr_Left_Thumbstick_Down;
		static const FKey Yvr_Left_Thumbstick_Left;
		static const FKey Yvr_Left_Thumbstick_Right;
		static const FKey Yvr_Right_A_Click;
		static const FKey Yvr_Right_B_Click;
		static const FKey Yvr_Right_A_Touch;
		static const FKey Yvr_Right_B_Touch;
		static const FKey Yvr_Right_System_Click;
		static const FKey Yvr_Right_Grip_Click;
		static const FKey Yvr_Right_Grip_Axis;
		static const FKey Yvr_Right_Trigger_Click;
		static const FKey Yvr_Right_Trigger_Axis;
		static const FKey Yvr_Right_Trigger_Touch;
		static const FKey Yvr_Right_Thumbstick_X;
		static const FKey Yvr_Right_Thumbstick_Y;
		static const FKey Yvr_Right_Thumbstick_Click;
		static const FKey Yvr_Right_Thumbstick_Touch;
		static const FKey Yvr_Right_Thumbstick_Up;
		static const FKey Yvr_Right_Thumbstick_Down;
		static const FKey Yvr_Right_Thumbstick_Left;
		static const FKey Yvr_Right_Thumbstick_Right;
		static const FKey Yvr_Left_Thumbstick_2D;
		static const FKey Yvr_Right_Thumbstick_2D;
	};

	struct FYvrKeyNames
	{
		static const FName Yvr_Left_X_Click;
		static const FName Yvr_Left_Y_Click;
		static const FName Yvr_Left_X_Touch;
		static const FName Yvr_Left_Y_Touch;
		static const FName Yvr_Left_Menu_Click;
		static const FName Yvr_Left_Grip_Click;
		static const FName Yvr_Left_Grip_Axis;
		static const FName Yvr_Left_Trigger_Click;
		static const FName Yvr_Left_Trigger_Axis;
		static const FName Yvr_Left_Trigger_Touch;
		static const FName Yvr_Left_Thumbstick_X;
		static const FName Yvr_Left_Thumbstick_Y;
		static const FName Yvr_Left_Thumbstick_Click;
		static const FName Yvr_Left_Thumbstick_Touch;
		static const FName Yvr_Left_Thumbstick_Up;
		static const FName Yvr_Left_Thumbstick_Down;
		static const FName Yvr_Left_Thumbstick_Left;
		static const FName Yvr_Left_Thumbstick_Right;
		static const FName Yvr_Right_A_Click;
		static const FName Yvr_Right_B_Click;
		static const FName Yvr_Right_A_Touch;
		static const FName Yvr_Right_B_Touch;
		static const FName Yvr_Right_System_Click;
		static const FName Yvr_Right_Grip_Click;
		static const FName Yvr_Right_Grip_Axis;
		static const FName Yvr_Right_Trigger_Click;
		static const FName Yvr_Right_Trigger_Axis;
		static const FName Yvr_Right_Trigger_Touch;
		static const FName Yvr_Right_Thumbstick_X;
		static const FName Yvr_Right_Thumbstick_Y;
		static const FName Yvr_Right_Thumbstick_Click;
		static const FName Yvr_Right_Thumbstick_Touch;
		static const FName Yvr_Right_Thumbstick_Up;
		static const FName Yvr_Right_Thumbstick_Down;
		static const FName Yvr_Right_Thumbstick_Left;
		static const FName Yvr_Right_Thumbstick_Right;
		static const FName Yvr_Left_Thumbstick_2D;
		static const FName Yvr_Right_Thumbstick_2D;
	};

	struct FYvrButtonState
	{
		/** The Unreal button this maps to.  Different depending on whether this is the Left or Right hand controller */
		FName Key;

		/** Whether we're pressed or not.  While pressed, we will generate repeat presses on a timer */
		bool bIsPressed;

		/** Next time a repeat event should be generated for each button */
		double NextRepeatTime;

		/** Default constructor that just sets sensible defaults */
		FYvrButtonState()
			: Key(NAME_None),
			bIsPressed(false),
			NextRepeatTime(0.0)
		{
		}
	};

	struct FYvrAxisState
	{
		/** The axis that this button state maps to */
		FName Axis;

		/** How close the finger is to this button, from 0.f to 1.f */
		float State;

		FYvrAxisState()
			: Axis(NAME_None)
			, State(0.f)
		{
		}
	};


	struct FYvrControllerState
	{
		/** Button states */
		FYvrButtonState Buttons[(int32)EYvrControllerButton::TotalButtonCount];

		float TriggerAxis;
		FVector2D ThumbstickAxes;

		FYvrControllerState(const EControllerHand Hand)
			: TriggerAxis(0.0f),
			ThumbstickAxes(FVector2D::ZeroVector)
		{
			for (FYvrButtonState& Button : Buttons)
			{
				Button.bIsPressed = false;
				Button.NextRepeatTime = 0.0;
			}

			Buttons[(int32)EYvrControllerButton::Trigger].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Trigger_Click : FYvrKeyNames::Yvr_Right_Trigger_Click;
			Buttons[(int32)EYvrControllerButton::Grip].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Grip_Click : FYvrKeyNames::Yvr_Right_Grip_Click;
			Buttons[(int32)EYvrControllerButton::Thumbstick].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Thumbstick_Click : FYvrKeyNames::Yvr_Right_Thumbstick_Click;
			Buttons[(int32)EYvrControllerButton::XA].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_X_Click : FYvrKeyNames::Yvr_Right_A_Click;
			Buttons[(int32)EYvrControllerButton::YB].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Y_Click : FYvrKeyNames::Yvr_Right_B_Click;
			Buttons[(int32)EYvrControllerButton::Thumbstick_Up].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Thumbstick_Up : FYvrKeyNames::Yvr_Right_Thumbstick_Up;
			Buttons[(int32)EYvrControllerButton::Thumbstick_Down].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Thumbstick_Down : FYvrKeyNames::Yvr_Right_Thumbstick_Down;
			Buttons[(int32)EYvrControllerButton::Thumbstick_Left].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Thumbstick_Left : FYvrKeyNames::Yvr_Right_Thumbstick_Left;
			Buttons[(int32)EYvrControllerButton::Thumbstick_Right].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Thumbstick_Right : FYvrKeyNames::Yvr_Right_Thumbstick_Right;

			Buttons[(int32)EYvrControllerButton::Menu].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Menu_Click : FYvrKeyNames::Yvr_Right_System_Click;

			Buttons[(int32)EYvrControllerButton::Thumbstick_Touch].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Thumbstick_Touch : FYvrKeyNames::Yvr_Right_Thumbstick_Touch;
			Buttons[(int32)EYvrControllerButton::Trigger_Touch].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Trigger_Touch : FYvrKeyNames::Yvr_Right_Trigger_Touch;
			Buttons[(int32)EYvrControllerButton::XA_Touch].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_X_Touch : FYvrKeyNames::Yvr_Right_A_Touch;
			Buttons[(int32)EYvrControllerButton::YB_Touch].Key = (Hand == EControllerHand::Left) ? FYvrKeyNames::Yvr_Left_Y_Touch : FYvrKeyNames::Yvr_Right_B_Touch;
		}

		FYvrControllerState()
		{

		}
	};
}