// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "IYvrLivePreviewInputModule.h"
#include "IInputDevice.h"
#include "YvrPreviewInput.h"
#include "Modules/ModuleManager.h"

class FYvrLivePreviewInputModule : public IYvrLivePreviewInputModule
{
public:
	FYvrLivePreviewInputModule();
	virtual ~FYvrLivePreviewInputModule();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;
private:
	TSharedPtr<class FYvrPreviewInput> InputDevice;
};
