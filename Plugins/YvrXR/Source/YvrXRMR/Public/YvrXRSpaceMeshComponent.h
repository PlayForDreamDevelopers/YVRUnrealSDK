// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProceduralMeshComponent.h"

#include "YvrXRMRTypes.h"
#include "openxr.h"
#include "YvrXRCore.h"

#include "YvrXRSpaceMeshActor.h"

#include "YvrXRSpaceMeshComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YVRXRMR_API UYvrXRSpaceMeshComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UYvrXRSpaceMeshComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Mesh interface
	void UpdateMeshState(FYvrAnchor key, TArray<FVector> MyVertices, TArray<int32> MyTriangles, FTransform Base);
	void RemoveMesh(FYvrAnchor key);

	// Plane interface
	void UpdatePlaneState(uint64 key, TArray<FVector> MyVertices, TArray<int32> MyTriangles, FTransform Base);
	void RemovePlane(uint64 key);
	
	void RemoveAll();

	TMap<FYvrAnchor, AYvrXRSpaceMeshActor*> Meshes;
	TMap<uint64, AYvrXRSpaceMeshActor*> Planes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool MeshVisible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CreateCollision = false;

};
