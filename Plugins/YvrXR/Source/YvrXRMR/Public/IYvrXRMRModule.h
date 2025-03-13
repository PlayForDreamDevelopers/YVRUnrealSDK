// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "Modules/ModuleManager.h"

class IYvrXRMRModule : public IModuleInterface
{

public:

	static inline IYvrXRMRModule& Get()
	{
		return FModuleManager::GetModuleChecked<IYvrXRMRModule>("YvrXRMR");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("YvrXRMR");
	}
};
