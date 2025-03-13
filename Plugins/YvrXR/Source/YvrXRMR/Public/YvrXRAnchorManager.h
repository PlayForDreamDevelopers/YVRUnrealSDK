// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YvrXRCore.h"
#include "YvrXRHMD.h"
#include "YvrXRMRTypes.h"
#include "YvrXRAnchorComponent.h"

DECLARE_DELEGATE_TwoParams(FYvrCreateAnchorEntityDelegate, EYvrResult, UYvrAnchorComponent*);
//DECLARE_DELEGATE_OneParam(FYvrDestroyAnchorEntityDelegate, EYvrResult);
DECLARE_DELEGATE_OneParam(FYvrSetSpaceComponentStatusDelegate, EYvrResult);
DECLARE_DELEGATE_TwoParams(FYvrSaveAnchorEntityDelegate, EYvrResult, UYvrAnchorComponent*);
DECLARE_DELEGATE_TwoParams(FYvrEraseAnchorEntityDelegate, EYvrResult, UYvrAnchorComponent*);
//DECLARE_DELEGATE_OneParam(FYvrClearAnchorEntityDelegate, EYvrResult);
DECLARE_DELEGATE_TwoParams(FYvrLoadAnchorEntityDelegate, EYvrResult, const TArray<FAnchorLoadResult>&);
DECLARE_DELEGATE_OneParam(FYvrStartSpatialSceneCaptureDelegate, EYvrResult);

class YVRXRMR_API FYvrAnchorManager
{
public:
	static FYvrAnchorManager* GetInstance();

	void Initialize(FYvrXRHMD* InYvrXRHMD);
	void Shutdown();

public:
	void PollEvent(XrEventDataBuffer EventData);

	bool CreateAnchorEntity(AActor* BindingActor, const FTransform& AnchorEntityTransform, float Timeout, const FYvrCreateAnchorEntityDelegate& Delegate);
	//bool DestroyAnchorEntity(AActor* BoundActor, const FYvrDestroyAnchorEntityDelegate& Delegate);
	bool SetSpaceComponentStatus(UYvrAnchorComponent* AnchorComponent, EYvrSaveLocation SaveLocation, const FYvrSaveAnchorEntityDelegate& Delegate);
	bool SaveAnchorEntity(UYvrAnchorComponent* AnchorComponent, EYvrSaveLocation SaveLocation, const FYvrSaveAnchorEntityDelegate& Delegate);
	bool EraseAnchorEntity(UYvrAnchorComponent* AnchorComponent, EYvrSaveLocation SaveLocation, const FYvrEraseAnchorEntityDelegate& Delegate);
	//bool ClearAnchorEntity(EYvrSaveLocation SaveLocation, const FYvrClearAnchorEntityDelegate& Delegate);
	bool LoadAnchorEntity(const FYvrAnchorLoadInfo& AnchorLoadInfo, const FYvrLoadAnchorEntityDelegate& Delegate);
	bool StartSpatialSceneCapture(const FYvrStartSpatialSceneCaptureDelegate& Delegate);

	bool GetAnchorEntityUUID(AActor* BoundActor, FYvrAnchorUUID& OutAnchorUUID);
	bool GetAnchorPose(UYvrAnchorComponent* AnchorComponent, FTransform& OutAnchorPose);
	bool GetAnchorBoundingBox2D(AActor* BoundActor, FBox2D& OutAnchorBoundingBox);
	bool GetAnchorBoundingBox3D(AActor* BoundActor, FBox& OutAnchorBoundingBox);
	bool GetAnchorSemanticLabels(AActor* BoundActor, TArray<FString>& OutLabels);
	bool UpdateAnchor(UYvrAnchorComponent* AnchorComponent);

private:
	FYvrAnchorManager();
	~FYvrAnchorManager();

