// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "proxy/ProxyService.h"
#include "IYvrLivePreviewServicePlugin.h"

class FYvrLivePreviewServiceModule : public IYvrLivePreviewServicePlugin
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
