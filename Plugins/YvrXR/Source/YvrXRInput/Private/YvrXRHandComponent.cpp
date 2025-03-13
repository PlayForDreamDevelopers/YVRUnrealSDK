// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.
#include "YvrXRHandComponent.h"
#include "YvrXRInput.h"

#include "Engine/SkeletalMesh.h"
#include "Components/InputComponent.h"
#include "Materials/MaterialInterface.h"

#include "GameFramework/PlayerController.h"

UYvrXRHandComponent::UYvrXRHandComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;

	bAutoActivate = true;
	bWantsInitializeComponent = true;

	for (uint8 BoneIndex = 0; BoneIndex < (uint8)EYvrXRHandJoint::HandJointMax; BoneIndex++)
	{
		BoneNameMappings.Add((EYvrXRHandJoint)BoneIndex, TEXT(""));
	}
}

void UYvrXRHandComponent::BeginPlay()
{
	Super::BeginPlay();
	// Use custom mesh if a skeletal mesh is already set, else try to load the runtime mesh
#if ENGINE_MAJOR_VERSION > 4
	if (GetSkinnedAsset())
#else
	if (SkeletalMesh)
#endif
	{
		bCustomHandMesh = true;
	}
	//Hide Component if HandTracking is Disabled
	const bool bStartHidden = UYvrXRInputFunctionLibrary::IsHandTrackingEnabled() ? false : true;
	SetHiddenInGame(bStartHidden, true);
}

void UYvrXRHandComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bHidden = false;
	if (UYvrXRInputFunctionLibrary::IsHandTrackingEnabled())
	{
		// Update Visibility based on Confidence
		if (ConfidenceBehavior == EYvrConfidenceBehavior::HideActor)
		{
			EYvrXRHandTrackingConfidence TrackingConfidence = UYvrXRInputFunctionLibrary::GetTrackingConfidence(SkeletonType);
			bHidden |= TrackingConfidence != EYvrXRHandTrackingConfidence::High;
		}

		if (bUpdateHandScale)
		{
			const float NewScale = UYvrXRInputFunctionLibrary::GetHandScale(SkeletonType);
			SetRelativeScale3D(FVector(NewScale));
		}

#if ENGINE_MAJOR_VERSION > 4
		if(GetSkinnedAsset())
#else
		if (SkeletalMesh)
#endif
		{
			UpdateBonePose();
			UpdateHandTransform();
		}
	}
	else
	{
		bHidden = true;
	}

	if (bHidden != bHiddenInGame)
	{
		SetHiddenInGame(bHidden, true);
	}
}

void UYvrXRHandComponent::UpdateBonePose()
{
	if (bCustomHandMesh)
	{
		for (auto& BoneElem : BoneNameMappings)
		{
#if ENGINE_MAJOR_VERSION > 4
			const int32 BoneIndex = GetSkinnedAsset()->GetRefSkeleton().FindBoneIndex(BoneElem.Value);
#elif ENGINE_MINOR_VERSION > 26
			const int32 BoneIndex = SkeletalMesh->GetRefSkeleton().FindBoneIndex(BoneElem.Value);
#else
			const int32 BoneIndex = SkeletalMesh->RefSkeleton.FindBoneIndex(BoneElem.Value);
#endif
			if (BoneIndex >= 0)
			{
				FQuat BoneRotation = UYvrXRInputFunctionLibrary::GetBoneRotation(SkeletonType, BoneElem.Key);
				if (!BoneRotation.IsIdentity() && BoneRotation.IsNormalized())
				{
					SetBoneRotationByName(BoneElem.Value, BoneRotation.Rotator(), EBoneSpaces::ComponentSpace);
				}
			}
		}
	}

	MarkRefreshTransformDirty();
}

void UYvrXRHandComponent::UpdateHandTransform()
{
	const FTransform HandPose = FTransform(UYvrXRInputFunctionLibrary::GetBoneRotation(SkeletonType, EYvrXRHandJoint::Wrist), UYvrXRInputFunctionLibrary::GetBoneLocation(SkeletonType, EYvrXRHandJoint::Wrist));

	if (HandPose.IsValid() && !HandPose.Equals(FTransform()))
	{
		if (!HandPose.GetLocation().ContainsNaN())
		{
			this->SetRelativeLocation(HandPose.GetLocation());
		}
	}
}