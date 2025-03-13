// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "YvrXRHMD_Layer.h"
#include "CoreMinimal.h"
#include "HeadMountedDisplayBase.h"
#include "XRTrackingSystemBase.h"
#include "XRRenderTargetManager.h"
#include "XRRenderBridge.h"
#include "XRSwapChain.h"
#include "SceneViewExtension.h"
#include "StereoLayerManager.h"
#include "IHeadMountedDisplayVulkanExtensions.h"

#include "openxr.h"
#include "openxr_yvr.h"

class APlayerController;
class FSceneView;
class FSceneViewFamily;
class UCanvas;
class FYvrXRRenderBridge;
class UYvrXRHMDSettings;

enum FRecenterTypes
{
	RecenterOrientation = 0x1,
	RecenterPosition = 0x2,
	RecenterOrientationAndPosition = 0x3
};

DECLARE_MULTICAST_DELEGATE_OneParam(FYvrPollEventDelegate, XrEventDataBuffer);

/**
 * Simple Head Mounted Display
 */
class FYvrXRHMD
	: public FHeadMountedDisplayBase
	, public FXRRenderTargetManager
	, public FSceneViewExtensionBase
	, public TStereoLayerManager<FYvrXRLayer>
{
public:
	class FDeviceSpace
	{
	public:
		FDeviceSpace(XrAction InAction, XrPath InPath);
		~FDeviceSpace();

		bool CreateSpace(XrSession InSession);
		void DestroySpace();

		XrAction Action;
		XrSpace Space;
		XrPath Path;
	};

	class FTrackingSpace
	{
	public:
		FTrackingSpace(XrReferenceSpaceType InType);
		FTrackingSpace(XrReferenceSpaceType InType, XrPosef InBasePose);
		~FTrackingSpace();

		bool CreateSpace(XrSession InSession);
		void DestroySpace();

		XrReferenceSpaceType Type;
		XrSpace Handle;
		XrPosef BasePose;
	};

	// The game and render threads each have a separate copy of these structures so that they don't stomp on each other or cause tearing
	// when the game thread progresses to the next frame while the render thread is still working on the previous frame.
	struct FPipelinedFrameState
	{
		XrFrameState FrameState{XR_TYPE_FRAME_STATE};
		XrViewState ViewState{XR_TYPE_VIEW_STATE};
		TArray<XrView> Views;
		TArray<XrSpaceLocation> DeviceLocations;
		TSharedPtr<FTrackingSpace> TrackingSpace;
		float WorldToMetersScale = 100.0f;

		TArray<XrViewConfigurationView> ViewConfigs;
		TArray<class IOpenXRExtensionPlugin*> PluginViews;

		FTransform TrackingToWorld;
		FTransform LastTrackingToWorld;

		bool bIsSpaceWarpFrame;
	};

	struct FPipelinedLayerState
	{
		TArray<FXrCompositionLayerUnion> NativeOverlays;
		TArray<XrCompositionLayerProjectionView> ProjectionLayers;
		TArray<XrCompositionLayerDepthInfoKHR> DepthLayers;
		TArray<XrCompositionLayerSpaceWarpInfoFB> MotionVectorLayers;

		TArray<XrSwapchainSubImage> ColorImages;
		TArray<XrSwapchainSubImage> DepthImages;
		TArray<XrSwapchainSubImage> MotionVectorImages;
		TArray<XrSwapchainSubImage> MotionVectorDepthImages;

		FXRSwapChainPtr ColorSwapchain;
		FXRSwapChainPtr DepthSwapchain;
		FXRSwapChainPtr FoveationSwapchain;
		FXRSwapChainPtr MotionVectorSwapchain;
		FXRSwapChainPtr MotionVectorDepthSwapchain;

		TArray<FXRSwapChainPtr> NativeOverlaySwapchains;

		TArray<XrCompositionLayerSettingsFB> LayerSettings;
	};

	class FVulkanExtensions : public IHeadMountedDisplayVulkanExtensions
	{
	public:
		FVulkanExtensions(XrInstance InInstance, XrSystemId InSystem) : Instance(InInstance), System(InSystem) {}
		virtual ~FVulkanExtensions() {}

		/** IHeadMountedDisplayVulkanExtensions */
		virtual bool GetVulkanInstanceExtensionsRequired(TArray<const ANSICHAR*>& Out) override;
		virtual bool GetVulkanDeviceExtensionsRequired(VkPhysicalDevice_T *pPhysicalDevice, TArray<const ANSICHAR*>& Out) override;

	private:
		XrInstance Instance;
		XrSystemId System;

		TArray<char> Extensions;
		TArray<char> DeviceExtensions;
	};

	/** IXRTrackingSystem interface */
	virtual FName GetSystemName() const override
	{
		// This identifier is relied upon for plugin identification,
		// see GetHMDName() to query the true XR system name.
		static FName DefaultName(TEXT("YvrXRHMD"));
		return DefaultName;
	}

	int32 GetXRSystemFlags() const override
	{
		int32 flags = EXRSystemFlags::IsHeadMounted;

		if (SelectedEnvironmentBlendMode != XR_ENVIRONMENT_BLEND_MODE_OPAQUE)
		{
			flags |= EXRSystemFlags::IsAR;
		}

		if (bSupportsHandTracking)
		{
			flags |= EXRSystemFlags::SupportsHandTracking;
		}

		return flags;
	}

	virtual bool EnumerateTrackedDevices(TArray<int32>& OutDevices, EXRTrackedDeviceType Type = EXRTrackedDeviceType::Any) override;

	virtual void SetInterpupillaryDistance(float NewInterpupillaryDistance) override;
	virtual float GetInterpupillaryDistance() const override;

	virtual void ResetOrientationAndPosition(float Yaw = 0.f) override;
	virtual void ResetOrientation(float Yaw = 0.f) override;
	virtual void ResetPosition() override;
	virtual void Recenter(EOrientPositionSelector::Type Selector, float Yaw = 0.f);

	virtual bool GetIsTracked(int32 DeviceId);
	virtual bool GetCurrentPose(int32 DeviceId, FQuat& CurrentOrientation, FVector& CurrentPosition) override;
	virtual void SetBaseRotation(const FRotator& BaseRot) override;
	virtual FRotator GetBaseRotation() const override;

	virtual void SetBaseOrientation(const FQuat& BaseOrient) override;
	virtual FQuat GetBaseOrientation() const override;
	virtual void SetBasePosition(const FVector& BasePosition);
	virtual FVector GetBasePosition() const;

	virtual void SetTrackingOrigin(EHMDTrackingOrigin::Type NewOrigin) override;
	virtual EHMDTrackingOrigin::Type GetTrackingOrigin() const override;

	virtual class IHeadMountedDisplay* GetHMDDevice() override
	{
		return this;
	}
	virtual class TSharedPtr< class IStereoRendering, ESPMode::ThreadSafe > GetStereoRenderingDevice() override
	{
		return SharedThis(this);
	}
#if !PLATFORM_HOLOLENS
	// Native stereo layers severely impact performance on Hololens
	virtual class IStereoLayers* GetStereoLayers() override
	{
		return this;
	}
#endif

	virtual void GetMotionControllerData(UObject* WorldContext, const EControllerHand Hand, FXRMotionControllerData& MotionControllerData) override;

	virtual float GetWorldToMetersScale() const override;

	//virtual FVector2D GetPlayAreaBounds(EHMDTrackingOrigin::Type Origin) const override;

protected:

	bool StartSession();
	bool StopSession();
	bool OnStereoStartup();
	bool OnStereoTeardown();
	bool ReadNextEvent(XrEventDataBuffer* buffer);
	void DestroySession();

	const FPipelinedFrameState& GetPipelinedFrameStateForThread() const;
	FPipelinedFrameState& GetPipelinedFrameStateForThread();

	void UpdateDeviceLocations(bool bUpdateOpenXRExtensionPlugins);
	void UpdateBasePose();
	void EnumerateViews(FPipelinedFrameState& PipelineState);
	void LocateViews(FPipelinedFrameState& PipelinedState, bool ResizeViewsArray = false);

	void CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture* SrcTexture, FIntRect SrcRect, FRHITexture* DstTexture, FIntRect DstRect, bool bClearBlack, bool bNoAlpha, ERenderTargetActions RTAction) const;
	void CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture* SrcTexture, FIntRect SrcRect, const FXRSwapChainPtr& DstSwapChain, FIntRect DstRect, bool bClearBlack, bool bNoAlpha) const;

	void SetFoveation(XrFoveationLevelFB Level, float VerticalOffset, XrFoveationDynamicFB Dynamic);
	void UpdateStereoRenderingParams();

	void SetupFrameLayers_RenderThread(FRHICommandListImmediate& RHICmdList);

