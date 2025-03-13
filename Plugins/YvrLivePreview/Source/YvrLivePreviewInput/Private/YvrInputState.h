// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "IMotionController.h"
#include "InputCoreTypes.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"

namespace YvrInput
{
	enum EYvrControllerButton : uint32_t
	{
		A = 0,
		B = 1,
		X = 2,
		Y = 3,
		Start = 4,
		Reserved = 5,
		LIndexTrigger = 6,
		RIndexTrigger = 7,
		LHandTrigger = 8,
		RHandTrigger = 9,
		LThumbstick = 10,
		RThumbstick = 11,
		LThumbstickUp = 12,
		LThumbstickDown = 13,
		LThumbstickLeft = 14,
		LThumbstickRight = 15,
		RThumbstickUp = 16,
		RThumbstickDown = 17,
		RThumbstickLeft = 18,
		RThumbstickRight = 19,
		Total
	};

	struct FYvrKey
	{
		//Button
		static const FKey YvrButton_A;
		static const FKey YvrButton_B;
		static const FKey YvrButton_X;
		static const FKey YvrButton_Y;
		static const FKey YvrButton_Start;
		static const FKey YvrButton_Reserved;
		static const FKey YvrButton_LIndexTrigger;
		static const FKey YvrButton_RIndexTrigger;
		static const FKey YvrButton_LHandTrigger;
		static const FKey YvrButton_RHandTrigger;
		static const FKey YvrButton_LThumbstick;
		static const FKey YvrButton_RThumbstick;
		static const FKey YvrButton_LThumbstickUp;
		static const FKey YvrButton_LThumbstickDown;
		static const FKey YvrButton_LThumbstickLeft;
		static const FKey YvrButton_LThumbstickRight;
		static const FKey YvrButton_RThumbstickUp;
		static const FKey YvrButton_RThumbstickDown;
		static const FKey YvrButton_RThumbstickLeft;
		static const FKey YvrButton_RThumbstickRight;

		//Touch
		static const FKey YvrTouch_A;
		static const FKey YvrTouch_B;
		static const FKey YvrTouch_X;
		static const FKey YvrTouch_Y;
		static const FKey YvrTouch_LIndexTrigger;
		static const FKey YvrTouch_RIndexTrigger;
		static const FKey YvrTouch_LThumbstick;
		static const FKey YvrTouch_RThumbstick;

		//Analog
		static const FKey YvrButton_LIndexTrigger_Axis;
		static const FKey YvrButton_LThumbstick_X;
		static const FKey YvrButton_LThumbstick_Y;
		static const FKey YvrButton_RIndexTrigger_Axis;
		static const FKey YvrButton_RThumbstick_X;
		static const FKey YvrButton_RThumbstick_Y;
	};

	struct FYvrKeyNames
	{
		//Button
		static const FName YvrButton_A;
		static const FName YvrButton_B;
		static const FName YvrButton_X;
		static const FName YvrButton_Y;
		static const FName YvrButton_Start;
		static const FName YvrButton_Reserved;

		static const FName YvrButton_LIndexTrigger;
		static const FName YvrButton_RIndexTrigger;
		static const FName YvrButton_LHandTrigger;
		static const FName YvrButton_RHandTrigger;
		static const FName YvrButton_LThumbstick;
		static const FName YvrButton_RThumbstick;
		static const FName YvrButton_LThumbstickUp;
		static const FName YvrButton_LThumbstickDown;
		static const FName YvrButton_LThumbstickLeft;
		static const FName YvrButton_LThumbstickRight;
		static const FName YvrButton_RThumbstickUp;
		static const FName YvrButton_RThumbstickDown;
		static const FName YvrButton_RThumbstickLeft;
		static const FName YvrButton_RThumbstickRight;

		//Touch
		static const FName YvrTouch_A;
		static const FName YvrTouch_B;
		static const FName YvrTouch_X;
		static const FName YvrTouch_Y;
		static const FName YvrTouch_LIndexTrigger;
		static const FName YvrTouch_RIndexTrigger;
		static const FName YvrTouch_LThumbstick;
		static const FName YvrTouch_RThumbstick;
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


	struct FYvrControllerState
	{
		/** Button states */
		FYvrButtonState Buttons[EYvrControllerButton::Total];
		FYvrButtonState Touches[EYvrControllerButton::Total];

		float IndexTriggerAxis;
		float ThumbstickAxis[2];

