// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrLivePreviewInputModule.h"

#define LOCTEXT_NAMESPACE "FYvrLivePreviewInputModule"

FYvrLivePreviewInputModule::FYvrLivePreviewInputModule()
	:InputDevice(nullptr)
{
}

FYvrLivePreviewInputModule::~FYvrLivePreviewInputModule()
{
}

void FYvrLivePreviewInputModule::StartupModule()
{
	IInputDeviceModule::StartupModule();
}

void FYvrLivePreviewInputModule::ShutdownModule()
{
	IInputDeviceModule::ShutdownModule();
}

TSharedPtr< class IInputDevice > FYvrLivePreviewInputModule::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	InputDevice = MakeShared<FYvrPreviewInput>();
	if (InputDevice)
	{
		InputDevice->SetMessageHandler(InMessageHandler);
	}
	return InputDevice;
}



#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FYvrLivePreviewInputModule, YvrLivePreviewInput)