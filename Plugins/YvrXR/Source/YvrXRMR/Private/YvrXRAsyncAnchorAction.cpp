#include "YvrXRAsyncAnchorAction.h"
#include "YvrXRAnchorManager.h"

//////////////////////////////////////////////////////////////////////////
/// Create Anchor Entity
//////////////////////////////////////////////////////////////////////////
void UYvrCreateAnchorEntity_AsyncAction::Activate()
{
	if (!IsValid(BindingActor))
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
		return;
	}

	bool bStarted = FYvrAnchorManager::GetInstance()->CreateAnchorEntity(
		BindingActor,
		AnchorEntityTransform,
		Timeout,
		FYvrCreateAnchorEntityDelegate::CreateUObject(this, &UYvrCreateAnchorEntity_AsyncAction::HandleCreateAnchorEntityComplete)
	);

	if (!bStarted)
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
	}
}

UYvrCreateAnchorEntity_AsyncAction* UYvrCreateAnchorEntity_AsyncAction::YvrXRCreateAnchorEntity_Async(AActor* InBindingActor, const FTransform& InAnchorEntityTransform, float InTimeout)
{
	UYvrCreateAnchorEntity_AsyncAction* Action = NewObject<UYvrCreateAnchorEntity_AsyncAction>();
	Action->BindingActor = InBindingActor;
	Action->AnchorEntityTransform = InAnchorEntityTransform;
	Action->Timeout = InTimeout;

	if (IsValid(InBindingActor))
	{
		Action->RegisterWithGameInstance(InBindingActor->GetWorld());
	}
	else
	{
		Action->RegisterWithGameInstance(GWorld);
	}

	return Action;
}

void UYvrCreateAnchorEntity_AsyncAction::HandleCreateAnchorEntityComplete(EYvrResult Result, UYvrAnchorComponent* AnchorEntityComponent)
{
	if (YVRXR_SUCCESS(Result))
	{
		OnSuccess.Broadcast(Result, AnchorEntityComponent);
	}
	else
	{
		OnFailure.Broadcast(Result);
	}

	SetReadyToDestroy();
}

//////////////////////////////////////////////////////////////////////////
/// Save Anchor Entity
//////////////////////////////////////////////////////////////////////////
void UYvrSaveAnchorEntity_AsyncAction::Activate()
{
	if (!IsValid(BindingActor))
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
		return;
	}

	UYvrAnchorComponent* AnchorComponent = BindingActor->FindComponentByClass<UYvrAnchorComponent>();
	if (AnchorComponent == nullptr)
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
		return;
	}


	bool bStarted = FYvrAnchorManager::GetInstance()->SetSpaceComponentStatus(
		AnchorComponent,
		SaveLocation,
		FYvrSaveAnchorEntityDelegate::CreateUObject(this, &UYvrSaveAnchorEntity_AsyncAction::HandleSaveAnchorEntityComplete)
	);

	if (!bStarted)
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
	}
}

UYvrSaveAnchorEntity_AsyncAction* UYvrSaveAnchorEntity_AsyncAction::YvrXRSaveAnchorEntity_Async(AActor* InBindingActor, EYvrSaveLocation InSaveLocation)
{
	UYvrSaveAnchorEntity_AsyncAction* Action = NewObject<UYvrSaveAnchorEntity_AsyncAction>();
	Action->BindingActor = InBindingActor;
	Action->SaveLocation = InSaveLocation;

	if (IsValid(InBindingActor))
	{
		Action->RegisterWithGameInstance(InBindingActor->GetWorld());
	}
	else
	{
		Action->RegisterWithGameInstance(GWorld);
	}

	return Action;
}

void UYvrSaveAnchorEntity_AsyncAction::HandleSaveAnchorEntityComplete(EYvrResult Result, UYvrAnchorComponent* Anchor)
{
	if (YVRXR_SUCCESS(Result))
	{
		OnSuccess.Broadcast(Result, Anchor);
	}
	else
	{
		OnFailure.Broadcast(Result);
	}

	SetReadyToDestroy();
}