public:
	/** IXRTrackingSystem interface */
	//virtual bool DoesSupportLateProjectionUpdate() const override { return true; }
	virtual FString GetVersionString() const override;
	virtual bool IsTracking(int32 DeviceId) override;
	virtual bool HasValidTrackingPosition() override { return IsTracking(HMDDeviceId); }

	/** IHeadMountedDisplay interface */
	virtual bool IsHMDConnected() override { return true; }
	virtual bool DoesSupportPositionalTracking() const override { return true; }
	virtual bool IsHMDEnabled() const override;
	virtual void EnableHMD(bool allow = true) override;
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
	virtual FName GetHMDName() const override;
#endif
	virtual bool GetHMDMonitorInfo(MonitorInfo&) override;
	virtual void GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const override;
	virtual bool IsChromaAbCorrectionEnabled() const override;
	virtual FIntPoint GetIdealRenderTargetSize() const override;
	virtual bool GetHMDDistortionEnabled(EShadingPath ShadingPath) const override { return false; }
	virtual float GetPixelDenity() const override;
	virtual void SetPixelDensity(const float NewPixelDensity) override;
	virtual FIntRect GetFullFlatEyeRect_RenderThread(FTexture2DRHIRef EyeTexture) const override;
	virtual void CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* SrcTexture, FIntRect SrcRect, FRHITexture2D* DstTexture, FIntRect DstRect, bool bClearBlack, bool bNoAlpha) const override;
	virtual void OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily) override;
	virtual void OnBeginRendering_GameThread() override;
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
	virtual void OnLateUpdateApplied_RenderThread(FRHICommandListImmediate& RHICmdList, const FTransform& NewRelativeTransform) override;
