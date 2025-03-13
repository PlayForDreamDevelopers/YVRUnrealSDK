#include "YvrXRAnchorManager.h"
#include "YvrXRAnchorComponent.h"
#include "..\Public\YvrXRAnchorManager.h"

FYvrAnchorManager::FYvrAnchorManager()
{
	HandleOfCreateAnchorEntity = CreateAnchorEntityEventDelegate.AddRaw(this, &FYvrAnchorManager::HandleCreateAnchorEntityEvent);
	HandleOfSetSpaceComponentStatus = SetSpaceComponentStatusEventDelegate.AddRaw(this, &FYvrAnchorManager::HandleSetSpaceComponentStatusEvent);
	HandleOfSaveAnchorEntity = SaveAnchorEntityEventDelegate.AddRaw(this, &FYvrAnchorManager::HandleSaveAnchorEntityEvent);
	HandleOfEraseAnchorEntity = EraseAnchorEntityEventDelegate.AddRaw(this, &FYvrAnchorManager::HandleEraseAnchorEntityEvent);
	//HandleOfClearAnchorEntity = ClearAnchorEntityEventDelegate.AddRaw(this, &FYvrAnchorManager::HandleClearAnchorEntityEvent);
	HandleOfLoadAnchorEntity = LoadAnchorEntityEventDelegate.AddRaw(this, &FYvrAnchorManager::HandleLoadAnchorEntityEvent);
	HandleOfStartSpatialSceneCapture = StartSpatialSceneCaptureEventDelegate.AddRaw(this, &FYvrAnchorManager::HandleStartSpatialSceneCaptureEvent);
}

FYvrAnchorManager::~FYvrAnchorManager()
{
	CreateAnchorEntityEventDelegate.Remove(HandleOfCreateAnchorEntity);
	SetSpaceComponentStatusEventDelegate.Remove(HandleOfSetSpaceComponentStatus);
	SaveAnchorEntityEventDelegate.Remove(HandleOfSaveAnchorEntity);
	EraseAnchorEntityEventDelegate.Remove(HandleOfEraseAnchorEntity);
	//ClearAnchorEntityEventDelegate.Remove(HandleOfClearAnchorEntity);
	LoadAnchorEntityEventDelegate.Remove(HandleOfLoadAnchorEntity);
	StartSpatialSceneCaptureEventDelegate.Remove(HandleOfStartSpatialSceneCapture);
}

FYvrAnchorManager* FYvrAnchorManager::GetInstance()
{
	static FYvrAnchorManager Instance;
	return &Instance;
}

void FYvrAnchorManager::Initialize(FYvrXRHMD* InYvrXRHMD)
{
	YvrXRHMD = InYvrXRHMD;
	if (YvrXRHMD)
	{
		HandleOfPollEvent = YvrXRHMD->OnPollEventDelegate().AddRaw(FYvrAnchorManager::GetInstance(), &FYvrAnchorManager::PollEvent);
	}
}

void FYvrAnchorManager::Shutdown()
{
	if (HandleOfPollEvent.IsValid() && YvrXRHMD)
	{
		YvrXRHMD->OnPollEventDelegate().Remove(HandleOfPollEvent);
	}
}

