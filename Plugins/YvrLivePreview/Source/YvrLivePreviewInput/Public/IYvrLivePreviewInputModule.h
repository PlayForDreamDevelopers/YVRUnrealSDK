// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "Modules/ModuleManager.h"
#include "IInputDeviceModule.h"

class IYvrLivePreviewInputModule : public IInputDeviceModule
{
public:
	static inline IYvrLivePreviewInputModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IYvrLivePreviewInputModule >( "YvrPreviewInput" );
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "YvrPreviewInput" );
	}
};