		float HapticAmplitude;
		float HapticFrequency;

		FYvrControllerState()
		{
			IndexTriggerAxis = 0.0;
			ThumbstickAxis[0] = 0.0;
			ThumbstickAxis[1] = 0.0;

			HapticAmplitude = 0.0;
			HapticFrequency = 0.0;

			for (FYvrButtonState& Button : Buttons)
			{
				Button.bIsPressed = false;
				Button.NextRepeatTime = 0.0;
			}

			Buttons[(int32)EYvrControllerButton::A].Key = FYvrKeyNames::YvrButton_A;
			Buttons[(int32)EYvrControllerButton::B].Key = FYvrKeyNames::YvrButton_B;
			Buttons[(int32)EYvrControllerButton::X].Key = FYvrKeyNames::YvrButton_X;
			Buttons[(int32)EYvrControllerButton::Y].Key = FYvrKeyNames::YvrButton_Y;
			Buttons[(int32)EYvrControllerButton::Start].Key = FYvrKeyNames::YvrButton_Start;
			Buttons[(int32)EYvrControllerButton::Reserved].Key = FYvrKeyNames::YvrButton_Reserved;
			Buttons[(int32)EYvrControllerButton::LIndexTrigger].Key = FYvrKeyNames::YvrButton_LIndexTrigger;
			Buttons[(int32)EYvrControllerButton::RIndexTrigger].Key = FYvrKeyNames::YvrButton_RIndexTrigger;
			Buttons[(int32)EYvrControllerButton::LHandTrigger].Key = FYvrKeyNames::YvrButton_LHandTrigger;
			Buttons[(int32)EYvrControllerButton::RHandTrigger].Key = FYvrKeyNames::YvrButton_RHandTrigger;
			Buttons[(int32)EYvrControllerButton::LThumbstick].Key = FYvrKeyNames::YvrButton_LThumbstick;
			Buttons[(int32)EYvrControllerButton::RThumbstick].Key = FYvrKeyNames::YvrButton_RThumbstick;
			Buttons[(int32)EYvrControllerButton::LThumbstickUp].Key = FYvrKeyNames::YvrButton_LThumbstickUp;
			Buttons[(int32)EYvrControllerButton::LThumbstickDown].Key = FYvrKeyNames::YvrButton_LThumbstickDown;
			Buttons[(int32)EYvrControllerButton::LThumbstickLeft].Key = FYvrKeyNames::YvrButton_LThumbstickLeft;
			Buttons[(int32)EYvrControllerButton::LThumbstickRight].Key = FYvrKeyNames::YvrButton_LThumbstickRight;
			Buttons[(int32)EYvrControllerButton::RThumbstickUp].Key = FYvrKeyNames::YvrButton_RThumbstickUp;
			Buttons[(int32)EYvrControllerButton::RThumbstickDown].Key = FYvrKeyNames::YvrButton_RThumbstickDown;
			Buttons[(int32)EYvrControllerButton::RThumbstickLeft].Key = FYvrKeyNames::YvrButton_RThumbstickLeft;
			Buttons[(int32)EYvrControllerButton::RThumbstickRight].Key = FYvrKeyNames::YvrButton_RThumbstickRight;


			for (FYvrButtonState& Touch : Touches)
			{
				Touch.bIsPressed = false;
				Touch.NextRepeatTime = 0.0;
			}

			Touches[(int32)EYvrControllerButton::A].Key = FYvrKeyNames::YvrTouch_A;
			Touches[(int32)EYvrControllerButton::B].Key = FYvrKeyNames::YvrTouch_B;
			Touches[(int32)EYvrControllerButton::X].Key = FYvrKeyNames::YvrTouch_X;
			Touches[(int32)EYvrControllerButton::Y].Key = FYvrKeyNames::YvrTouch_Y;

			Touches[(int32)EYvrControllerButton::LIndexTrigger].Key = FYvrKeyNames::YvrTouch_LIndexTrigger;
			Touches[(int32)EYvrControllerButton::RIndexTrigger].Key = FYvrKeyNames::YvrTouch_RIndexTrigger;
			Touches[(int32)EYvrControllerButton::LThumbstick].Key = FYvrKeyNames::YvrTouch_LThumbstick;
			Touches[(int32)EYvrControllerButton::RThumbstick].Key = FYvrKeyNames::YvrTouch_RThumbstick;
		}
	};
}