// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FYvrXREditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterSettings();
	void UnregisterSettings();
};