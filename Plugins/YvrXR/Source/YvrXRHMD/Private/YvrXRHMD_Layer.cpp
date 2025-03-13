// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRHMD_Layer.h"
#include "YvrXRHMD.h"
#include "YvrXRCore.h"
#include "YvrXRPlatformRHI.h"
#include "YvrXRHMD_Swapchain.h"

#include "Materials/MaterialInstanceDynamic.h"

FYvrXRLayer::FYvrXRLayer(const IStereoLayers::FLayerDesc& InLayerDesc)
	: Desc(InLayerDesc)
	, UnderlayMeshComponentPtr(nullptr)
{
	ManageUnderlayComponent();
}

bool FYvrXRLayer::NeedReallocateRightTexture()
{
	if (!Desc.Texture.IsValid())
	{
		return false;
	}

	FRHITexture2D* Texture2D = Desc.Texture->GetTexture2D();
	FRHITextureCube* TextureCube = Desc.Texture->GetTextureCube();
	if (Texture2D)
	{
		if (!RightEye.Swapchain.IsValid())
		{
			return true;
		}

		return RightEye.SwapchainSize != Texture2D->GetSizeXY();
	}
	else if(TextureCube)
	{
		if (!RightEye.Swapchain.IsValid())
		{
			return true;
		}
	}

	return false;
}

bool FYvrXRLayer::NeedReallocateLeftTexture()
{
	if (!Desc.LeftTexture.IsValid())
	{
		return false;
	}

	FRHITexture2D* Texture2D = Desc.LeftTexture->GetTexture2D();
	FRHITextureCube* TextureCube = Desc.LeftTexture->GetTextureCube();
	if (Texture2D)
	{
		if (!LeftEye.Swapchain.IsValid())
		{
			return true;
		}

		return LeftEye.SwapchainSize != Texture2D->GetSizeXY();
	}
	else if(TextureCube)
	{
		if (!LeftEye.Swapchain.IsValid())
		{
			return true;
		}
	}
		
	return false;
}

FIntRect FYvrXRLayer::GetRightViewportSize() const
{
	FBox2D Viewport(RightEye.SwapchainSize * Desc.UVRect.Min, RightEye.SwapchainSize * Desc.UVRect.Max);
	return FIntRect(Viewport.Min.IntPoint(), Viewport.Max.IntPoint());
}

FIntRect FYvrXRLayer::GetLeftViewportSize() const
{
	FBox2D Viewport(LeftEye.SwapchainSize * Desc.UVRect.Min, LeftEye.SwapchainSize * Desc.UVRect.Max);
	return FIntRect(Viewport.Min.IntPoint(), Viewport.Max.IntPoint());
}

FVector2D FYvrXRLayer::GetRightQuadSize() const
{
	if (Desc.Flags & IStereoLayers::LAYER_FLAG_QUAD_PRESERVE_TEX_RATIO)
	{
		float AspectRatio = RightEye.SwapchainSize.Y / RightEye.SwapchainSize.X;
		return FVector2D(Desc.QuadSize.X, Desc.QuadSize.X * AspectRatio);
	}
	return Desc.QuadSize;
}

FVector2D FYvrXRLayer::GetLeftQuadSize() const
{
	if (Desc.Flags & IStereoLayers::LAYER_FLAG_QUAD_PRESERVE_TEX_RATIO)
	{
		float AspectRatio = LeftEye.SwapchainSize.Y / LeftEye.SwapchainSize.X;
		return FVector2D(Desc.QuadSize.X, Desc.QuadSize.X * AspectRatio);
	}
	return Desc.QuadSize;
}