void FYvrAnchorManager::PollEvent(XrEventDataBuffer EventData)
{
	switch (EventData.type)
	{
		case XR_TYPE_EVENT_DATA_SPATIAL_ANCHOR_CREATE_COMPLETE_FB:					// CreateAnchorEntity				Event
		{
			const XrEventDataSpatialAnchorCreateCompleteFB& CreatedInfo = reinterpret_cast<const XrEventDataSpatialAnchorCreateCompleteFB&>(EventData);
			//EYvrResult Result = ToYvrResult(CreatedInfo.result);
			FYvrAnchor AnchorHandle = (uint64_t)CreatedInfo.space;
			FYvrAnchorUUID AnchorUUID = CreatedInfo.uuid.data;
			CreateAnchorEntityEventDelegate.Broadcast(CreatedInfo.requestId, CreatedInfo.result, AnchorHandle, AnchorUUID);
			break;
		}
		case XR_TYPE_EVENT_DATA_SPACE_SAVE_COMPLETE_FB:				// SaveAnchorEntity				Event
		{
			const XrEventDataSpaceSaveCompleteFB& SavedInfo = reinterpret_cast<const XrEventDataSpaceSaveCompleteFB&>(EventData);
			EYvrSaveLocation SaveLocation = (EYvrSaveLocation)SavedInfo.location;
			SaveAnchorEntityEventDelegate.Broadcast(SavedInfo.requestId, SavedInfo.result, SaveLocation);
			break;
		}
		case XR_TYPE_EVENT_DATA_SPACE_ERASE_COMPLETE_FB:				// EraseAnchorEntity			Event
		{
			const XrEventDataSpaceEraseCompleteFB& ErasedInfo = reinterpret_cast<const XrEventDataSpaceEraseCompleteFB&>(EventData);
			EYvrSaveLocation SaveLocation = (EYvrSaveLocation)ErasedInfo.location;
			EraseAnchorEntityEventDelegate.Broadcast(ErasedInfo.requestId, ErasedInfo.result, SaveLocation);
			break;
		}
		case XR_TYPE_EVENT_DATA_SPACE_QUERY_COMPLETE_FB:					// LoadAnchorEntity					Event
		{
			const XrEventDataSpaceQueryCompleteFB& LoadedInfo = reinterpret_cast<const XrEventDataSpaceQueryCompleteFB&>(EventData);
			LoadAnchorEntityEventDelegate.Broadcast(LoadedInfo.requestId, LoadedInfo.result);
			break;
		}
		case XR_TYPE_EVENT_DATA_SCENE_CAPTURE_COMPLETE_FB:				// SpatialSceneCaptured				Event
		{
			const XrEventDataSceneCaptureCompleteFB& CapturedInfo = reinterpret_cast<const XrEventDataSceneCaptureCompleteFB&>(EventData);
			StartSpatialSceneCaptureEventDelegate.Broadcast(CapturedInfo.requestId, CapturedInfo.result);
			break;
		}
		case XR_TYPE_EVENT_DATA_SPACE_SET_STATUS_COMPLETE_FB:            // SetSpaceComponentStatus				Event
		{
			const XrEventDataSpaceSetStatusCompleteFB& StateInfo = reinterpret_cast<const XrEventDataSpaceSetStatusCompleteFB&>(EventData);
			SetSpaceComponentStatusEventDelegate.Broadcast(StateInfo.requestId, StateInfo.result);
			break;
		}
		default:
		{
			break;
		}
	}
}

bool FYvrAnchorManager::CreateAnchorEntity(AActor* BindingActor, const FTransform& AnchorEntityTransform, float Timeout, const FYvrCreateAnchorEntityDelegate& Delegate)
{
	if (!IsValid(BindingActor) || !IsValid(BindingActor->GetWorld()))
	{
		return false;
	}

	UYvrAnchorComponent* AnchorComponent = Cast<UYvrAnchorComponent>(BindingActor->GetComponentByClass(UYvrAnchorComponent::StaticClass()));

	if (IsValid(AnchorComponent) && AnchorComponent->IsAnchorValid())
	{
		return false;
	}
	if (!AnchorComponent)
	{
		AnchorComponent = Cast<UYvrAnchorComponent>(BindingActor->AddComponentByClass(UYvrAnchorComponent::StaticClass(), false, FTransform::Identity, false));
	}

	float WorldToMetersScale = BindingActor->GetWorld()->GetWorldSettings()->WorldToMeters;
	FTransform TrackingToWorld = YvrXRHMD->GetTrackingToWorldTransform();
	const FQuat TrackingOrientation = TrackingToWorld.Inverse().TransformRotation(AnchorEntityTransform.Rotator().Quaternion());
	const FVector TrackingPosition = TrackingToWorld.Inverse().TransformPosition(AnchorEntityTransform.GetLocation());
	FTransform UnrealPose = FTransform(TrackingOrientation, TrackingPosition);

	XrSpatialAnchorCreateInfoFB EntityInfo;
	EntityInfo.type = XR_TYPE_SPATIAL_ANCHOR_CREATE_INFO_FB;
	EntityInfo.space = YvrXRHMD->GetTrackingSpace();
	EntityInfo.poseInSpace = ToXrPose(UnrealPose, WorldToMetersScale);
	EntityInfo.time = ToXrTime(Timeout);
	EntityInfo.next = nullptr;

	XrAsyncRequestIdFB AsyncTaskId = 0;
	PFN_xrCreateSpatialAnchorFB CreateSpatialAnchorFB;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrCreateSpatialAnchorFB", (PFN_xrVoidFunction*)&CreateSpatialAnchorFB));
	//XR_ENSURE(CreateSpatialAnchorFB(Session, &EntityInfo, &AsyncTaskId));
	XrResult Result = CreateSpatialAnchorFB(YvrXRHMD->GetSession(), &EntityInfo, &AsyncTaskId);

	if (XR_FAILED(Result))
	{
		return false;
	}

	FAnchorCreateInfo TaskInfo;
	TaskInfo.AsyncTaskId = AsyncTaskId;
	TaskInfo.Delegate = Delegate;
	TaskInfo.AnchorComponent = AnchorComponent;
	CreateAnchorBindings.Add(AsyncTaskId, TaskInfo);
	return true;
}

