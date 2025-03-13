// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "OnlineSettingsYvr.generated.h"

UCLASS(config = Engine, defaultconfig)
class ONLINESUBSYSTEMYVR_API UOnlineSettingsYvr : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	//EntitlementCheck
	UPROPERTY(Config, EditAnywhere, Category = Platform, Meta = (DisplayName = "App Id"))
		int64 AppId;
};
