// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Delegates/DelegateCombinations.h"
#include "YvrXRAnchorComponent.h"
#include "openxr.h"

#include "YvrXRAsyncAnchorAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FYvrCreateAnchorEntityActionSuccess, EYvrResult, Result, UYvrAnchorComponent*, AnchorEntityComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FYvrCreateAnchorEntityActionFailure, EYvrResult, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FYvrSaveAnchorEntityActionSuccess, EYvrResult, Result, UYvrAnchorComponent*, AnchorEntityComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FYvrSaveAnchorEntityActionFailure, EYvrResult, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FYvrEraseAnchorEntityActionSuccess, EYvrResult, Result, UYvrAnchorComponent*, AnchorEntityComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FYvrEraseAnchorEntityActionFailure, EYvrResult, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FYvrLoadAnchorEntityActionSuccess, EYvrResult, Result, const TArray<FAnchorLoadResult>&, AnchorLoadResults);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FYvrLoadAnchorEntityActionFailure, EYvrResult, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FYvrStartSpatialSceneCaptureActionSuccess, EYvrResult, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FYvrStartSpatialSceneCaptureActionFailure, EYvrResult, Result);

//////////////////////////////////////////////////////////////////////////
/// Create Anchor Entity
//////////////////////////////////////////////////////////////////////////
UCLASS()
class YVRXRMR_API UYvrCreateAnchorEntity_AsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UYvrCreateAnchorEntity_AsyncAction* YvrXRCreateAnchorEntity_Async(AActor* InBindingActor, const FTransform& InAnchorEntityTransform, float InTimeout);

	UPROPERTY(BlueprintAssignable)
	FYvrCreateAnchorEntityActionSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FYvrCreateAnchorEntityActionFailure OnFailure;

	AActor* BindingActor;

	FTransform AnchorEntityTransform;

	float Timeout;

private:
	void HandleCreateAnchorEntityComplete(EYvrResult Result, UYvrAnchorComponent* AnchorEntityComponent);
};

//////////////////////////////////////////////////////////////////////////
/// Save Anchor Entity
//////////////////////////////////////////////////////////////////////////
UCLASS()
class YVRXRMR_API UYvrSaveAnchorEntity_AsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UYvrSaveAnchorEntity_AsyncAction* YvrXRSaveAnchorEntity_Async(AActor* InBindingActor, EYvrSaveLocation InSaveLocation = EYvrSaveLocation::SaveLocation_Local);

	UPROPERTY(BlueprintAssignable)
	FYvrSaveAnchorEntityActionSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FYvrSaveAnchorEntityActionFailure OnFailure;

	AActor* BindingActor;

	EYvrSaveLocation SaveLocation;

private:
	void HandleSaveAnchorEntityComplete(EYvrResult Result, UYvrAnchorComponent* Anchor);
};

//////////////////////////////////////////////////////////////////////////
/// Erase Anchor Entity
//////////////////////////////////////////////////////////////////////////
UCLASS()
class YVRXRMR_API UYvrEraseAnchorEntity_AsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UYvrEraseAnchorEntity_AsyncAction* YvrXREraseAnchorEntity_Async(AActor* InBindingActor, EYvrSaveLocation InSaveLocation = EYvrSaveLocation::SaveLocation_Local);

	UPROPERTY(BlueprintAssignable)
	FYvrEraseAnchorEntityActionSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FYvrEraseAnchorEntityActionFailure OnFailure;

	AActor* BindingActor;

	EYvrSaveLocation SaveLocation;

private:
	void HandleEraseAnchorEntityComplete(EYvrResult Result, UYvrAnchorComponent* Anchor);
};

//////////////////////////////////////////////////////////////////////////
/// Load Anchor Entity
//////////////////////////////////////////////////////////////////////////
UCLASS()
class YVRXRMR_API UYvrLoadAnchorEntity_AsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UYvrLoadAnchorEntity_AsyncAction* YvrXRLoadAnchorEntity_Async(const FYvrAnchorLoadInfo& InLoadInfo);

	UPROPERTY(BlueprintAssignable)
	FYvrLoadAnchorEntityActionSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FYvrLoadAnchorEntityActionFailure OnFailure;

	FYvrAnchorLoadInfo LoadInfo;

private:
	void HandleLoadAnchorEntityComplete(EYvrResult Result, const TArray<FAnchorLoadResult>& AnchorLoadResults);
};

//////////////////////////////////////////////////////////////////////////
/// Start Spatial Scene Capture
//////////////////////////////////////////////////////////////////////////
UCLASS()
class YVRXRMR_API UYvrStartSpatialSceneCapture_AsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UYvrStartSpatialSceneCapture_AsyncAction* YvrXRStartSpatialSceneCapture_Async();

	UPROPERTY(BlueprintAssignable)
	FYvrStartSpatialSceneCaptureActionSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FYvrStartSpatialSceneCaptureActionFailure OnFailure;

private:
	void HandleStartSpatialSceneCaptureComplete(EYvrResult Result);
};