bool FYvrAnchorManager::SetSpaceComponentStatus(UYvrAnchorComponent* AnchorComponent, EYvrSaveLocation SaveLocation, const FYvrSaveAnchorEntityDelegate& Delegate)
{
	if (!IsValid(AnchorComponent))
	{
		return false;
	}

	if (!AnchorComponent->IsAnchorValid())
	{
		return false;
	}

	PFN_xrSetSpaceComponentStatusFB setSpaceComponentStatus;
	XrSpaceComponentStatusSetInfoFB setInfoFB;
	setInfoFB.type = XR_TYPE_SPACE_COMPONENT_STATUS_SET_INFO_FB;
	setInfoFB.next = nullptr;
	setInfoFB.componentType = XR_SPACE_COMPONENT_TYPE_STORABLE_FB;
	setInfoFB.enabled = true;
	setInfoFB.timeout = 0;
	XrAsyncRequestIdFB AsyncTaskId = 0;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrSetSpaceComponentStatusFB", (PFN_xrVoidFunction*)&setSpaceComponentStatus));

	XrResult Result = setSpaceComponentStatus((XrSpace)AnchorComponent->GetAnchorHandle().GetValue(), &setInfoFB, &AsyncTaskId);

	if (XR_FAILED(Result))
	{
		return false;
	}

	FComponentStatusInfo TaskInfo;
	TaskInfo.AsyncTaskId = AsyncTaskId;
	TaskInfo.Delegate = Delegate;
	TaskInfo.AnchorComponent = AnchorComponent;
	TaskInfo.Location = SaveLocation;

	ComponentStatusBindings.Add(AsyncTaskId, TaskInfo);
	return true;
}

bool FYvrAnchorManager::SaveAnchorEntity(UYvrAnchorComponent* AnchorComponent, EYvrSaveLocation SaveLocation, const FYvrSaveAnchorEntityDelegate& Delegate)
{
	if (!IsValid(AnchorComponent))
	{
		return false;
	}

	if (!AnchorComponent->IsAnchorValid())
	{
		return false;
	}

	XrSpaceSaveInfoFB SaveInfo;
	SaveInfo.type = XR_TYPE_SPACE_SAVE_INFO_FB;
	SaveInfo.space = (XrSpace)AnchorComponent->GetAnchorHandle().GetValue();
	SaveInfo.location = (XrSpaceStorageLocationFB)SaveLocation;
	SaveInfo.persistenceMode = XrSpacePersistenceModeFB::XR_SPACE_PERSISTENCE_MODE_INDEFINITE_FB;
	SaveInfo.next = nullptr;

	XrAsyncRequestIdFB AsyncTaskId = 0;
	PFN_xrSaveSpaceFB SaveSpaceFB;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrSaveSpaceFB", (PFN_xrVoidFunction*)&SaveSpaceFB));
	XrResult Result = SaveSpaceFB(YvrXRHMD->GetSession(), &SaveInfo, &AsyncTaskId);

	if (XR_FAILED(Result))
	{
		return false;
	}

	FAnchorSaveInfo TaskInfo;
	TaskInfo.AsyncTaskId = AsyncTaskId;
	TaskInfo.Delegate = Delegate;
	TaskInfo.AnchorComponent = AnchorComponent;
	SaveAnchorsBindings.Add(AsyncTaskId, TaskInfo);
	return true;
}

bool FYvrAnchorManager::EraseAnchorEntity(UYvrAnchorComponent* AnchorComponent, EYvrSaveLocation SaveLocation, const FYvrEraseAnchorEntityDelegate& Delegate)
{
	if (!IsValid(AnchorComponent))
	{
		return false;
	}

	if (!AnchorComponent->IsAnchorValid())
	{
		return false;
	}

	XrSpaceEraseInfoFB EraseInfo;
	EraseInfo.type = XR_TYPE_SPACE_ERASE_INFO_FB;
	EraseInfo.space = (XrSpace)AnchorComponent->GetAnchorHandle().GetValue();
	EraseInfo.location = (XrSpaceStorageLocationFB)SaveLocation;
	EraseInfo.next = nullptr;

	XrAsyncRequestIdFB AsyncTaskId = 0;
	PFN_xrEraseSpaceFB EraseSpaceFB;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrEraseSpaceFB", (PFN_xrVoidFunction*)&EraseSpaceFB));
	XrResult Result = EraseSpaceFB(YvrXRHMD->GetSession(), &EraseInfo, &AsyncTaskId);

	if (XR_FAILED(Result))
	{
		return false;
	}

	FAnchorEraseInfo TaskInfo;
	TaskInfo.AsyncTaskId = AsyncTaskId;
	TaskInfo.Delegate = Delegate;
	TaskInfo.AnchorComponent = AnchorComponent;
	EraseAnchorsBindings.Add(AsyncTaskId, TaskInfo);
	return true;
}