	DECLARE_MULTICAST_DELEGATE_FourParams(FYvrCreateAnchorEntityEventDelegate, uint64_t, XrResult, const FYvrAnchor&, const FYvrAnchorUUID&);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FYvrSetSpaceComponentStatusEventDelegate, uint64_t, XrResult);
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FYvrSaveAnchorEntityEventDelegate, uint64_t, XrResult, EYvrSaveLocation);
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FYvrEraseAnchorEntityEventDelegate, uint64_t, XrResult, EYvrSaveLocation);
	//DECLARE_MULTICAST_DELEGATE_ThreeParams(FYvrClearAnchorEntityEventDelegate, uint64_t, EYvrResult, EYvrSaveLocation);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FYvrLoadAnchorEntityEventDelegate, uint64_t, XrResult);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FYvrStartSpatialSceneCaptureEventDelegate, uint64_t, XrResult);

	FYvrCreateAnchorEntityEventDelegate CreateAnchorEntityEventDelegate;
	FYvrSetSpaceComponentStatusEventDelegate SetSpaceComponentStatusEventDelegate;
	FYvrSaveAnchorEntityEventDelegate SaveAnchorEntityEventDelegate;
	FYvrEraseAnchorEntityEventDelegate EraseAnchorEntityEventDelegate;
	//FYvrClearAnchorEntityEventDelegate ClearAnchorEntityEventDelegate;
	FYvrLoadAnchorEntityEventDelegate LoadAnchorEntityEventDelegate;
	FYvrStartSpatialSceneCaptureEventDelegate StartSpatialSceneCaptureEventDelegate;

	FDelegateHandle HandleOfCreateAnchorEntity;
	FDelegateHandle HandleOfSaveAnchorEntity;
	FDelegateHandle HandleOfEraseAnchorEntity;
	//FDelegateHandle HandleOfClearAnchorEntity;
	FDelegateHandle HandleOfLoadAnchorEntity;
	FDelegateHandle HandleOfStartSpatialSceneCapture;
	FDelegateHandle HandleOfSetSpaceComponentStatus;

	void HandleCreateAnchorEntityEvent(uint64_t AsyncTaskId, XrResult Result, const FYvrAnchor& AnchorHandle, const FYvrAnchorUUID& AnchorUUID);
	void HandleSetSpaceComponentStatusEvent(uint64_t AsyncTaskId, XrResult Result);
	void HandleSaveAnchorEntityEvent(uint64_t AsyncTaskId, XrResult Result, EYvrSaveLocation SaveLocation);
	void HandleEraseAnchorEntityEvent(uint64_t AsyncTaskId, XrResult Result, EYvrSaveLocation SaveLocation);
	//void HandleClearAnchorEntityEvent(uint64_t AsyncTaskId, EYvrResult Result, EYvrSaveLocation SaveLocation);
	void HandleLoadAnchorEntityEvent(uint64_t AsyncTaskId, XrResult Result);
	void HandleStartSpatialSceneCaptureEvent(uint64_t AsyncTaskId, XrResult Result);

	bool IsAnchorValid(AActor* BoundActor);
	bool IsAnchorValid(UYvrAnchorComponent* AnchorComponent);
	UYvrAnchorComponent* GetAnchorComponent(AActor* BoundActor);

private:
	FYvrXRHMD* YvrXRHMD;

	EYvrResult ToYvrResult(XrResult Result);

	struct FAnchorCreateInfo
	{
		uint64_t AsyncTaskId;
		FYvrCreateAnchorEntityDelegate Delegate;
		UYvrAnchorComponent* AnchorComponent;
	};

	struct FComponentStatusInfo
	{
		uint64_t AsyncTaskId;
		FYvrSaveAnchorEntityDelegate Delegate;
		UYvrAnchorComponent* AnchorComponent;
		EYvrSaveLocation Location;
	};

	struct FAnchorSaveInfo
	{
		uint64_t AsyncTaskId;
		FYvrSaveAnchorEntityDelegate Delegate;
		UYvrAnchorComponent* AnchorComponent;
	};

	struct FAnchorEraseInfo
	{
		uint64_t AsyncTaskId;
		FYvrEraseAnchorEntityDelegate Delegate;
		UYvrAnchorComponent* AnchorComponent;
	};

	struct FAnchorLoadInfo
	{
		uint64_t AsyncTaskId;
		FYvrLoadAnchorEntityDelegate Delegate;
	};

	struct FStartSpatialSceneCaptureInfo
	{
		uint64_t AsyncTaskId;
		FYvrStartSpatialSceneCaptureDelegate Delegate;
	};

	TMap<uint64_t, FAnchorCreateInfo> CreateAnchorBindings;
	TMap<uint64_t, FComponentStatusInfo> ComponentStatusBindings;
	TMap<uint64_t, FAnchorSaveInfo> SaveAnchorsBindings;
	TMap<uint64_t, FAnchorEraseInfo> EraseAnchorsBindings;
	//TMap<uint64_t, FAnchorClearInfo> ClearAnchorsBindings;
	TMap<uint64_t, FAnchorLoadInfo> LoadAnchorsBindings;
	TMap<uint64_t, FStartSpatialSceneCaptureInfo> StartSpatialSceneCaptureBindings;

	FDelegateHandle HandleOfPollEvent;
};