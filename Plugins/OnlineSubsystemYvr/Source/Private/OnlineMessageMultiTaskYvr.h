// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineDelegateMacros.h"
#include "OnlineSubsystemYvr.h"
#include "OnlineSubsystemPackage.h"

class FOnlineMessageMultiTaskYvr
{
private:
	TArray<int> InProgressRequests;

protected:
	bool bDidAllRequestsFinishedSuccessfully = true;

	DECLARE_DELEGATE(FFinalizeDelegate);

	FOnlineMessageMultiTaskYvr::FFinalizeDelegate Delegate;

PACKAGE_SCOPE:
	FOnlineSubsystemYvr& YvrSubsystem;

	FOnlineMessageMultiTaskYvr(
		FOnlineSubsystemYvr& InYvrSubsystem,
		const FOnlineMessageMultiTaskYvr::FFinalizeDelegate& InDelegate)
		: Delegate(InDelegate)
		, YvrSubsystem(InYvrSubsystem)
	{
	}

	void AddNewRequest(int RequestId);
};
