// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "YvrXRHMD.h"
#include "YvrXRMRTypes.h"
#include "Delegates/DelegateCombinations.h"
#include "YvrXRSpaceMeshComponent.h"

class YVRXRMR_API FYvrXRSpaceMeshManager
{
public:

	static FYvrXRSpaceMeshManager* GetInstance();
	void Initialize(FYvrXRHMD* InYvrXRHMD);
	bool CreateMeshDetector(UYvrXRSpaceMeshComponent* InSpaceMeshComponent, UYvrXRSpaceMeshComponent* InSpacePlaneComponent);
	bool DestroyMeshDetector(UYvrXRSpaceMeshComponent* inSpaceMeshComponent);

private:
	FYvrXRHMD* YvrXRHMD;
	EYvrResult ToYvrResult(XrResult Result);

	void PollEvent(XrEventDataBuffer EventData);
	FYvrXRSpaceMeshManager();
	~FYvrXRSpaceMeshManager();

	XrMeshDetectorYVR MeshDetector;

	UYvrXRSpaceMeshComponent* SpaceMeshComponent;
	UYvrXRSpaceMeshComponent* SpacePlaneComponent;
};