#else
	virtual void OnLateUpdateApplied_RenderThread(const FTransform& NewRelativeTransform) override;
#endif
	virtual bool OnStartGameFrame(FWorldContext& WorldContext) override;
	virtual bool OnEndGameFrame(FWorldContext& WorldContext) override;
	virtual EHMDWornState::Type GetHMDWornState() override { return bIsReady ? EHMDWornState::Worn : EHMDWornState::NotWorn; }

	/** IStereoRendering interface */
	virtual bool IsStereoEnabled() const override;
	virtual bool EnableStereo(bool stereo = true) override;
	virtual int32 GetDesiredNumberOfViews(bool bStereoRequested) const override;

	virtual IStereoRenderTargetManager* GetRenderTargetManager() override;

	/** ISceneViewExtension interface */
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override;
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const;

	/** IStereoRenderTargetManager */
	virtual void CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY) override;
	virtual bool ShouldUseSeparateRenderTarget() const override { return IsStereoEnabled(); }
	virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;
	virtual bool NeedReAllocateDepthTexture(const TRefCountPtr<IPooledRenderTarget>& DepthTarget) override final { return bNeedReAllocatedDepth; }
	virtual bool AllocateDepthTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags InTexFlags, ETextureCreateFlags TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override final;
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 26
	virtual bool NeedReAllocateShadingRateTexture(const TRefCountPtr<IPooledRenderTarget>& FoveationTarget) override;
	virtual bool AllocateShadingRateTexture(uint32 Index, uint32 RenderSizeX, uint32 RenderSizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags InTexFlags, ETextureCreateFlags InTargetableTextureFlags, FTexture2DRHIRef& OutTexture, FIntPoint& OutTextureSize) override;
