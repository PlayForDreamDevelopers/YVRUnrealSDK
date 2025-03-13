#include "YvrXRMRFunctionLibrary.h"
#include "YvrXRAnchorManager.h"
#include "..\Public\YvrXRMRFunctionLibrary.h"

#include "YvrXrSpaceMeshManager.h"


bool UYvrXRMRFunctionLibrary::YvrXRGetAnchorEntityUuid(AActor* BoundActor, FYvrAnchorUUID& OutAnchorUUID)
{
	return FYvrAnchorManager::GetInstance()->GetAnchorEntityUUID(BoundActor, OutAnchorUUID);
}

bool UYvrXRMRFunctionLibrary::YvrXRGetAnchorPoseByComponent(UYvrAnchorComponent* BoundComponent, FTransform& OutTransform)
{
	return FYvrAnchorManager::GetInstance()->GetAnchorPose(BoundComponent, OutTransform);
}

bool UYvrXRMRFunctionLibrary::YvrXRGetAnchorPoseByActor(AActor* BoundActor, FTransform& OutTransform)
{
	if (!IsValid(BoundActor) || !IsValid(BoundActor->GetWorld()))
	{
		return false;
	}

	UYvrAnchorComponent* AnchorComponent = Cast<UYvrAnchorComponent>(BoundActor->GetComponentByClass(UYvrAnchorComponent::StaticClass()));
	return FYvrAnchorManager::GetInstance()->GetAnchorPose(AnchorComponent, OutTransform);
}

AActor* UYvrXRMRFunctionLibrary::YvrXRSpawnActorFromLoadResult(UObject* WorldContext, const FAnchorLoadResult& LoadResult, UClass* ActorClass)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!IsValid(World))
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.ObjectFlags |= RF_Transient;

	AActor* AnchorActor = World->SpawnActor(ActorClass, nullptr, nullptr, SpawnInfo);
	if (!IsValid(AnchorActor))
	{
		return nullptr;
	}

	UYvrAnchorComponent* AnchorComponent = Cast<UYvrAnchorComponent>(AnchorActor->GetComponentByClass(UYvrAnchorComponent::StaticClass()));
	if (IsValid(AnchorComponent) && AnchorComponent->IsAnchorValid())
	{
		return AnchorActor;
	}
	if (AnchorComponent == nullptr)
	{
		AnchorComponent = Cast<UYvrAnchorComponent>(AnchorActor->AddComponentByClass(UYvrAnchorComponent::StaticClass(), false, FTransform::Identity, false));
	}

	AnchorComponent->SetAnchorHandle(LoadResult.AnchorHandle);
	AnchorComponent->SetAnchorUUID(LoadResult.AnchorUUID);
	return AnchorActor;
}

bool UYvrXRMRFunctionLibrary::YvrXRIsAnchorValidForActor(AActor* BoundActor)
{
	if (!IsValid(BoundActor))
	{
		return false;
	}

	UYvrAnchorComponent* AnchorComponent = Cast<UYvrAnchorComponent>(BoundActor->GetComponentByClass(UYvrAnchorComponent::StaticClass()));
	if (IsValid(AnchorComponent) && AnchorComponent->IsAnchorValid())
	{
		return true;
	}

	return false;
}

bool UYvrXRMRFunctionLibrary::YvrXRIsAnchorValidForComponent(UActorComponent* BoundComponent)
{
	if (!IsValid(BoundComponent) || !BoundComponent->IsA(UYvrAnchorComponent::StaticClass()))
	{
		return false;
	}

	UYvrAnchorComponent* AnchorComponent = Cast<UYvrAnchorComponent>(BoundComponent);
	if (IsValid(AnchorComponent) && AnchorComponent->IsAnchorValid())
	{
		return true;
	}

	return false;
}

FString UYvrXRMRFunctionLibrary::YvrXRFromAnchorToString(const FYvrAnchor& Anchor)
{
	return Anchor.ToString();
}

FString UYvrXRMRFunctionLibrary::YvrXRFromUUIDToString(const FYvrAnchorUUID& AnchorUUID)
{
	return AnchorUUID.ToString();
}

bool UYvrXRMRFunctionLibrary::YvrXRGetAnchorBoundingBox2D(AActor* BoundActor, FBox2D& BoundingBox)
{
	return FYvrAnchorManager::GetInstance()->GetAnchorBoundingBox2D(BoundActor, BoundingBox);
}

bool UYvrXRMRFunctionLibrary::YvrXRGetAnchorBoundingBox3D(AActor* BoundActor, FBox& BoundingBox)
{
	return FYvrAnchorManager::GetInstance()->GetAnchorBoundingBox3D(BoundActor, BoundingBox);
}

bool UYvrXRMRFunctionLibrary::YvrXRGetAnchorSemanticLabels(AActor* BoundActor, TArray<FString>& Labels)
{
	return FYvrAnchorManager::GetInstance()->GetAnchorSemanticLabels(BoundActor, Labels);
}


bool UYvrXRMRFunctionLibrary::YvrXRCreateMeshDetector(UYvrXRSpaceMeshComponent* SpaceMeshComponent, UYvrXRSpaceMeshComponent* SpacePlaneComponent)
{
	return FYvrXRSpaceMeshManager::GetInstance()->CreateMeshDetector(SpaceMeshComponent, SpacePlaneComponent);
}

bool UYvrXRMRFunctionLibrary::YvrXRDestroyMeshDetector(UYvrXRSpaceMeshComponent* SpaceMeshComponent)
{
	return FYvrXRSpaceMeshManager::GetInstance()->DestroyMeshDetector(SpaceMeshComponent);
}