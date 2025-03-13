// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "UObject/Object.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "YvrEntitlementCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FYvrEntitlementCheckResult);

/**
 * Exposes some of the Platform SDK for blueprint use.
 */
UCLASS(MinimalAPI)
class UYvrEntitlementCallbackProxy : public UOnlineBlueprintCallProxyBase
{
    GENERATED_UCLASS_BODY()

    // Called when there is a successful entitlement check
    UPROPERTY(BlueprintAssignable)
    FYvrEntitlementCheckResult OnSuccess;

    // Called when there is an unsuccessful entitlement check
    UPROPERTY(BlueprintAssignable)
    FYvrEntitlementCheckResult OnFailure;

    // Kick off entitlement check. Asynchronous-- see OnUserPrivilegeCompleteDelegate for results.
    UFUNCTION(BlueprintCallable, Category = "Yvr|Entitlement", meta = (BlueprintInternalUseOnly = "true"))
    static UYvrEntitlementCallbackProxy* VerifyEntitlement();

    /** UOnlineBlueprintCallProxyBase interface */
    virtual void Activate() override;

private:

    // Delegate for VerifyEntitlement.
    void OnUserPrivilegeCompleteDelegate(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 Result);
};
