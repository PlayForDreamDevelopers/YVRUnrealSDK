// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HeadMountedDisplayBase.h"
#include "XRRenderTargetManager.h"
#include "StereoLayerManager.h"
#include "SceneViewExtension.h"
#include "HardwareInfo.h"
#include "XRRenderBridge.h"
#include "XRSwapChain.h"

#include "HeadMountedDisplay.h"
#include "IStereoLayers.h"

#if PLATFORM_WINDOWS
#include "D3D11RHIPrivate.h"
#endif

#include "YvrPreviewLayer.h"
#include "IYvrPreviewHMDPlugin.h"

class YVRLIVEPREVIEWHMD_API FYvrPreviewHMD
	: public FHeadMountedDisplayBase
	, public FXRRenderTargetManager
	, public FSceneViewExtensionBase
	, public TStereoLayerManager<FYvrPreviewLayer>
{
public:
	FYvrPreviewHMD(const FAutoRegister&, IYvrPreviewHMDPlugin*);
	~FYvrPreviewHMD();

	/** @return	True if the API was initialized OK */
	bool IsInitialized() const;

	bool InitializePreviewWindows() const;

public:
	/** IXRTrackingSystem interface */
	virtual FName GetSystemName() const override
	{
		static FName DefaultName(TEXT("YvrPreviewHMD"));
		return DefaultName;
	}


#if ENGINE_MINOR_VERSION >25
	int32 GetXRSystemFlags() const
	{
		return EXRSystemFlags::IsHeadMounted;
	}
#endif

	virtual class IHeadMountedDisplay* GetHMDDevice() override
	{
		return this;
	}

	virtual class TSharedPtr< class IStereoRendering, ESPMode::ThreadSafe > GetStereoRenderingDevice() override
	{
		return SharedThis(this);
	}

	/** IXRTrackingSystem */
	virtual bool OnStartGameFrame(FWorldContext& WorldContext) override;
	virtual bool DoesSupportPositionalTracking() const override;
	virtual bool HasValidTrackingPosition() override;
	virtual bool EnumerateTrackedDevices(TArray<int32>& TrackedIds, EXRTrackedDeviceType DeviceType = EXRTrackedDeviceType::Any) override;

	virtual bool GetTrackingSensorProperties(int32 InDeviceId, FQuat& OutOrientation, FVector& OutOrigin, FXRSensorProperties& OutSensorProperties) override;
#if ENGINE_MINOR_VERSION >25
	virtual FString GetTrackedDevicePropertySerialNumber(int32 DeviceId) override;
#endif
	virtual bool GetCurrentPose(int32 DeviceId, FQuat& CurrentOrientation, FVector& CurrentPosition) override;
	virtual bool GetRelativeEyePose(int32 DeviceId, EStereoscopicPass Eye, FQuat& OutOrientation, FVector& OutPosition) override;
	virtual bool IsTracking(int32 DeviceId) override;

	virtual void ResetOrientationAndPosition(float yaw = 0.f) override;
	virtual void ResetOrientation(float Yaw = 0.f) override;
	virtual void ResetPosition() override;

	virtual void SetBaseRotation(const FRotator& BaseRot) override;
	virtual FRotator GetBaseRotation() const override;
	virtual void SetBaseOrientation(const FQuat& BaseOrient) override;
	virtual FQuat GetBaseOrientation() const override;
	virtual void SetBasePosition(const FVector& BasePosition) override;
	virtual FVector GetBasePosition() const override;
	virtual void OnBeginPlay(FWorldContext& InWorldContext) override;
	virtual void OnEndPlay(FWorldContext& InWorldContext) override;

	virtual void SetTrackingOrigin(EHMDTrackingOrigin::Type NewOrigin) override;
	virtual EHMDTrackingOrigin::Type GetTrackingOrigin() const override;
	virtual bool GetFloorToEyeTrackingTransform(FTransform& OutFloorToEye) const override;


public:
	/** IHeadMountedDisplay interface */
	virtual bool IsHMDConnected() override;
	virtual bool IsHMDEnabled() const override;
	virtual EHMDWornState::Type GetHMDWornState() override;
	virtual void EnableHMD(bool allow = true) override;
	virtual bool GetHMDMonitorInfo(MonitorInfo&) override;

	virtual void GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const override;

	virtual void SetInterpupillaryDistance(float NewInterpupillaryDistance) override;
	virtual float GetInterpupillaryDistance() const override;

	virtual bool IsChromaAbCorrectionEnabled() const override;

	virtual void UpdateScreenSettings(const FViewport* InViewport) override {}

	virtual bool GetHMDDistortionEnabled(EShadingPath ShadingPath) const override;

	virtual void OnBeginRendering_GameThread() override;
	virtual void OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily) override;

	virtual float GetPixelDenity() const override { return 1; }
	virtual void SetPixelDensity(const float NewDensity) override { PixelDensity = NewDensity; }
	virtual FIntPoint GetIdealRenderTargetSize() const override { return IdealRenderTargetSize; }

	/** IStereoRendering interface */
	virtual bool IsStereoEnabled() const override;
	virtual bool EnableStereo(bool stereo = true) override;
	virtual void AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const override;
	virtual void CalculateStereoViewOffset(const EStereoscopicPass StereoPassType, FRotator& ViewRotation, const float MetersToWorld, FVector& ViewLocation) override;
	virtual FMatrix GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType) const override;
	virtual void RenderTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* BackBuffer, FRHITexture2D* SrcTexture, FVector2D WindowSize) const override;
	virtual void GetEyeRenderParams_RenderThread(const FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const override;
	virtual IStereoRenderTargetManager* GetRenderTargetManager() override { return this; }

	/** FXRRenderTargetManager interface */
	virtual FXRRenderBridge* GetActiveRenderBridge_GameThread(bool bUseSeparateRenderTarget) override;
	virtual bool ShouldUseSeparateRenderTarget() const override
	{
		check(IsInGameThread());
		return IsStereoEnabled();
	}
	virtual void CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY) override;
	virtual bool NeedReAllocateViewportRenderTarget(const class FViewport& Viewport) override;
	//virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags InTexFlags, ETextureCreateFlags InTargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;
