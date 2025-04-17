#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/ActorComponent.h"

#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"

#include "YvrXRMarkerComponent.generated.h"

UCLASS(BlueprintType)
class YVRXRMR_API UYvrXRMarkerDelegates : public UObject
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_FourParams(FNewYvrMarkerDetected, int32, const FVector&, const FRotator&, const FVector2D&);
    static FNewYvrMarkerDetected NewYvrMarkerDetected;

    DECLARE_MULTICAST_DELEGATE_FourParams(FYvrMarkerMoved, int32, const FVector&, const FRotator&, const FVector2D&);
    static FYvrMarkerMoved YvrMarkerMoved;

    DECLARE_MULTICAST_DELEGATE_OneParam(FYvrMarkerLost, int32);
    static FYvrMarkerLost YvrMarkerLost;
};


UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class YVRXRMR_API UYvrXRMarkerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FNewYvrMarkerDetected, int32, MarkerId, const FVector&, Position, const FRotator&, Rotation, const FVector2D&, Size);
    UPROPERTY(BlueprintAssignable)
    FNewYvrMarkerDetected NewYvrMarkerDetected;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FYvrMarkerMoved, int32, MarkerId, const FVector&, Position, const FRotator&, Rotation, const FVector2D&, Size);
    UPROPERTY(BlueprintAssignable)
    FYvrMarkerMoved YvrMarkerMoved;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FYvrMarkerLost, int32, MarkerId);
    UPROPERTY(BlueprintAssignable)
    FYvrMarkerLost YvrMarkerLost;

    void OnRegister() override;
    void OnUnregister() override;


private:
    void FNewYvrMarkerDetected_Handler(int32 MarkerId, const FVector& Position, const FRotator& Rotation, const FVector2D& Size) { NewYvrMarkerDetected.Broadcast(MarkerId, Position, Rotation, Size); }
    void FYvrMarkerMoved_Handler(int32 MarkerId, const FVector& Position, const FRotator& Rotation, const FVector2D& Size) { YvrMarkerMoved.Broadcast(MarkerId, Position, Rotation, Size); }
    void FYvrMarkerLost_Handler(int32 MarkerId) { YvrMarkerLost.Broadcast(MarkerId); }
};
