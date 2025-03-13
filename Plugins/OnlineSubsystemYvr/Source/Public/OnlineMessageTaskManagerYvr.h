// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "OnlineSubsystemYvrTypes.h"

DECLARE_DELEGATE_OneParam(FYvrInitOnCompleteDelegate, bool);
DECLARE_DELEGATE_TwoParams(FYvrMessageOnCompleteDelegate, YvrMessageHandle, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FYvrMulticastMessageOnCompleteDelegate, YvrMessageHandle, bool);

class FOnlineMessageTaskManagerYvr
{
	void OnReceiveMessage(YvrMessageHandle Message);

	/** Direct Requests waiting for a Message response */
	TMap<uint64, FYvrMessageOnCompleteDelegate> RequestDelegates;

	/** Notify Requests waiting for a Message response */
	TMap<YvrMessageType, FYvrMulticastMessageOnCompleteDelegate> NotifyDelegates;

public:


	void AddRequestDelegate(uint64 RequestId, FYvrMessageOnCompleteDelegate&& Delegate);

	FYvrMulticastMessageOnCompleteDelegate& GetNotifyDelegate(YvrMessageType MessageType);

	void RemoveNotifyDelegate(YvrMessageType MessageType, const FDelegateHandle& Delegate);

	bool Tick(float DeltaTime);
};