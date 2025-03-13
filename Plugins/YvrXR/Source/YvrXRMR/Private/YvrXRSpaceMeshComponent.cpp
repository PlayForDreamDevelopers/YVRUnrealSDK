// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRSpaceMeshComponent.h"
#include "openxr.h"

// Sets default values for this component's properties
UYvrXRSpaceMeshComponent::UYvrXRSpaceMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UYvrXRSpaceMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UYvrXRSpaceMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UYvrXRSpaceMeshComponent::UpdateMeshState(FYvrAnchor key, TArray<FVector> MyVertices, TArray<int32> MyTriangles, FTransform Base)
{
	if (Meshes.Contains(key))
	{
		Meshes[key]->SetActorTransform(Base);
		Meshes[key]->UpdateMesh(MyVertices, MyTriangles, MeshVisible, CreateCollision);
	}
	else
	{
		FActorSpawnParameters SpawnInfo;
		AYvrXRSpaceMeshActor* MyActor = (AYvrXRSpaceMeshActor*)GetWorld()->SpawnActor<AYvrXRSpaceMeshActor>();
		MyActor->SetActorTransform(Base);
		MyActor->UpdateMesh(MyVertices, MyTriangles, MeshVisible, CreateCollision);
		Meshes.Add(key, MyActor);
	}
}

void UYvrXRSpaceMeshComponent::UpdatePlaneState(uint64 key, TArray<FVector> MyVertices, TArray<int32> MyTriangles, FTransform Base)
{
	if (Planes.Contains(key))
	{
		Planes[key]->SetActorTransform(Base);
		Planes[key]->UpdateMesh(MyVertices, MyTriangles, MeshVisible, CreateCollision);

	}
	else
	{
		FActorSpawnParameters SpawnInfo;
		AYvrXRSpaceMeshActor* MyActor = (AYvrXRSpaceMeshActor*)GetWorld()->SpawnActor<AYvrXRSpaceMeshActor>();
		MyActor->SetActorTransform(Base);
		MyActor->UpdateMesh(MyVertices, MyTriangles, MeshVisible, CreateCollision);
		Planes.Add(key, MyActor);
	}
}

void UYvrXRSpaceMeshComponent::RemovePlane(uint64 key)
{
	if (Planes.Contains(key))
	{
		Planes[key]->Destroy();
		Planes.Remove(key);
	}
}

void UYvrXRSpaceMeshComponent::RemoveMesh(FYvrAnchor key)
{
	if (Meshes.Contains(key))
	{
		Meshes[key]->Destroy();
		Meshes.Remove(key);
	}
}

void UYvrXRSpaceMeshComponent::RemoveAll()
{
	for (TMap<FYvrAnchor, AYvrXRSpaceMeshActor*>::TConstIterator iter = Meshes.CreateConstIterator(); iter; ++iter)
	{
		iter->Value->Destroy();
	}
	Meshes.Empty();

	for (TMap<uint64, AYvrXRSpaceMeshActor*>::TConstIterator iter = Planes.CreateConstIterator(); iter; ++iter)
	{
		iter->Value->Destroy();
	}
	Planes.Empty();
}