bool FYvrAnchorManager::LoadAnchorEntity(const FYvrAnchorLoadInfo& AnchorLoadInfo, const FYvrLoadAnchorEntityDelegate& Delegate)
{
	XrSpaceQueryInfoFB LoadInfo;
	LoadInfo.type = XR_TYPE_SPACE_QUERY_INFO_FB;
	LoadInfo.queryAction = XR_SPACE_QUERY_ACTION_LOAD_FB;
	LoadInfo.maxResultCount = AnchorLoadInfo.LoadMaxCount;
	LoadInfo.timeout = static_cast<double>(AnchorLoadInfo.Timeout);
	LoadInfo.next = nullptr;
	//LoadInfo.location = (PxrSaveLocation)AnchorLoadInfo.SaveLocation;

	XrSpaceUuidFilterInfoFB IncludeUUIDFilter, ExcludeUUIDFilter;
	IncludeUUIDFilter.type = ExcludeUUIDFilter.type = XR_TYPE_SPACE_UUID_FILTER_INFO_FB;
	IncludeUUIDFilter.uuids = ExcludeUUIDFilter.uuids = nullptr;
	IncludeUUIDFilter.next = ExcludeUUIDFilter.next = nullptr;

	XrSpaceStorageLocationFilterInfoFB IncludeLocationFilter;
	IncludeLocationFilter.type = { XR_TYPE_SPACE_STORAGE_LOCATION_FILTER_INFO_FB };
	IncludeLocationFilter.location = (XrSpaceStorageLocationFB)AnchorLoadInfo.SaveLocation;
	IncludeLocationFilter.next = nullptr;

	IncludeUUIDFilter.next = &IncludeLocationFilter;

	XrSpaceComponentFilterInfoFB IncludeSpatialSceneFilter, ExcludeSpatialSceneFilter;
	IncludeSpatialSceneFilter.type = ExcludeSpatialSceneFilter.type = XR_TYPE_SPACE_COMPONENT_FILTER_INFO_FB;
	IncludeSpatialSceneFilter.next = ExcludeSpatialSceneFilter.next = nullptr;

	if (AnchorLoadInfo.IncludeSpatialSceneDataTypeFilter != EYvrSpatialSceneDataTypeFlag::SpatialSceneDataTypeFlag_Max)
	{
		IncludeSpatialSceneFilter.componentType = (XrSpaceComponentTypeFB)AnchorLoadInfo.IncludeSpatialSceneDataTypeFilter;
		IncludeLocationFilter.next = &IncludeSpatialSceneFilter;
	}
	if (AnchorLoadInfo.ExcludeSpatialSceneDataTypeFilter != EYvrSpatialSceneDataTypeFlag::SpatialSceneDataTypeFlag_Max)
	{
		ExcludeSpatialSceneFilter.componentType = (XrSpaceComponentTypeFB)AnchorLoadInfo.ExcludeSpatialSceneDataTypeFilter;
		ExcludeUUIDFilter.next = &ExcludeSpatialSceneFilter;
	}

	// uuid
	IncludeUUIDFilter.uuidCount = AnchorLoadInfo.IncludeUUIDFilter.Num();
	if (IncludeUUIDFilter.uuidCount > 0)
	{
		IncludeUUIDFilter.uuids = new XrUuidEXT[IncludeUUIDFilter.uuidCount];
		for (uint32_t Index = 0; Index < IncludeUUIDFilter.uuidCount; ++Index)
		{
			FMemory::Memcpy(IncludeUUIDFilter.uuids[Index].data, AnchorLoadInfo.IncludeUUIDFilter[Index].UUIDArray);
		}
	}

	ExcludeUUIDFilter.uuidCount = AnchorLoadInfo.ExcludeUUIDFilter.Num();
	if (ExcludeUUIDFilter.uuidCount > 0)
	{
		ExcludeUUIDFilter.uuids = new XrUuidEXT[ExcludeUUIDFilter.uuidCount];
		for (uint32_t Index = 0; Index < ExcludeUUIDFilter.uuidCount; ++Index)
		{
			FMemory::Memcpy(ExcludeUUIDFilter.uuids[Index].data, AnchorLoadInfo.ExcludeUUIDFilter[Index].UUIDArray);
		}
	}

	LoadInfo.filter = (const XrSpaceFilterInfoBaseHeaderFB*)(&IncludeUUIDFilter);
	LoadInfo.excludeFilter = (const XrSpaceFilterInfoBaseHeaderFB*)(&ExcludeUUIDFilter);

	XrAsyncRequestIdFB AsyncTaskId = 0;
	PFN_xrQuerySpacesFB QuerySpaceFB;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrQuerySpacesFB", (PFN_xrVoidFunction*)&QuerySpaceFB));
	XrResult Result = QuerySpaceFB(YvrXRHMD->GetSession(), (const XrSpaceQueryInfoBaseHeaderFB*)&LoadInfo, &AsyncTaskId);

	if (IncludeUUIDFilter.uuids)
	{
		delete[] IncludeUUIDFilter.uuids;
	}
	if (ExcludeUUIDFilter.uuids)
	{
		delete[] ExcludeUUIDFilter.uuids;
	}

	if (XR_FAILED(Result))
	{
		return false;
	}

	FAnchorLoadInfo TaskInfo;
	TaskInfo.AsyncTaskId = AsyncTaskId;
	TaskInfo.Delegate = Delegate;
	LoadAnchorsBindings.Add(AsyncTaskId, TaskInfo);

	return true;
}