void FYvrXRLayer::ManageUnderlayComponent()
{
	if (NeedsCreateUnderlayMesh())
	{
		const FString BaseComponentName = FString::Printf(TEXT("YvrXRUnderlay_%d"), Desc.Id);
		const FName ComponentName(*BaseComponentName);

		if (!UnderlayMeshComponentPtr) {
			UWorld* World = nullptr;
			for (const FWorldContext& Context : GEngine->GetWorldContexts())
			{
				if (Context.WorldType == EWorldType::Game || Context.WorldType == EWorldType::PIE)
				{
					World = Context.World();
				}
			}

			if (!World)
			{
				return;
			}

			UnderlayActor = World->SpawnActor<AActor>();

			UnderlayMeshComponentPtr = NewObject<UProceduralMeshComponent>(UnderlayActor, ComponentName);
			UnderlayMeshComponentPtr->RegisterComponent();

			TArray<FVector> Vertices;
			TArray<int32> Triangles;
			TArray<FVector> Normals;
			TArray<FVector2D> UV0;
			TArray<FLinearColor> VertexColors;
			TArray<FProcMeshTangent> Tangents;

			CreateUnderlayMesh(Vertices, Triangles, UV0);
			UnderlayMeshComponentPtr->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false);

			FYvrXRHMD* YvrXRHMD = static_cast<FYvrXRHMD*>(GEngine->XRSystem->GetHMDDevice());
			UMaterial* UnderlayMeshMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("Material'/YvrXR/Materials/UnderlayMeshMaterial.UnderlayMeshMaterial'")));
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(UnderlayMeshMaterial, NULL);
			UnderlayMeshComponentPtr->SetMaterial(0, DynamicMaterial);
		}
		UnderlayMeshComponentPtr->SetWorldTransform(Desc.Transform);
	}
}

void FYvrXRLayer::CreateUnderlayMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector2D>& UV0)
{
	if (Desc.HasShape<FQuadLayer>())
	{
		const float QuadScale = 0.99;

		FIntPoint TexSize = Desc.Texture.IsValid() ? Desc.Texture->GetTexture2D()->GetSizeXY() : Desc.LayerSize;
		float AspectRatio = TexSize.X ? (float)TexSize.Y / (float)TexSize.X : 3.0f / 4.0f;

		float QuadSizeX = Desc.QuadSize.X;
		float QuadSizeY = (Desc.Flags & IStereoLayers::LAYER_FLAG_QUAD_PRESERVE_TEX_RATIO) ? Desc.QuadSize.X * AspectRatio : Desc.QuadSize.Y;

		Vertices.Init(FVector::ZeroVector, 4);
		Vertices[0] = FVector(0.0, -QuadSizeX / 2, -QuadSizeY / 2) * QuadScale;
		Vertices[1] = FVector(0.0, QuadSizeX / 2, -QuadSizeY / 2) * QuadScale;
		Vertices[2] = FVector(0.0, QuadSizeX / 2, QuadSizeY / 2) * QuadScale;
		Vertices[3] = FVector(0.0, -QuadSizeX / 2, QuadSizeY / 2) * QuadScale;

		UV0.Init(FVector2D::ZeroVector, 4);
		UV0[0] = FVector2D(1, 0);
		UV0[1] = FVector2D(1, 1);
		UV0[2] = FVector2D(0, 0);
		UV0[3] = FVector2D(0, 1);

		Triangles.Reserve(6);
		Triangles.Add(0);
		Triangles.Add(1);
		Triangles.Add(2);
		Triangles.Add(0);
		Triangles.Add(2);
		Triangles.Add(3);
	}
	else if (Desc.HasShape<FCylinderLayer>())
	{
		float Arc, Radius, Height;
		const FCylinderLayer& CylinderProps = Desc.GetShape<FCylinderLayer>();
		Arc = CylinderProps.OverlayArc;
		Radius = CylinderProps.Radius;
		Height = CylinderProps.Height;

		const float CylinderScale = 0.99;

		FIntPoint TexSize = Desc.Texture.IsValid() ? Desc.Texture->GetTexture2D()->GetSizeXY() : Desc.LayerSize;
		float AspectRatio = TexSize.X ? (float)TexSize.Y / (float)TexSize.X : 3.0f / 4.0f;

		float CylinderHeight = (Desc.Flags & IStereoLayers::LAYER_FLAG_QUAD_PRESERVE_TEX_RATIO) ? Arc * AspectRatio : Height;

		const FVector XAxis = FVector(1, 0, 0);
		const FVector YAxis = FVector(0, 1, 0);
		const FVector HalfHeight = FVector(0, 0, CylinderHeight / 2);

		const float ArcAngle = Arc / Radius;
		const int Sides = (int)((ArcAngle * 180) / (PI * 5));
		Vertices.Init(FVector::ZeroVector, 2 * (Sides + 1));
		UV0.Init(FVector2D::ZeroVector, 2 * (Sides + 1));
		Triangles.Init(0, Sides * 6);

		float CurrentAngle = -ArcAngle / 2;
		const float AngleStep = ArcAngle / Sides;


		for (int Side = 0; Side < Sides + 1; Side++)
		{
			FVector MidVertex = Radius * (FMath::Cos(CurrentAngle) * XAxis + FMath::Sin(CurrentAngle) * YAxis);
			Vertices[2 * Side] = (MidVertex - HalfHeight) * CylinderScale;
			Vertices[(2 * Side) + 1] = (MidVertex + HalfHeight) * CylinderScale;

			UV0[2 * Side] = FVector2D(1 - (Side / (float)Sides), 0);
			UV0[(2 * Side) + 1] = FVector2D(1 - (Side / (float)Sides), 1);

			CurrentAngle += AngleStep;

			if (Side < Sides)
			{
				Triangles[6 * Side + 0] = 2 * Side;
				Triangles[6 * Side + 2] = 2 * Side + 1;
				Triangles[6 * Side + 1] = 2 * (Side + 1) + 1;
				Triangles[6 * Side + 3] = 2 * Side;
				Triangles[6 * Side + 5] = 2 * (Side + 1) + 1;
				Triangles[6 * Side + 4] = 2 * (Side + 1);
			}
		}
	}
}