#else
	virtual bool NeedReAllocateFoveationTexture(const TRefCountPtr<IPooledRenderTarget>& FoveationTarget) override;
	virtual bool AllocateFoveationTexture(uint32 Index, uint32 RenderSizeX, uint32 RenderSizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags InTexFlags, ETextureCreateFlags InTargetableTextureFlags, FTexture2DRHIRef& OutTexture, FIntPoint& OutTextureSize) override;
#endif

#ifdef YVR_APPSW
	virtual bool NeedReAllocateMotionVectorTexture(const TRefCountPtr<IPooledRenderTarget>& MotionVectorTarget, const TRefCountPtr<IPooledRenderTarget>& MotionVectorDepthTarget) override;
	virtual bool AllocateMotionVectorTexture(uint32 Index, uint8 Format, uint32 NumMips, uint32 InTexFlags, uint32 InTargetableTextureFlags, FTexture2DRHIRef& OutTexture, FIntPoint& OutTextureSize, FTexture2DRHIRef& OutDepthTexture, FIntPoint& OutDepthTextureSize) override;
#endif
	virtual FXRRenderBridge* GetActiveRenderBridge_GameThread(bool bUseSeparateRenderTarget) override;

	/** IStereoRenderTargetManager */
	virtual void OnBeginPlay(FWorldContext& InWorldContext) override;
	virtual void OnEndPlay(FWorldContext& InWorldContext) override;

	/** IStereoLayers */
	virtual bool ShouldCopyDebugLayersToSpectatorScreen() const override { return true; }

	struct FLayerPtr_CompareTotal
	{
		FORCEINLINE bool operator()(const FYvrXRLayer& A, const FYvrXRLayer& B) const
		{
			// Draw PoleAHole layers (Android only), EyeFov layer, followed by other layers
			int32 PassA = (A.GetLayerId() == FYvrXRHMD::EyebufferLayerId) ? 0 : A.NeedsCreateUnderlayMesh() ? -1 : 1;
			int32 PassB = (B.GetLayerId() == FYvrXRHMD::EyebufferLayerId) ? 0 : B.NeedsCreateUnderlayMesh() ? -1 : 1;

			if (PassA != PassB)
				return PassA < PassB;

			// Draw non-FaceLocked layers first
			IStereoLayers::FLayerDesc DescA;
			GetLayerDescMember(A, DescA);
			IStereoLayers::FLayerDesc DescB;
			GetLayerDescMember(B, DescB);

			bool bFaceLockedA = (DescA.PositionType == IStereoLayers::ELayerType::FaceLocked);
			bool bFaceLockedB = (DescB.PositionType == IStereoLayers::ELayerType::FaceLocked);

			if (bFaceLockedA != bFaceLockedB)
				return !bFaceLockedA;

			// Draw layers by ascending priority
			if (DescA.Priority != DescB.Priority)
				return DescA.Priority < DescB.Priority;

			// Draw layers by ascending id
			return A.GetLayerId() < B.GetLayerId();
		}
	};

	void CopySortedAllLayers(TArray<FYvrXRLayer>& OutArray, bool bMarkClean = true)
	{
		CopyLayers(OutArray, bMarkClean);
		OutArray.Sort(FLayerPtr_CompareTotal());
	}

