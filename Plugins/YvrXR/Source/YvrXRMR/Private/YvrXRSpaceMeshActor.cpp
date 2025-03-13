// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRSpaceMeshActor.h"

// Sets default values
AYvrXRSpaceMeshActor::AYvrXRSpaceMeshActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MeshScene"));
	RootComponent = SceneComponent;
	SceneComponent->SetVisibility(true);
}

// Called when the game starts or when spawned
void AYvrXRSpaceMeshActor::BeginPlay()
{
	Super::BeginPlay();
}


void AYvrXRSpaceMeshActor::UpdateMesh(TArray<FVector> Vertices, TArray<int32> Triangles,  bool bMeshVisible, bool bCreateCollision)
{
	if (!ProceduralMesh)
	{
		ProceduralMesh = Cast<UProceduralMeshComponent>(this->AddComponentByClass(UProceduralMeshComponent::StaticClass(), false, FTransform::Identity, false));
		ProceduralMesh->SetupAttachment(RootComponent);
		ProceduralMesh->RegisterComponent();
		UMaterial* MeshMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("Material'/YvrXR/Materials/MeshMaterial.MeshMaterial'")));
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(MeshMaterial, NULL);
		ProceduralMesh->SetMaterial(0, DynamicMaterial);
	}
	else
	{
		ProceduralMesh->ClearMeshSection(0);
	}

	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;
	
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV, VertexColors, Tangents, bCreateCollision);
	ProceduralMesh->SetMeshSectionVisible(0, bMeshVisible);
}

// Called every frame
void AYvrXRSpaceMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