TArray<FXrCompositionLayerUnion> FYvrXRLayer::CreateOpenXRLayer(FTransform InvTrackingToWorld, float WorldToMeters, XrSpace Space) const
{
	TArray<FXrCompositionLayerUnion> Headers;

	const bool bNoAlpha = Desc.Flags & IStereoLayers::LAYER_FLAG_TEX_NO_ALPHA_CHANNEL;
	const bool bIsStereo = Desc.LeftTexture.IsValid();
	FTransform PositionTransform = Desc.PositionType == IStereoLayers::ELayerType::WorldLocked ?
		InvTrackingToWorld : FTransform::Identity;

	if (GetLayerId() == FYvrXRHMD::EyebufferLayerId)
	{
		CreateOpenXRProjectionLayer(Headers);
	}
	else if (Desc.HasShape<FQuadLayer>())
	{
		CreateOpenXRQuadLayer(bIsStereo, bNoAlpha, PositionTransform, WorldToMeters, Space, Headers);
	}
	else if (Desc.HasShape<FCylinderLayer>())
	{
		CreateOpenXRCylinderLayer(bIsStereo, bNoAlpha, PositionTransform, WorldToMeters, Space, Headers);
	}
	else if (Desc.HasShape<FEquirectLayer>())
	{
		CreateOpenXREquirectLayer(bIsStereo, bNoAlpha, PositionTransform, WorldToMeters, Space, Headers);
	}
	else if (Desc.HasShape<FCubemapLayer>())
	{
		CreateOpenXRCubeLayer(bIsStereo, bNoAlpha, PositionTransform, WorldToMeters, Space, Headers);
	}

	return Headers;
}

void FYvrXRLayer::CreateOpenXRProjectionLayer(TArray<FXrCompositionLayerUnion>& Headers) const
{
	XrCompositionLayerProjection Projection = { XR_TYPE_COMPOSITION_LAYER_PROJECTION, nullptr };
	FXrCompositionLayerUnion LayerUnion;
	LayerUnion.Projection = Projection;
	Headers.Add(LayerUnion);
}

void FYvrXRLayer::CreateOpenXRCylinderLayer(bool bIsStereo, bool bNoAlpha, FTransform PositionTransform, float WorldToMeters, XrSpace Space, TArray<FXrCompositionLayerUnion>& Headers) const
{
	XrCompositionLayerCylinderKHR Cylinder = { XR_TYPE_COMPOSITION_LAYER_CYLINDER_KHR, /*next*/ nullptr };
	Cylinder.layerFlags = bNoAlpha ? 0 : XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT |
		XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
	Cylinder.space = Space;
	Cylinder.subImage.imageArrayIndex = 0;
	Cylinder.pose = ToXrPose(Desc.Transform * PositionTransform, WorldToMeters);

	const FCylinderLayer& CylinderProps = Desc.GetShape<FCylinderLayer>();
	Cylinder.radius = FMath::Abs(CylinderProps.Radius / WorldToMeters);
	Cylinder.centralAngle = FMath::Min((float)(2.0f * PI), FMath::Abs(CylinderProps.OverlayArc / CylinderProps.Radius));
	Cylinder.aspectRatio = FMath::Abs(CylinderProps.OverlayArc / CylinderProps.Height);

	FXrCompositionLayerUnion LayerUnion;
	LayerUnion.Cylinder = Cylinder;

	if (RightEye.Swapchain.IsValid())
	{
		LayerUnion.Cylinder.eyeVisibility = bIsStereo ? XR_EYE_VISIBILITY_RIGHT : XR_EYE_VISIBILITY_BOTH;
		LayerUnion.Cylinder.subImage.imageRect = ToXrRect(GetRightViewportSize());
		LayerUnion.Cylinder.subImage.swapchain = static_cast<FYvrXRSwapchain*>(RightEye.Swapchain.Get())->GetHandle();
		Headers.Add(LayerUnion);
	}
	if (LeftEye.Swapchain.IsValid())
	{
		LayerUnion.Cylinder.eyeVisibility = XR_EYE_VISIBILITY_LEFT;
		LayerUnion.Cylinder.subImage.imageRect = ToXrRect(GetLeftViewportSize());
		LayerUnion.Cylinder.subImage.swapchain = static_cast<FYvrXRSwapchain*>(LeftEye.Swapchain.Get())->GetHandle();
		Headers.Add(LayerUnion);
	}
}