bool FYvrAnchorManager::StartSpatialSceneCapture(const FYvrStartSpatialSceneCaptureDelegate& Delegate)
{
	XrSceneCaptureRequestInfoFB SceneCaptureRequestInfo;
	SceneCaptureRequestInfo.type = XR_TYPE_SCENE_CAPTURE_REQUEST_INFO_FB;
	SceneCaptureRequestInfo.requestByteCount = 0;
	SceneCaptureRequestInfo.request = nullptr;
	SceneCaptureRequestInfo.next = nullptr;

	XrAsyncRequestIdFB AsyncTaskId = 0;
	PFN_xrRequestSceneCaptureFB RequestSceneCaptureFB;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrRequestSceneCaptureFB", (PFN_xrVoidFunction*)&RequestSceneCaptureFB));

	XrResult Result = RequestSceneCaptureFB(YvrXRHMD->GetSession(), &SceneCaptureRequestInfo, &AsyncTaskId);

	if (XR_FAILED(Result))
	{
		return false;
	}

	FStartSpatialSceneCaptureInfo TaskInfo;
	TaskInfo.AsyncTaskId = AsyncTaskId;
	TaskInfo.Delegate = Delegate;
	StartSpatialSceneCaptureBindings.Add(AsyncTaskId, TaskInfo);
	return true;
}

bool FYvrAnchorManager::GetAnchorEntityUUID(AActor* BoundActor, FYvrAnchorUUID& OutAnchorUUID)
{
	if (!IsAnchorValid(BoundActor))
	{
		return false;
	}

	UYvrAnchorComponent* AnchorComponent = GetAnchorComponent(BoundActor);
	if (!IsAnchorValid(AnchorComponent))
	{
		return false;
	}

	OutAnchorUUID = AnchorComponent->GetAnchorUUID();

	return true;
}

bool FYvrAnchorManager::GetAnchorPose(UYvrAnchorComponent* AnchorComponent, FTransform& OutAnchorPose)
{
	if (!IsAnchorValid(AnchorComponent))
	{
		return false;
	}

	FYvrAnchor AnchorHandle = AnchorComponent->GetAnchorHandle();

	XrSpaceLocation DeviceLocation = { XR_TYPE_SPACE_LOCATION, nullptr };
	XrSpace BaseSpace = YvrXRHMD->GetTrackingSpace();
	XrSpace DeviceSpace = (XrSpace)AnchorHandle.GetValue();
	XrTime TargetTime = YvrXRHMD->GetDisplayTime();
	XrResult Result = xrLocateSpace(DeviceSpace, BaseSpace, TargetTime, &DeviceLocation);

	if (XR_FAILED(Result))
	{
		return false;
	}

	float WorldToMetersScale = AnchorComponent->GetWorld()->GetWorldSettings()->WorldToMeters;

	FTransform UnrealPose = ToFTransform(DeviceLocation.pose, WorldToMetersScale);
	FTransform TrackingToWorld = YvrXRHMD->GetTrackingToWorldTransform();

	OutAnchorPose.SetLocation(TrackingToWorld.TransformPosition(UnrealPose.GetLocation()));
	OutAnchorPose.SetRotation(TrackingToWorld.TransformRotation(UnrealPose.GetRotation()));

	return true;
}

