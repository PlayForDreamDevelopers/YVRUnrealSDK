// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IHeadMountedDisplayModule.h"

class YVRLIVEPREVIEWHMD_API IYvrPreviewHMDPlugin : public IHeadMountedDisplayModule
{
public:
	IYvrPreviewHMDPlugin();
	~IYvrPreviewHMDPlugin();

	static inline IYvrPreviewHMDPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked< IYvrPreviewHMDPlugin >("YvrPreviewHMD");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("YvrPreviewHMD");
	}

	/**
	 * Initializes the plugin, in case of VR system restart
	 */
	virtual bool Initialize() = 0;

};
