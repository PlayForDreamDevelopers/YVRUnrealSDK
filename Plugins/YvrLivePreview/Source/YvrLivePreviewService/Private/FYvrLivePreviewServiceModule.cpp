// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "FYvrLivePreviewServiceModule.h"
#include "YvrPreviewManager.h"

#define LOCTEXT_NAMESPACE "FYvrLivePreviewServiceModule"

void FYvrLivePreviewServiceModule::StartupModule()
{
	YvrPreviewManager::GetInstance()->Start();
}

void FYvrLivePreviewServiceModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FYvrLivePreviewServiceModule, YvrLivePreviewService)