void FYvrXRLayer::CreateOpenXRQuadLayer(bool bIsStereo, bool bNoAlpha, FTransform PositionTransform, float WorldToMeters, XrSpace Space, TArray<FXrCompositionLayerUnion>& Headers) const
{
	XrCompositionLayerQuad Quad = { XR_TYPE_COMPOSITION_LAYER_QUAD, nullptr };
	Quad.layerFlags = bNoAlpha ? 0 : XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT |
		XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
	Quad.space = Space;
	Quad.subImage.imageArrayIndex = 0;
	Quad.pose = ToXrPose(Desc.Transform * PositionTransform, WorldToMeters);

	// The layer pose doesn't take the transform scale into consideration, so we need to manually apply it the quad size.
	const FVector2D LayerComponentScaler(Desc.Transform.GetScale3D().Y, Desc.Transform.GetScale3D().Z);

	FXrCompositionLayerUnion LayerUnion;
	LayerUnion.Quad = Quad;

	// We need to copy each layer into an OpenXR swapchain so they can be displayed by the compositor
	if (RightEye.Swapchain.IsValid())
	{
		LayerUnion.Quad.eyeVisibility = bIsStereo ? XR_EYE_VISIBILITY_RIGHT : XR_EYE_VISIBILITY_BOTH;
		LayerUnion.Quad.subImage.imageRect = ToXrRect(GetRightViewportSize());
		LayerUnion.Quad.subImage.swapchain = static_cast<FYvrXRSwapchain*>(RightEye.Swapchain.Get())->GetHandle();
		LayerUnion.Quad.size = ToXrExtent2D(GetRightQuadSize() * LayerComponentScaler, WorldToMeters);
		Headers.Add(LayerUnion);
	}
	if (LeftEye.Swapchain.IsValid())
	{
		LayerUnion.Quad.eyeVisibility = XR_EYE_VISIBILITY_LEFT;
		LayerUnion.Quad.subImage.imageRect = ToXrRect(GetLeftViewportSize());
		LayerUnion.Quad.subImage.swapchain = static_cast<FYvrXRSwapchain*>(LeftEye.Swapchain.Get())->GetHandle();
		LayerUnion.Quad.size = ToXrExtent2D(GetLeftQuadSize() * LayerComponentScaler, WorldToMeters);
		Headers.Add(LayerUnion);
	}
}

