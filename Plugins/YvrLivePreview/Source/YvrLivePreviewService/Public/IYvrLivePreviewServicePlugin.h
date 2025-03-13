// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class IYvrLivePreviewServicePlugin : public IModuleInterface
{
public:

	static inline IYvrLivePreviewServicePlugin& Get()
	{
		return FModuleManager::GetModuleChecked<IYvrLivePreviewServicePlugin>("YvrXRMR");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("YvrXRMR");
	}
};
