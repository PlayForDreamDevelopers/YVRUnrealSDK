// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "UObject/Object.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "YvrIdentityCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FYvrIdentitySuccessResult, FString, YvrId, FString, YvrName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FYvrIdentityFailureResult);

/**
 * Exposes the yvr id of the Platform SDK for blueprint use.
 */
UCLASS(MinimalAPI)
class UYvrIdentityCallbackProxy : public UOnlineBlueprintCallProxyBase
{
GENERATED_UCLASS_BODY()

	// Called when it successfully gets back the yvr id
	UPROPERTY(BlueprintAssignable)
	FYvrIdentitySuccessResult OnSuccess;

	// Called when it fails to get the yvr id
	UPROPERTY(BlueprintAssignable)
	FYvrIdentityFailureResult OnFailure;

	// Kick off GetYvrIdentity. Asynchronous-- see OnLoginCompleteDelegate for results.
	UFUNCTION(BlueprintCallable, Category = "Yvr|Identity", meta = (BlueprintInternalUseOnly = "true"))
	static UYvrIdentityCallbackProxy* GetYvrIdentity(int32 LocalUserNum);

	/** UOnlineBlueprintCallProxyBase interface */
	virtual void Activate() override;

private:

	int32 LocalUserNum;

	FDelegateHandle DelegateHandle;

	// Delegate for GetYvrID.
	void OnLoginCompleteDelegate(int32 Unused, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorStr);

};
