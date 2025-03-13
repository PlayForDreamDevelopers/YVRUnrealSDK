// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

#ifndef YVR_SUBSYSTEM
#define YVR_SUBSYSTEM FName(TEXT("Yvr"))
#endif

/**
 * Online subsystem module class  (Yvr Implementation)
 * Code related to the loading of the Yvr module
 */
class FOnlineSubsystemYvrModule : public IModuleInterface
{
private:

	/** Class responsible for creating instance(s) of the subsystem */
	class FOnlineFactoryYvr* YvrFactory;

public:

	FOnlineSubsystemYvrModule() :
		YvrFactory(nullptr)
	{}

	virtual ~FOnlineSubsystemYvrModule() {}

	// IModuleInterface

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

	virtual bool SupportsAutomaticShutdown() override
	{
		return false;
	}
};