#if ENGINE_MAJOR_VERSION > 4
	virtual bool GetRelativeEyePose(int32 InDeviceId, int32 ViewIndex, FQuat& OutOrientation, FVector& OutPosition) override;
	virtual bool GetPoseForTime(int32 DeviceId, FTimespan Timespan, bool& OutTimeWasUsed, FQuat& Orientation, FVector& Position, bool& bProvidedLinearVelocity, FVector& LinearVelocity, bool& bProvidedAngularVelocity, FVector& AngularVelocityRadPerSec, bool& bProvidedLinearAcceleration, FVector& LinearAcceleration, float WorldToMetersScale);
	virtual void AdjustViewRect(int32 ViewIndex, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const override;
	virtual void SetFinalViewRect(FRHICommandListImmediate& RHICmdList, const int32 ViewIndex, const FIntRect& FinalViewRect) override;
	virtual FMatrix GetStereoProjectionMatrix(const int32 ViewIndex) const override;
	virtual void GetEyeRenderParams_RenderThread(const struct FHeadMountedDisplayPassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const override;
#else
#if ENGINE_MINOR_VERSION > 26
	virtual void SetFinalViewRect(FRHICommandListImmediate & RHICmdList, const enum EStereoscopicPass StereoPass, const FIntRect & FinalViewRect) override;
#else
	virtual void SetFinalViewRect(const enum EStereoscopicPass StereoPass, const FIntRect& FinalViewRect) override;
#endif
	virtual uint32 GetViewIndexForPass(EStereoscopicPass StereoPassType) const override;
	virtual bool GetRelativeEyePose(int32 InDeviceId, EStereoscopicPass StereoPass, FQuat& OutOrientation, FVector& OutPosition) override;
	virtual bool GetPoseForTime(int32 DeviceId, FTimespan Timespan, FQuat& Orientation, FVector& Position, bool& bProvidedLinearVelocity, FVector& LinearVelocity, bool& bProvidedAngularVelocity, FVector& AngularVelocityRadPerSec);
	virtual void AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const override;
	virtual FMatrix GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPass) const override;
	virtual void GetEyeRenderParams_RenderThread(const struct FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const override;
#endif

public:
	/** Constructor */
	FYvrXRHMD(const FAutoRegister&, XrInstance InInstance, XrSystemId InSystem, TRefCountPtr<FYvrXRRenderBridge>& InRenderBridge, TArray<const char*> InEnabledExtensions);

	/** Destructor */
	virtual ~FYvrXRHMD();

	void OnBeginRendering_RHIThread(const FPipelinedFrameState& InFrameState, FXRSwapChainPtr ColorSwapchain, FXRSwapChainPtr DepthSwapchain, FXRSwapChainPtr MotionVectorSwapchain, FXRSwapChainPtr MotionVectorDepthSwapchain);
	void OnFinishRendering_RHIThread();

	FIntPoint GetRenderTargetSize() const;
	FIntPoint GetRenderViewportSize(const FIntPoint& RenderTargetSize) const;

	/** YVR API */
	void SetCpuLevel(const uint8 CpuLevel);
	void SetGpuLevel(const uint8 GpuLevel);
	void SetDisplayRefreshRate(const float RefreshRate);
	float GetDisplayRefreshRate();
	void SetFFRLevel(const uint8 Level);
	uint8 GetFFRLevel() const;
	void SetColorScaleAndOffset(FLinearColor ColorScale, FLinearColor ColorOffset, bool bApplyToAllLayers);
	void SetSpaceWarpEnabled(bool bIsEnabled);
	void SetSeeThroughBackgroundEnabled(bool bIsEnabled);
	void SetSharpenType(const uint8 SharpenType, bool bApplyToAllLayers);

	/** @return	True if the HMD was initialized OK */
	YVRXRHMD_API bool IsInitialized() const;
	YVRXRHMD_API bool IsRunning() const;
	YVRXRHMD_API bool IsFocused() const;

	YVRXRHMD_API int32 AddActionDevice(XrAction Action, XrPath Path);
	YVRXRHMD_API void ResetActionDevices();
	YVRXRHMD_API XrPath GetTrackedDevicePath(const int32 DeviceId);

	YVRXRHMD_API bool IsExtensionEnabled(const FString& Name) const { return EnabledExtensions.Contains(Name); }
	YVRXRHMD_API XrInstance GetInstance() { return Instance; }
	YVRXRHMD_API XrSystemId GetSystem() { return System; }
	YVRXRHMD_API XrSession GetSession() { return Session; }
	YVRXRHMD_API XrSpace GetTrackingSpace();
	YVRXRHMD_API XrTime GetDisplayTime() const;
	YVRXRHMD_API TArray<IOpenXRExtensionPlugin*>& GetExtensionPlugins() { return ExtensionPlugins; }

	YVRXRHMD_API bool IsStandaloneStereoOnlyDevice() const { return bIsStandaloneStereoOnlyDevice; }
	YVRXRHMD_API bool IsHandTrackingEnabled() const;

	YVRXRHMD_API FYvrPollEventDelegate& OnPollEventDelegate()
	{
		return PollEventDelegate;
	}

	static uint32			EyebufferLayerId;

private:

	void UpdateLayerSwapchainTexture(const FYvrXRLayer& Layer, FRHICommandListImmediate& RHICmdList);
	void ConfigureLayerSwapchain(FYvrXRLayer& Layer);

	bool					bStereoEnabled;
	TAtomic<bool>			bIsRunning;
	TAtomic<bool>			bIsReady;
	TAtomic<bool>			bIsRendering;
	TAtomic<bool>			bIsSynchronized;
	bool					bDepthExtensionSupported;
	bool					bViewConfigurationFovSupported;
	bool					bNeedReAllocatedDepth;
	bool					bNeedReAllocatedFoveation;
	bool					bIsMobileMultiViewEnabled;
	bool					bIsMobileHDREnabled;
	bool					bSupportsHandTracking;
	bool					bIsStandaloneStereoOnlyDevice;
	float					WorldToMetersScale = 100.0f;

	XrSessionState			CurrentSessionState;
	FRWLock					SessionHandleMutex;

	TArray<const char*>		EnabledExtensions;
	TArray<class IOpenXRExtensionPlugin*> ExtensionPlugins;
	XrInstance				Instance;
	XrSystemId				System;
	XrSession				Session;
	XrSpace					LocalSpace;
	XrSpace					FloorSpace;
	XrSpace					StageSpace;
	XrReferenceSpaceType	TrackingSpaceType;
	XrViewConfigurationType SelectedViewConfigurationType;
	XrEnvironmentBlendMode  SelectedEnvironmentBlendMode;
	XrInstanceProperties    InstanceProperties;
	XrSystemProperties      SystemProperties;

	FPipelinedFrameState	PipelinedFrameStateGame;
	FPipelinedFrameState	PipelinedFrameStateRendering;
	FPipelinedFrameState	PipelinedFrameStateRHI;

	FPipelinedLayerState	PipelinedLayerStateRendering;
	FPipelinedLayerState	PipelinedLayerStateRHI;

	FRWLock					DeviceMutex;
	TArray<FDeviceSpace>	DeviceSpaces;

	TRefCountPtr<FYvrXRRenderBridge> RenderBridge;
	IRendererModule*		RendererModule;

	uint8					LastRequestedSwapchainFormat;
	uint8					LastRequestedDepthSwapchainFormat;

	UYvrXRHMDSettings*		Settings;

	bool					bEnableSpaceWarp;
	bool					bNeedReAllocatedMotionVector;
	FTransform				LastTrackingToWorld;
	uint32					RecommendedMotionVectorImageRectWidth;
	uint32					RecommendedMotionVectorImageRectHeight;

	FQuat					BaseOrientation;
	FVector					BasePosition;

	FYvrPollEventDelegate   PollEventDelegate;
};
