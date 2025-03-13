// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "YvrXRMRTypes.h"
#include "YvrXRAnchorComponent.h"


#include "YvrXRSpaceMeshComponent.h"


#include "YvrXRMRFunctionLibrary.generated.h"



UCLASS()
class YVRXRMR_API UYvrXRMRFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|MR")
	static bool YvrXRGetAnchorEntityUuid(AActor* BoundActor, FYvrAnchorUUID& OutAnchorUUID);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|MR")
	static bool YvrXRGetAnchorPoseByComponent(UYvrAnchorComponent* BoundComponent, FTransform& OutTransform);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|MR")
	static bool YvrXRGetAnchorPoseByActor(AActor* BoundActor, FTransform& OutTransform);

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR", meta = (WorldContext = "WorldContext", UnsafeDuringActorConstruction = "true"))
	static AActor* YvrXRSpawnActorFromLoadResult(UObject* WorldContext, const FAnchorLoadResult& LoadResult, UClass* ActorClass);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|MR")
	static bool YvrXRIsAnchorValidForActor(AActor* BoundActor);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|MR")
	static bool YvrXRIsAnchorValidForComponent(UActorComponent* BoundComponent);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (Anchor Handle)", CompactNodeTitle = "->", BlueprintAutocast), Category = "YvrXRLibrary|MR")
	static FString YvrXRFromAnchorToString(const FYvrAnchor& Anchor);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (Anchor UUID)", CompactNodeTitle = "->", BlueprintAutocast), Category = "YvrXRLibrary|MR")
	static FString YvrXRFromUUIDToString(const FYvrAnchorUUID& AnchorUUID);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|MR")
	static bool YvrXRGetAnchorBoundingBox2D(AActor* BoundActor, FBox2D& BoundingBox);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|MR")
	static bool YvrXRGetAnchorBoundingBox3D(AActor* BoundActor, FBox& BoundingBox);

	UFUNCTION(BlueprintPure, Category = "YvrXRLibrary|MR")
	static bool YvrXRGetAnchorSemanticLabels(AActor* BoundActor, TArray<FString>& Labels);

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR")
	static bool YvrXRCreateMeshDetector(UYvrXRSpaceMeshComponent* SpaceMeshComponent, UYvrXRSpaceMeshComponent* SpacePlaneComponent);

	UFUNCTION(BlueprintCallable, Category = "YvrXRLibrary|MR")
	static bool YvrXRDestroyMeshDetector(UYvrXRSpaceMeshComponent* SpaceMeshComponent);

};