void FYvrXRLayer::CreateOpenXREquirectLayer(bool bIsStereo, bool bNoAlpha, FTransform PositionTransform, float WorldToMeters, XrSpace Space, TArray<FXrCompositionLayerUnion>& Headers) const
{
	XrCompositionLayerEquirectKHR Equirect = { XR_TYPE_COMPOSITION_LAYER_EQUIRECT_KHR, nullptr };
	Equirect.layerFlags = bNoAlpha ? 0 : XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT |
		XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
	Equirect.space = Space;
	Equirect.subImage.imageArrayIndex = 0;
	Equirect.pose = ToXrPose(Desc.Transform * PositionTransform, WorldToMeters);

	const FEquirectLayer& EquirectProps = Desc.GetShape<FEquirectLayer>();

	//TODO
	Equirect.radius = 10.0f; //FMath::Abs(EquirectProps.Radius / WorldToMeters);

	FXrCompositionLayerUnion LayerUnion;
	LayerUnion.Equirect = Equirect;

	// We need to copy each layer into an OpenXR swapchain so they can be displayed by the compositor
	if (RightEye.Swapchain.IsValid())
	{
		LayerUnion.Equirect.eyeVisibility = bIsStereo ? XR_EYE_VISIBILITY_RIGHT : XR_EYE_VISIBILITY_BOTH;
		LayerUnion.Equirect.subImage.imageRect = ToXrRect(GetRightViewportSize());
		LayerUnion.Equirect.subImage.swapchain = static_cast<FYvrXRSwapchain*>(RightEye.Swapchain.Get())->GetHandle();
		LayerUnion.Equirect.scale = ToXrVector2f(EquirectProps.RightScale);
		LayerUnion.Equirect.bias = ToXrVector2f(EquirectProps.RightBias);
		Headers.Add(LayerUnion);
	}
	if (LeftEye.Swapchain.IsValid())
	{
		LayerUnion.Equirect.eyeVisibility = XR_EYE_VISIBILITY_LEFT;
		LayerUnion.Equirect.subImage.imageRect = ToXrRect(GetLeftViewportSize());
		LayerUnion.Equirect.subImage.swapchain = static_cast<FYvrXRSwapchain*>(LeftEye.Swapchain.Get())->GetHandle();
		LayerUnion.Equirect.scale = ToXrVector2f(EquirectProps.LeftScale);
		LayerUnion.Equirect.bias = ToXrVector2f(EquirectProps.LeftBias);
		Headers.Add(LayerUnion);
	}
}

void FYvrXRLayer::CreateOpenXRCubeLayer(bool bIsStereo, bool bNoAlpha, FTransform PositionTransform, float WorldToMeters, XrSpace Space, TArray<FXrCompositionLayerUnion>& Headers) const
{
	 XrCompositionLayerCubeKHR Cube = { XR_TYPE_COMPOSITION_LAYER_CUBE_KHR, nullptr };
	 Cube.layerFlags = bNoAlpha ? 0 : XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT |
	 	XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
	 Cube.space = Space;
	 Cube.imageArrayIndex = 0;

	 // TODO
	 Cube.orientation = XrQuaternionf{ 0.0f, 0.0f, 0.0f, 1.0f };

	 const FCubemapLayer& CubeProps = Desc.GetShape<FCubemapLayer>();

	 FXrCompositionLayerUnion LayerUnion;
	 LayerUnion.Cube = Cube;

	 // We need to copy each layer into an OpenXR swapchain so they can be displayed by the compositor
	 if (RightEye.Swapchain.IsValid())
	 {
	 	LayerUnion.Cube.eyeVisibility = bIsStereo ? XR_EYE_VISIBILITY_RIGHT : XR_EYE_VISIBILITY_BOTH;
	 	LayerUnion.Cube.swapchain = static_cast<FYvrXRSwapchain*>(RightEye.Swapchain.Get())->GetHandle();
	 	Headers.Add(LayerUnion);
	 }
	 if (LeftEye.Swapchain.IsValid())
	 {
	 	LayerUnion.Cube.eyeVisibility = XR_EYE_VISIBILITY_LEFT;
	 	LayerUnion.Cube.swapchain = static_cast<FYvrXRSwapchain*>(LeftEye.Swapchain.Get())->GetHandle();
	 	Headers.Add(LayerUnion);
	 }
}

// TStereoLayerManager helper functions

bool GetLayerDescMember(const FYvrXRLayer& Layer, IStereoLayers::FLayerDesc& OutLayerDesc)
{
	OutLayerDesc = Layer.Desc;
	return true;
}

void SetLayerDescMember(FYvrXRLayer& Layer, const IStereoLayers::FLayerDesc& Desc)
{
	Layer.Desc = Desc;
}

void MarkLayerTextureForUpdate(FYvrXRLayer& Layer)
{
	// If the swapchain is static we need to re-allocate it before it can be updated
	if (!(Layer.Desc.Flags & IStereoLayers::LAYER_FLAG_TEX_CONTINUOUS_UPDATE))
	{
		Layer.RightEye.Swapchain.Reset();
		Layer.LeftEye.Swapchain.Reset();
	}
	Layer.RightEye.bUpdateTexture = true;
	Layer.LeftEye.bUpdateTexture = true;
}
