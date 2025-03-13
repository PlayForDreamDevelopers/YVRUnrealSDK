// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IYvrXRMRModule.h"
#include "YvrXRHMD.h"

#define LOCTEXT_NAMESPACE "YvrXRMRModule"

class FYvrXRMRModule : public IYvrXRMRModule
{
public:

	FYvrXRMRModule();
	~FYvrXRMRModule();

	static inline FYvrXRMRModule& Get()
	{
		return FModuleManager::GetModuleChecked<FYvrXRMRModule>("YvrXRMR");
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	FYvrXRHMD* YvrXRHMD;
};

#undef LOCTEXT_NAMESPACE