//////////////////////////////////////////////////////////////////////////
/// Erase Anchor Entity
//////////////////////////////////////////////////////////////////////////
void UYvrEraseAnchorEntity_AsyncAction::Activate()
{
	if (!IsValid(BindingActor))
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
		return;
	}

	UYvrAnchorComponent* AnchorComponent = BindingActor->FindComponentByClass<UYvrAnchorComponent>();
	if (AnchorComponent == nullptr)
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
		return;
	}

	bool bStarted = FYvrAnchorManager::GetInstance()->EraseAnchorEntity(
		AnchorComponent,
		SaveLocation,
		FYvrEraseAnchorEntityDelegate::CreateUObject(this, &UYvrEraseAnchorEntity_AsyncAction::HandleEraseAnchorEntityComplete)
	);

	if (!bStarted)
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
	}
}

UYvrEraseAnchorEntity_AsyncAction* UYvrEraseAnchorEntity_AsyncAction::YvrXREraseAnchorEntity_Async(AActor* InBindingActor, EYvrSaveLocation InSaveLocation)
{
	UYvrEraseAnchorEntity_AsyncAction* Action = NewObject<UYvrEraseAnchorEntity_AsyncAction>();
	Action->BindingActor = InBindingActor;
	Action->SaveLocation = InSaveLocation;

	if (IsValid(InBindingActor))
	{
		Action->RegisterWithGameInstance(InBindingActor->GetWorld());
	}
	else
	{
		Action->RegisterWithGameInstance(GWorld);
	}

	return Action;
}

void UYvrEraseAnchorEntity_AsyncAction::HandleEraseAnchorEntityComplete(EYvrResult Result, UYvrAnchorComponent* Anchor)
{
	if (YVRXR_SUCCESS(Result))
	{
		OnSuccess.Broadcast(Result, Anchor);
	}
	else
	{
		OnFailure.Broadcast(Result);
	}

	SetReadyToDestroy();
}

//////////////////////////////////////////////////////////////////////////
/// Load Anchor Entity
//////////////////////////////////////////////////////////////////////////
void UYvrLoadAnchorEntity_AsyncAction::Activate()
{
	bool bStarted = FYvrAnchorManager::GetInstance()->LoadAnchorEntity(
		LoadInfo,
		FYvrLoadAnchorEntityDelegate::CreateUObject(this, &UYvrLoadAnchorEntity_AsyncAction::HandleLoadAnchorEntityComplete)
	);

	if (!bStarted)
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
	}
}

UYvrLoadAnchorEntity_AsyncAction* UYvrLoadAnchorEntity_AsyncAction::YvrXRLoadAnchorEntity_Async(const FYvrAnchorLoadInfo& InLoadInfo)
{
	UYvrLoadAnchorEntity_AsyncAction* Action = NewObject<UYvrLoadAnchorEntity_AsyncAction>();
	Action->LoadInfo = InLoadInfo;
	Action->RegisterWithGameInstance(GWorld);
	return Action;
}

void UYvrLoadAnchorEntity_AsyncAction::HandleLoadAnchorEntityComplete(EYvrResult Result, const TArray<FAnchorLoadResult>& AnchorLoadResults)
{
	if (YVRXR_SUCCESS(Result))
	{
		OnSuccess.Broadcast(Result, AnchorLoadResults);
	}
	else
	{
		OnFailure.Broadcast(Result);
	}

	SetReadyToDestroy();
}


//////////////////////////////////////////////////////////////////////////
/// Load Anchor Entity
//////////////////////////////////////////////////////////////////////////
void UYvrStartSpatialSceneCapture_AsyncAction::Activate()
{
	bool bStarted = FYvrAnchorManager::GetInstance()->StartSpatialSceneCapture(
		FYvrStartSpatialSceneCaptureDelegate::CreateUObject(this, &UYvrStartSpatialSceneCapture_AsyncAction::HandleStartSpatialSceneCaptureComplete)
	);

	if (!bStarted)
	{
		OnFailure.Broadcast(EYvrResult::Error_ValidationFailure);
	}
}

UYvrStartSpatialSceneCapture_AsyncAction* UYvrStartSpatialSceneCapture_AsyncAction::YvrXRStartSpatialSceneCapture_Async()
{
	UYvrStartSpatialSceneCapture_AsyncAction* Action = NewObject<UYvrStartSpatialSceneCapture_AsyncAction>();
	Action->RegisterWithGameInstance(GWorld);
	return Action;
}

void UYvrStartSpatialSceneCapture_AsyncAction::HandleStartSpatialSceneCaptureComplete(EYvrResult Result)
{
	if (YVRXR_SUCCESS(Result))
	{
		OnSuccess.Broadcast(Result);
	}
	else
	{
		OnFailure.Broadcast(Result);
	}

	SetReadyToDestroy();
}
