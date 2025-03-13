// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IStereoLayers.h"
#include "ProceduralMeshComponent.h"
#include "XRSwapChain.h"
#include "YvrXRCore.h"

struct FYvrXRLayer
{
	struct FPerEyeTextureData
	{
		FXRSwapChainPtr			Swapchain = nullptr;
		FVector2D				SwapchainSize{};
		bool					bUpdateTexture = false;

		void					SetSwapchain(FXRSwapChainPtr InSwapchain, const FVector2D& InSwapchainSize)
		{
			Swapchain = InSwapchain;
			SwapchainSize = InSwapchainSize;
			bUpdateTexture = true;
		}
	};

	IStereoLayers::FLayerDesc	Desc;

	/** Texture tracking data for the right eye.*/
	FPerEyeTextureData			RightEye;

	/** Texture tracking data for the left eye, may not be present.*/
	FPerEyeTextureData			LeftEye;

	AActor* UnderlayActor;
	UProceduralMeshComponent* UnderlayMeshComponentPtr;

	FYvrXRLayer(const IStereoLayers::FLayerDesc& InLayerDesc);

	void SetLayerId(uint32 InId) { Desc.SetLayerId(InId); }
	uint32 GetLayerId() const { return Desc.GetLayerId(); }

	bool NeedReallocateRightTexture();
	bool NeedReallocateLeftTexture();

	FIntRect GetRightViewportSize() const;
	FVector2D GetRightQuadSize() const;

	FIntRect GetLeftViewportSize() const;
	FVector2D GetLeftQuadSize() const;

	void ManageUnderlayComponent();
	void CreateUnderlayMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector2D>& UV0);
	bool NeedsCreateUnderlayMesh() const { return (Desc.Flags & IStereoLayers::LAYER_FLAG_SUPPORT_DEPTH) != 0; }

	TArray<FXrCompositionLayerUnion> CreateOpenXRLayer(FTransform InvTrackingToWorld, float WorldToMeters, XrSpace Space) const;
	
private:
	void CreateOpenXRProjectionLayer(TArray<FXrCompositionLayerUnion>& Headers) const;
	void CreateOpenXRQuadLayer(bool bIsStereo, bool bNoAlpha, FTransform PositionTransform, float WorldToMeters, XrSpace Space, TArray<FXrCompositionLayerUnion>& Headers) const;
	void CreateOpenXRCylinderLayer(bool bIsStereo, bool bNoAlpha, FTransform PositionTransform, float WorldToMeters, XrSpace Space, TArray<FXrCompositionLayerUnion>& Headers) const;
	void CreateOpenXREquirectLayer(bool bIsStereo, bool bNoAlpha, FTransform PositionTransform, float WorldToMeters, XrSpace Space, TArray<FXrCompositionLayerUnion>& Headers) const;
	void CreateOpenXRCubeLayer(bool bIsStereo, bool bNoAlpha, FTransform PositionTransform, float WorldToMeters, XrSpace Space, TArray<FXrCompositionLayerUnion>& Headers) const;
};

bool GetLayerDescMember(const FYvrXRLayer& Layer, IStereoLayers::FLayerDesc& OutLayerDesc);
void SetLayerDescMember(FYvrXRLayer& OutLayer, const IStereoLayers::FLayerDesc& InLayerDesc);
void MarkLayerTextureForUpdate(FYvrXRLayer& Layer);
