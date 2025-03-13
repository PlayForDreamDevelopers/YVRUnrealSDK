// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "YvrXRInputFunctionLibrary.h"
#include "Components/PoseableMeshComponent.h"
#include "YvrXRHandComponent.generated.h"

UENUM(BlueprintType)
enum class EYvrConfidenceBehavior : uint8
{
	None,
	HideActor
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent), ClassGroup = YvrXRHand)
class YVRXRINPUT_API UYvrXRHandComponent : public UPoseableMeshComponent
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** The hand skeleton that will be loaded */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandProperties")
	EYvrXRHandType SkeletonType;

	/** Behavior for when hand tracking loses high confidence tracking */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandProperties")
		EYvrConfidenceBehavior ConfidenceBehavior = EYvrConfidenceBehavior::HideActor;

	/** Whether or not the hand scale should update based on values from the runtime to match the users hand scale */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandProperties")
	bool bUpdateHandScale;

	/** Bone mapping for custom hand skeletal meshes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomSkeletalMesh")
	TMap<EYvrXRHandJoint, FName> BoneNameMappings;

private:
	/** Whether or not a custom hand mesh is being used */
	bool bCustomHandMesh = false;

	void UpdateBonePose();
	void UpdateHandTransform();

};