bool FYvrAnchorManager::GetAnchorBoundingBox2D(AActor* BoundActor, FBox2D& OutAnchorBoundingBox)
{
	if (!IsAnchorValid(BoundActor))
	{
		return false;
	}

	UYvrAnchorComponent* AnchorComponent = GetAnchorComponent(BoundActor);

	if (!IsAnchorValid(AnchorComponent))
	{
		return false;
	}

	float WorldToMetersScale = BoundActor->GetWorld()->GetWorldSettings()->WorldToMeters;
	FYvrAnchor AnchorHandle = AnchorComponent->GetAnchorHandle();

	PFN_xrGetSpaceBoundingBox2DFB GetSpaceBoundingBox2DFB;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrGetSpaceBoundingBox2DFB", (PFN_xrVoidFunction*)&GetSpaceBoundingBox2DFB));
	XrRect2Df Boundingbox2D;
	memset(&Boundingbox2D, 0, sizeof(Boundingbox2D));
	XrResult Result = GetSpaceBoundingBox2DFB(YvrXRHMD->GetSession(), (XrSpace)AnchorHandle.GetValue(), &Boundingbox2D);
	if (XR_FAILED(Result))
	{
		return false;
	}

	OutAnchorBoundingBox = ToFBox2D(Boundingbox2D, WorldToMetersScale);

	return true;
}

bool FYvrAnchorManager::GetAnchorBoundingBox3D(AActor* BoundActor, FBox& OutAnchorBoundingBox)
{
	if (!IsAnchorValid(BoundActor))
	{
		return false;
	}

	UYvrAnchorComponent* AnchorComponent = GetAnchorComponent(BoundActor);

	if (!IsAnchorValid(AnchorComponent))
	{
		return false;
	}
	float WorldToMetersScale = BoundActor->GetWorld()->GetWorldSettings()->WorldToMeters;
	FYvrAnchor AnchorHandle = AnchorComponent->GetAnchorHandle();

	PFN_xrGetSpaceBoundingBox3DFB GetSpaceBoundingBox3DFB;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrGetSpaceBoundingBox3DFB", (PFN_xrVoidFunction*)&GetSpaceBoundingBox3DFB));
	XrRect3DfFB Boundingbox3D;
	memset(&Boundingbox3D, 0, sizeof(Boundingbox3D));
	XrResult Result = GetSpaceBoundingBox3DFB(YvrXRHMD->GetSession(), (XrSpace)AnchorHandle.GetValue(), &Boundingbox3D);
	if (XR_FAILED(Result))
	{
		return false;
	}
	OutAnchorBoundingBox = ToFBox(Boundingbox3D, WorldToMetersScale);

	return true;
}

bool FYvrAnchorManager::GetAnchorSemanticLabels(AActor* BoundActor, TArray<FString>& OutLabels)
{
	if (!IsAnchorValid(BoundActor))
	{
		return false;
	}

	UYvrAnchorComponent* AnchorComponent = GetAnchorComponent(BoundActor);

	if (!IsAnchorValid(AnchorComponent))
	{
		return false;
	}

	FYvrAnchor AnchorHandle = AnchorComponent->GetAnchorHandle();

	PFN_xrGetSpaceSemanticLabelsFB GetSpaceSemanticLabelsFB;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrGetSpaceSemanticLabelsFB", (PFN_xrVoidFunction*)&GetSpaceSemanticLabelsFB));


	XrSemanticLabelsFB SemanticLabelsCount;
	SemanticLabelsCount.type = { XR_TYPE_SEMANTIC_LABELS_FB };
	SemanticLabelsCount.next = nullptr;
	SemanticLabelsCount.bufferCapacityInput = 0;
	SemanticLabelsCount.bufferCountOutput = 0;
	GetSpaceSemanticLabelsFB(YvrXRHMD->GetSession(), (XrSpace)AnchorHandle.GetValue(), &SemanticLabelsCount);

	TArray<char> LabelResults;
	LabelResults.AddZeroed(SemanticLabelsCount.bufferCountOutput);
	XrSemanticLabelsFB SemanticLabels;

	SemanticLabels.type = { XR_TYPE_SEMANTIC_LABELS_FB };
	SemanticLabels.next = nullptr;
	SemanticLabels.bufferCapacityInput = SemanticLabelsCount.bufferCountOutput;
	SemanticLabels.bufferCountOutput = 0;
	SemanticLabels.buffer = LabelResults.GetData();
	GetSpaceSemanticLabelsFB(YvrXRHMD->GetSession(), (XrSpace)AnchorHandle.GetValue(), &SemanticLabels);

	// Split string
	FString SubString = "";
	for (int i = 0; i < LabelResults.Num(); ++i)
	{
		if (LabelResults[i] == ',')
		{
			OutLabels.Add(SubString);
			SubString = "";
		}
		else
		{
			SubString.AppendChar(LabelResults[i]);
		}
	}

	OutLabels.Add(SubString);

	return true;
}

