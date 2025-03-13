// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrBusiness.h"
#include "YvrBusinessWrapper.h"
#include "YvrInterfaceWrapper.h"

#define LOCTEXT_NAMESPACE "FYvrBusinessModule"

void FYvrBusinessModule::StartupModule()
{

#if PLATFORM_ANDROID
	FYvrBusinessWrapper::Init();
	FYvrInterfaceWrapper::Init();
#endif
}

void FYvrBusinessModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FYvrBusinessModule, YvrBusiness)