// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YvrXRMRTypes.h"
#include "Components/ActorComponent.h"

#include "YvrXRAnchorComponent.generated.h"

UCLASS(BlueprintType)
class YVRXRMR_API UYvrAnchorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UYvrAnchorComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR")
	FYvrAnchor GetAnchorHandle() const { return AnchorHandle; }

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR")
	void SetAnchorHandle(FYvrAnchor NewAnchorHandle) { AnchorHandle = NewAnchorHandle; }

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR")
	FString GetAnchorHandleString() const { return AnchorHandle.ToString(); }

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR")
	bool IsAnchorValid() const { return AnchorHandle.IsValid(); }

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR")
	void SetAnchorUUID(FYvrAnchorUUID NewAnchorUUID) { AnchorUUID = NewAnchorUUID; }

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR")
	FYvrAnchorUUID GetAnchorUUID() const { return AnchorUUID; }

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR")
	FString GetAnchorUUIDString() const {return AnchorUUID.ToString(); }

protected:
	FYvrAnchor AnchorHandle;
	FYvrAnchorUUID AnchorUUID;
};