bool FYvrAnchorManager::UpdateAnchor(UYvrAnchorComponent* AnchorComponent)
{
	FTransform AnchorTransform;
	if (!GetAnchorPose(AnchorComponent, AnchorTransform))
	{
		return false;
	}

	AActor* BoundActor = AnchorComponent->GetOwner();
	bool Result = BoundActor->SetActorLocationAndRotation(AnchorTransform.GetLocation(), AnchorTransform.GetRotation());
	return true;
}

void FYvrAnchorManager::HandleCreateAnchorEntityEvent(uint64_t AsyncTaskId, XrResult Result, const FYvrAnchor& AnchorHandle, const FYvrAnchorUUID& AnchorUUID)
{
	FAnchorCreateInfo* TaskInfo = CreateAnchorBindings.Find(AsyncTaskId);
	if (!TaskInfo)
	{
		return;
	}

	if (XR_FAILED(Result) || !IsValid(TaskInfo->AnchorComponent))
	{
		TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(Result), TaskInfo->AnchorComponent);
		SaveAnchorsBindings.Remove(AsyncTaskId);
		return;
	}

	TaskInfo->AnchorComponent->SetAnchorHandle(AnchorHandle);
	TaskInfo->AnchorComponent->SetAnchorUUID(AnchorUUID);
	TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(Result), TaskInfo->AnchorComponent);
	CreateAnchorBindings.Remove(AsyncTaskId);
}

void FYvrAnchorManager::HandleSetSpaceComponentStatusEvent(uint64_t AsyncTaskId, XrResult Result)
{
	FComponentStatusInfo* TaskInfo = ComponentStatusBindings.Find(AsyncTaskId);
	if (!TaskInfo)
	{
		return;
	}

	if (XR_FAILED(Result))
	{
		TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(Result), nullptr);
		ComponentStatusBindings.Remove(AsyncTaskId);
		return;
	}

	SaveAnchorEntity(TaskInfo->AnchorComponent, TaskInfo->Location, TaskInfo->Delegate);
	ComponentStatusBindings.Remove(AsyncTaskId);
}

void FYvrAnchorManager::HandleSaveAnchorEntityEvent(uint64_t AsyncTaskId, XrResult Result, EYvrSaveLocation SaveLocation)
{
	FAnchorSaveInfo* TaskInfo = SaveAnchorsBindings.Find(AsyncTaskId);
	if (!TaskInfo)
	{
		return;
	}

	if (XR_FAILED(Result))
	{
		TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(Result), nullptr);
		SaveAnchorsBindings.Remove(AsyncTaskId);
		return;
	}

	TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(Result), TaskInfo->AnchorComponent);
	SaveAnchorsBindings.Remove(AsyncTaskId);
}

void FYvrAnchorManager::HandleEraseAnchorEntityEvent(uint64_t AsyncTaskId, XrResult Result, EYvrSaveLocation SaveLocation)
{
	FAnchorEraseInfo* TaskInfo = EraseAnchorsBindings.Find(AsyncTaskId);
	if (!TaskInfo)
	{
		return;
	}

	TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(Result), TaskInfo->AnchorComponent);
	EraseAnchorsBindings.Remove(AsyncTaskId);
}

void FYvrAnchorManager::HandleLoadAnchorEntityEvent(uint64_t AsyncTaskId, XrResult Result)
{
	FAnchorLoadInfo* TaskInfo = LoadAnchorsBindings.Find(AsyncTaskId);
	if (!TaskInfo)
	{
		return;
	}

	TArray<FAnchorLoadResult> LoadedAnchors;

	if (XR_FAILED(Result))
	{
		TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(Result), LoadedAnchors);
		LoadAnchorsBindings.Remove(AsyncTaskId);
		return;
	}

	PFN_xrRetrieveSpaceQueryResultsFB RetrieveSpaceFB;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrRetrieveSpaceQueryResultsFB", (PFN_xrVoidFunction*)&RetrieveSpaceFB));

	XrSpaceQueryResultsFB ResultsCount;
	ResultsCount.type = { XR_TYPE_SPACE_QUERY_RESULTS_FB };
	ResultsCount.next = nullptr;
	ResultsCount.resultCapacityInput = 0;
	ResultsCount.resultCountOutput = 0;
	XrResult CountResult = RetrieveSpaceFB(YvrXRHMD->GetSession(), AsyncTaskId, &ResultsCount);

	TArray<XrSpaceQueryResultFB> QueryResults;
	QueryResults.AddZeroed(ResultsCount.resultCountOutput);
	XrSpaceQueryResultsFB Results;
	Results.type = { XR_TYPE_SPACE_QUERY_RESULTS_FB };
	Results.next = nullptr;
	Results.resultCapacityInput = ResultsCount.resultCountOutput;
	Results.resultCountOutput = 0;
	Results.results = reinterpret_cast<XrSpaceQueryResultFB*>(QueryResults.GetData());

	XrResult LoadResult = RetrieveSpaceFB(YvrXRHMD->GetSession(), AsyncTaskId, &Results);

	if (XR_FAILED(LoadResult))
	{
		TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(LoadResult), LoadedAnchors);
		LoadAnchorsBindings.Remove(AsyncTaskId);
		return;
	}

	uint32_t AnchorCount = Results.resultCountOutput;

	LoadedAnchors.SetNum(AnchorCount);
	for (uint32_t Index = 0; Index < AnchorCount; ++Index)
	{
		LoadedAnchors[Index].AnchorHandle = (uint64_t)Results.results[Index].space;
		LoadedAnchors[Index].AnchorUUID = Results.results[Index].uuid.data;
	}
	TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(LoadResult), LoadedAnchors);
	LoadAnchorsBindings.Remove(AsyncTaskId);
}

