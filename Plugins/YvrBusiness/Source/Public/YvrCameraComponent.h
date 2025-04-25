// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/ActorComponent.h"
#include "YvrBusinessBPLibrary.h"
#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"

#include "YvrCameraComponent.generated.h"

UCLASS(BlueprintType)
class UYvrBusinessDelegates : public UObject
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FCameraFrameDelegate, FVSTCameraFrameItem);
    static FCameraFrameDelegate CameraFrameDelegate;
};


UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class UYvrCameraComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCameraFrameDelegate, FVSTCameraFrameItem, Frame);
    UPROPERTY(BlueprintAssignable)
    FCameraFrameDelegate CameraFrameDelegate;

    void OnRegister() override; 
    void OnUnregister() override;

private:
    void FNewYvrCameraFrameAcquired_Handler(FVSTCameraFrameItem Frame) { CameraFrameDelegate.Broadcast(Frame); }
};