#if ENGINE_MINOR_VERSION >25
	virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags InTexFlags, ETextureCreateFlags InTargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;
#else
	virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;
#endif
	// todo layer
	virtual bool ShouldCopyDebugLayersToSpectatorScreen() const override { return true; }

	//ISceneViewExtension interface
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override {}
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {};
	virtual void PostRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;
#if ENGINE_MINOR_VERSION >26
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;
#endif

	// todo SpectatorScreen
private:
	void CreateSpectatorScreenController();
	void TransferImage_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* SrcTexture, FIntRect SrcRect, FRHITexture2D* DstTexture, FIntRect DstRect, bool bNoAlpha) const;
public:
	virtual FIntRect GetFullFlatEyeRect_RenderThread(FTexture2DRHIRef EyeTexture) const override;
	virtual void CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* SrcTexture, FIntRect SrcRect, FRHITexture2D* DstTexture, FIntRect DstRect, bool bClearBlack, bool bNoAlpha) const override;

	// support functions
	bool Startup();
	void onConnected();


	// shared texture
	TArray<TRefCountPtr<ID3D11Texture2D>> HMDDstSwapChainTextures;
	TArray<void*> HMDDstTextureHandles;
	TArray<FTextureRHIRef> HMDRHITextureSwapChain;
	FTextureRHIRef HMDBindingTexture;
	bool CreateRHITexture(ID3D11Texture2D* OpenedSharedResource, EPixelFormat Format, FTexture2DRHIRef& OutTexture);
	bool CreateSharedTexture2D();




	class BridgeBaseImpl : public FXRRenderBridge
	{
	public:
		BridgeBaseImpl(FYvrPreviewHMD* plugin)
			: Plugin(plugin)
			, bInitialized(false)
			, bUseExplicitTimingMode(false)
		{}

		// Render bridge virtual interface
		virtual bool Present(int& SyncInterval) override;
		virtual void PostPresent() override;
		virtual bool NeedsNativePresent() override;

		// Non-virtual public interface
		bool IsInitialized() const { return bInitialized; }

		bool IsUsingExplicitTimingMode() const
		{
			return bUseExplicitTimingMode;
		}

		FXRSwapChainPtr GetPCSwapChain() { return PCSwapChain; }
		FXRSwapChainPtr GetHMDSwapChain() { return HMDSwapChain; }

		/** Schedules BeginRendering_RHI on the RHI thread when in explicit timing mode */
		void BeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList);

		/** Called only when we're in explicit timing mode, which needs to be paired with a call to PostPresentHandoff */
		void BeginRendering_RHI();

		void CreatePCSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures);
		void CreateHMDSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures);

		// Virtual interface implemented by subclasses
		virtual void Reset() {};

	private:
		virtual void FinishRendering() {};

	protected:

		FYvrPreviewHMD* Plugin;
		FXRSwapChainPtr	PCSwapChain;
		FXRSwapChainPtr	HMDSwapChain;

		bool bInitialized;

		/** If we use explicit timing mode, we must have matching calls to BeginRendering_RHI and PostPresentHandoff */
		bool bUseExplicitTimingMode;

	};

	/** IXRTrackingSystem */
	virtual float GetWorldToMetersScale() const override;

	/** parameters */
	FIntPoint IdealRenderTargetSize;
	float PixelDensity;

	IYvrPreviewHMDPlugin* YvrPreviewHMDPlugin;
	IRendererModule* RendererModule;
	TRefCountPtr<BridgeBaseImpl> pBridge;

	bool bHmdEnabled;
	EHMDWornState::Type HmdWornState;
	bool InitializedSucceeded;
	bool bIsVRPreview;

	bool bStereoDesired;
	bool bStereoEnabled;

	// HMD base values, specify forward orientation and zero pos offset
	FQuat BaseOrientation;
	FVector	BaseOffset;

};