void FYvrAnchorManager::HandleStartSpatialSceneCaptureEvent(uint64_t AsyncTaskId, XrResult Result)
{
	FStartSpatialSceneCaptureInfo* TaskInfo = StartSpatialSceneCaptureBindings.Find(AsyncTaskId);
	if (!TaskInfo)
	{
		return;
	}

	TaskInfo->Delegate.ExecuteIfBound(ToYvrResult(Result));
	StartSpatialSceneCaptureBindings.Remove(AsyncTaskId);
}

bool FYvrAnchorManager::IsAnchorValid(AActor* BoundActor)
{
	if (!IsValid(BoundActor) || !IsValid(BoundActor->GetWorld()))
	{
		return false;
	}

	UYvrAnchorComponent* AnchorComponent = Cast<UYvrAnchorComponent>(BoundActor->GetComponentByClass(UYvrAnchorComponent::StaticClass()));
	if (!IsValid(AnchorComponent) || !AnchorComponent->IsAnchorValid())
	{
		return false;
	}
	return true;
}

bool FYvrAnchorManager::IsAnchorValid(UYvrAnchorComponent* AnchorComponent)
{
	if (!IsValid(AnchorComponent) || !AnchorComponent->IsAnchorValid())
	{
		return false;
	}
	return true;
}

UYvrAnchorComponent* FYvrAnchorManager::GetAnchorComponent(AActor* BoundActor)
{
	if (!IsValid(BoundActor) || !IsValid(BoundActor->GetWorld()))
	{
		return nullptr;
	}

	UYvrAnchorComponent* AnchorComponent = Cast<UYvrAnchorComponent>(BoundActor->GetComponentByClass(UYvrAnchorComponent::StaticClass()));
	return AnchorComponent;
}

EYvrResult FYvrAnchorManager::ToYvrResult(XrResult Result)
{
	EYvrResult YvrResult = EYvrResult::Error_Unknow;
	switch (Result)
	{
	case XrResult::XR_SUCCESS:
		YvrResult = EYvrResult::Success;
		break;
	case XrResult::XR_TIMEOUT_EXPIRED:
		YvrResult = EYvrResult::TimeoutExpired;
		break;
	case XrResult::XR_ERROR_VALIDATION_FAILURE:
		YvrResult = EYvrResult::Error_ValidationFailure;
		break;
	case XrResult::XR_ERROR_RUNTIME_FAILURE:
		YvrResult = EYvrResult::Error_RuntimeFailure;
		break;
	case XrResult::XR_ERROR_OUT_OF_MEMORY:
		YvrResult = EYvrResult::Error_OutOfMemory;
		break;
	case XrResult::XR_ERROR_API_VERSION_UNSUPPORTED:
		YvrResult = EYvrResult::Error_APIVersionUnsupported;
		break;
	case XrResult::XR_ERROR_INITIALIZATION_FAILED:
		YvrResult = EYvrResult::Error_InitializationFailed;
		break;
	case XrResult::XR_ERROR_FUNCTION_UNSUPPORTED:
		YvrResult = EYvrResult::Error_FunctionUnsupported;
		break;
	case XrResult::XR_ERROR_FEATURE_UNSUPPORTED:
		YvrResult = EYvrResult::Error_FeatureUnsupported;
		break;
	case XrResult::XR_ERROR_LIMIT_REACHED:
		YvrResult = EYvrResult::Error_LimitReached;
		break;
	case XrResult::XR_ERROR_SIZE_INSUFFICIENT:
		YvrResult = EYvrResult::Error_SizeInsufficient;
		break;
	case XrResult::XR_ERROR_HANDLE_INVALID:
		YvrResult = EYvrResult::Error_HandleInvalid;
		break;
	default:
		break;
	}

	return YvrResult;
}
