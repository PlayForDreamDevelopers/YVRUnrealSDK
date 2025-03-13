// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrPreviewHMD.h"

#include "YvrPreviewManager.h"
#include "YVR_Lib_Types.h"
#include "SceneViewExtension.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/GameEngine.h"
#include "Slate/SceneViewport.h"
#include "ClearQuad.h"
#include "ScenePrivate.h"
#include <vector>
#include "CoreMinimal.h"
#include "XRThreadUtils.h"
#include "RendererPrivate.h"
#include "PostProcess/PostProcessHMD.h"
#include "PipelineStateCache.h"
#include "DefaultSpectatorScreenController.h"
#include "ScreenRendering.h"

#include "Logging/LogMacros.h"

#define FovAngle 1.091307
DEFINE_LOG_CATEGORY(LogYvrPreview);


/** Helper function for acquiring the appropriate FSceneViewport */
FSceneViewport* FindSceneViewport()
{
	if (!GIsEditor)
	{
		UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
		return GameEngine->SceneViewport.Get();
	}
#if WITH_EDITOR
	else
	{
		UEditorEngine* EditorEngine = CastChecked<UEditorEngine>(GEngine);
		FSceneViewport* PIEViewport = (FSceneViewport*)EditorEngine->GetPIEViewport();
		if (PIEViewport != nullptr && PIEViewport->IsStereoRenderingAllowed())
		{
			// PIE is setup for stereo rendering
			return PIEViewport;
		}
		else
		{
			// Check to see if the active editor viewport is drawing in stereo mode
			// @todo vreditor: Should work with even non-active viewport!
			FSceneViewport* EditorViewport = (FSceneViewport*)EditorEngine->GetActiveViewport();
			if (EditorViewport != nullptr && EditorViewport->IsStereoRenderingAllowed())
			{
				return EditorViewport;
			}
		}
	}
#endif
	return nullptr;
}

FYvrPreviewHMD::FYvrPreviewHMD(const FAutoRegister& AutoRegister, IYvrPreviewHMDPlugin* InYvrPreviewHMDPlugin) :
	FSceneViewExtensionBase(AutoRegister),
	FHeadMountedDisplayBase(nullptr),
	IdealRenderTargetSize(FIntPoint(3200, 1600)),
	PixelDensity(1.0f),
	YvrPreviewHMDPlugin(InYvrPreviewHMDPlugin),
	RendererModule(nullptr),
	bHmdEnabled(true),
	HmdWornState(EHMDWornState::Unknown),
	InitializedSucceeded(false),
	bIsVRPreview(false),
	bStereoDesired(false),
	bStereoEnabled(false),
	BaseOrientation(FQuat::Identity),
	BaseOffset(FVector::ZeroVector)
{
	Startup();
}

FYvrPreviewHMD::~FYvrPreviewHMD()
{
}

bool FYvrPreviewHMD::IsInitialized() const
{
	return true;
}

// set the window size as the viewport resolution in editor preference
bool FYvrPreviewHMD::InitializePreviewWindows() const
{
#if WITH_EDITOR
	//user current ScreenResolution as the NewWindowSize
	ULevelEditorPlaySettings* PlaySettingsConfig = GetMutableDefault<ULevelEditorPlaySettings>();
	if (GEngine->IsValidLowLevel())
	{
		const UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
		FIntPoint ScreenResolution = FIntPoint(1080, 1080);
		if (UserSettings->IsValidLowLevel())
		{
			ScreenResolution = UserSettings->GetScreenResolution();
		}
		PlaySettingsConfig->NewWindowHeight = ScreenResolution.Y;
		PlaySettingsConfig->NewWindowWidth = ScreenResolution.X;
	}
	return true;
#endif
	return false;
}


/** IHeadMountedDisplay Implementation */
bool FYvrPreviewHMD::IsHMDConnected()
{
	return YvrPreviewHMDPlugin->IsHMDConnected();
}

bool FYvrPreviewHMD::IsHMDEnabled() const
{
	return bHmdEnabled;
}

EHMDWornState::Type FYvrPreviewHMD::GetHMDWornState()
{
	return HmdWornState;
}

void FYvrPreviewHMD::EnableHMD(bool enable)
{
	bHmdEnabled = enable;

	if (!bHmdEnabled)
	{
		EnableStereo(false);
	}
}

bool FYvrPreviewHMD::GetHMDMonitorInfo(MonitorInfo& MonitorDesc)
{
	MonitorDesc.MonitorName = "";
	MonitorDesc.MonitorId = 0;
	MonitorDesc.DesktopX = MonitorDesc.DesktopY = MonitorDesc.ResolutionX = MonitorDesc.ResolutionY = 0;
	return false;
}

void FYvrPreviewHMD::GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const
{
	OutHFOVInDegrees = 0.0f;
	OutVFOVInDegrees = 0.0f;
}


bool FYvrPreviewHMD::DoesSupportPositionalTracking() const
{
	return true;
}

bool FYvrPreviewHMD::HasValidTrackingPosition()
{
	return true;
}

bool FYvrPreviewHMD::GetTrackingSensorProperties(int32 SensorId, FQuat& OutOrientation, FVector& OutOrigin, FXRSensorProperties& OutSensorProperties)
{
	OutOrigin = FVector::ZeroVector;
	OutOrientation = FQuat::Identity;
	OutSensorProperties = FXRSensorProperties();
	return true;
}

#if ENGINE_MINOR_VERSION >25
FString FYvrPreviewHMD::GetTrackedDevicePropertySerialNumber(int32 DeviceId)
{
	return FString();
}
#endif


void FYvrPreviewHMD::SetInterpupillaryDistance(float NewInterpupillaryDistance)
{
}

float FYvrPreviewHMD::GetInterpupillaryDistance() const
{
	return 0.064f;
}

bool FYvrPreviewHMD::GetCurrentPose(int32 DeviceId, FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	CurrentOrientation = FQuat::Identity;
	CurrentPosition = FVector::ZeroVector;

	if (YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_CONNECTED)
	{
		yvrPoseState yvrHMDHeadPose;
		YvrPreviewManager::GetInstance()->GetHeadTrackingData(&yvrHMDHeadPose);

		CurrentOrientation = FQuat((float)-yvrHMDHeadPose.pose.orientation.z, (float)yvrHMDHeadPose.pose.orientation.x, (float)yvrHMDHeadPose.pose.orientation.y, (float)-yvrHMDHeadPose.pose.orientation.w);
		CurrentPosition = FVector((float)-yvrHMDHeadPose.pose.position.z * GetWorldToMetersScale(), (float)yvrHMDHeadPose.pose.position.x * GetWorldToMetersScale(), (float)yvrHMDHeadPose.pose.position.y * GetWorldToMetersScale());
		return true;
	}
	return false;
}

void FYvrPreviewHMD::SetTrackingOrigin(EHMDTrackingOrigin::Type NewOrigin)
{
}

EHMDTrackingOrigin::Type FYvrPreviewHMD::GetTrackingOrigin() const
{
	return EHMDTrackingOrigin::Eye;
}

bool FYvrPreviewHMD::GetFloorToEyeTrackingTransform(FTransform& OutStandingToSeatedTransform) const
{
	bool bSuccess = false;
	return bSuccess;
}

float FYvrPreviewHMD::GetWorldToMetersScale() const
{
	return 100.0f;
}

bool FYvrPreviewHMD::EnumerateTrackedDevices(TArray<int32>& TrackedIds, EXRTrackedDeviceType DeviceType)
{
	TrackedIds.Empty();
	if (DeviceType == EXRTrackedDeviceType::Any || DeviceType == EXRTrackedDeviceType::HeadMountedDisplay)
	{
		TrackedIds.Add(IXRTrackingSystem::HMDDeviceId);
		return true;
	}
	return false;
}

bool FYvrPreviewHMD::IsTracking(int32 DeviceId)
{
	return true;
}

bool FYvrPreviewHMD::IsChromaAbCorrectionEnabled() const
{
	return false;
}


void FYvrPreviewHMD::OnBeginPlay(FWorldContext& InWorldContext)
{
#if WITH_EDITOR


	if (!InitializedSucceeded)
	{
		return;
	}

	if (GIsEditor)
	{
		UEditorEngine* EdEngine = Cast<UEditorEngine>(GEngine);
		if (EdEngine->GetPlayInEditorSessionInfo().IsSet())
		{
			bIsVRPreview = EdEngine->GetPlayInEditorSessionInfo()->OriginalRequestParams.SessionPreviewTypeOverride ==
				EPlaySessionPreviewType::VRPreview;
		}
	}

	if (bIsVRPreview)
	{
		UHeadMountedDisplayFunctionLibrary::SetSpectatorScreenMode(ESpectatorScreenMode::SingleEye);

		GEngine->bUseFixedFrameRate = true;
		GEngine->FixedFrameRate = 72;
	}

#endif
}


void FYvrPreviewHMD::OnEndPlay(FWorldContext& InWorldContext)
{
	if (!GEnableVREditorHacks)
	{
		EnableStereo(false);
	}

	if (bIsVRPreview)
	{
		GEngine->bUseFixedFrameRate = false;
		GEngine->FixedFrameRate = 30;
	}
}

bool FYvrPreviewHMD::OnStartGameFrame(FWorldContext& WorldContext)
{
	if (bStereoEnabled != bStereoDesired)
	{
		bStereoEnabled = EnableStereo(bStereoDesired);
	}
	return true;
}

void FYvrPreviewHMD::ResetOrientationAndPosition(float yaw)
{
	ResetOrientation(yaw);
	ResetPosition();
}

void FYvrPreviewHMD::ResetOrientation(float Yaw)
{
	BaseOrientation = FQuat::Identity;
}

void FYvrPreviewHMD::ResetPosition()
{
	BaseOffset = FVector();
}

void FYvrPreviewHMD::SetBaseRotation(const FRotator& BaseRot)
{
	BaseOrientation = BaseRot.Quaternion();
}

FRotator FYvrPreviewHMD::GetBaseRotation() const
{
	return  FRotator::ZeroRotator;
}

void FYvrPreviewHMD::SetBaseOrientation(const FQuat& BaseOrient)
{
	BaseOrientation = BaseOrient;
}

FQuat FYvrPreviewHMD::GetBaseOrientation() const
{
	return BaseOrientation;
}

void FYvrPreviewHMD::SetBasePosition(const FVector& BasePosition)
{
	BaseOffset = BasePosition;
}

FVector FYvrPreviewHMD::GetBasePosition() const
{
	return BaseOffset;
}

bool FYvrPreviewHMD::IsStereoEnabled() const
{
	return bStereoEnabled && bHmdEnabled;
}

bool FYvrPreviewHMD::EnableStereo(bool bStereo)
{
	if (bStereoEnabled == bStereo)
	{
		return false;
	}

	if (!bStereo)
	{
		return false;
	}

	bStereoDesired = (IsHMDEnabled()) ? bStereo : false;
	FSceneViewport* SceneVP = FindSceneViewport();
	if (SceneVP)
	{
		TSharedPtr<SWindow> Window = SceneVP->FindWindow();
		if (Window.IsValid() && SceneVP->GetViewportWidget().IsValid())
		{
			if (bStereo)
			{
				bStereoEnabled = bStereoDesired;
			}
		}
	}

	// Uncap fps to enable FPS higher than 62
	GEngine->bForceDisableFrameRateSmoothing = bStereoEnabled;

	return bStereoEnabled;
}


void FYvrPreviewHMD::AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
{
	SizeX = FMath::CeilToInt(IdealRenderTargetSize.X * PixelDensity);
	SizeY = FMath::CeilToInt(IdealRenderTargetSize.Y * PixelDensity);

	SizeX = SizeX / 2;
	if (StereoPass == eSSP_RIGHT_EYE)
	{
		X += SizeX;
	}
}

bool FYvrPreviewHMD::GetRelativeEyePose(int32 DeviceId, EStereoscopicPass Eye, FQuat& OutOrientation, FVector& OutPosition)
{
	OutOrientation = FQuat::Identity;
	OutPosition = FVector::ZeroVector;
	if (DeviceId == IXRTrackingSystem::HMDDeviceId && (Eye == eSSP_LEFT_EYE || Eye == eSSP_RIGHT_EYE))
	{
		OutPosition = FVector(0, (Eye == EStereoscopicPass::eSSP_LEFT_EYE ? -.5 : .5) * 0.064f * GetWorldToMetersScale(), 0);
		return true;
	}
	else
	{
		return false;
	}
}

void FYvrPreviewHMD::CalculateStereoViewOffset(const enum EStereoscopicPass StereoPassType, FRotator& ViewRotation, const float WorldToMeters, FVector& ViewLocation)
{
	// Forward to the base implementation (that in turn will call the DefaultXRCamera implementation)
	FHeadMountedDisplayBase::CalculateStereoViewOffset(StereoPassType, ViewRotation, WorldToMeters, ViewLocation);
}

FMatrix FYvrPreviewHMD::GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType) const
{
	check(IsStereoEnabled() || IsHeadTrackingEnforced());

	XrFovf Fov = {};
	Fov = XrFovf{ -FovAngle, FovAngle, FovAngle,  -FovAngle };

	float ZNear = GNearClippingPlane;
	float SumRL = (Fov.angleRight + Fov.angleLeft);
	float SumTB = (Fov.angleUp + Fov.angleDown);
	float InvRL = (1.0f / (Fov.angleRight - Fov.angleLeft));
	float InvTB = (1.0f / (Fov.angleUp - Fov.angleDown));

	FMatrix Mat = FMatrix(
		FPlane((2.0f * InvRL), 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, (2.0f * InvTB), 0.0f, 0.0f),
		FPlane((SumRL * -InvRL), (SumTB * -InvTB), 0.0f, 1.0f),
		FPlane(0.0f, 0.0f, ZNear, 0.0f)
	);

	return Mat;
}

void FYvrPreviewHMD::GetEyeRenderParams_RenderThread(const FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const
{
	if (Context.View.StereoPass == eSSP_LEFT_EYE)
	{
		EyeToSrcUVOffsetValue.X = 0.0f;
		EyeToSrcUVOffsetValue.Y = 0.0f;

		EyeToSrcUVScaleValue.X = 0.5f;
		EyeToSrcUVScaleValue.Y = 1.0f;
	}
	else
	{
		EyeToSrcUVOffsetValue.X = 0.5f;
		EyeToSrcUVOffsetValue.Y = 0.0f;

		EyeToSrcUVScaleValue.X = 0.5f;
		EyeToSrcUVScaleValue.Y = 1.0f;
	}
}

bool FYvrPreviewHMD::GetHMDDistortionEnabled(EShadingPath /* ShadingPath */) const
{
	return false;
}

void FYvrPreviewHMD::OnBeginRendering_GameThread()
{
	check(IsInGameThread());
	SpectatorScreenController->BeginRenderViewFamily();
}


void FYvrPreviewHMD::OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	check(IsInRenderingThread());

	check(pBridge);
	pBridge->BeginRendering_RenderThread(RHICmdList);

	check(SpectatorScreenController);
	SpectatorScreenController->UpdateSpectatorScreenMode_RenderThread();
}

FXRRenderBridge* FYvrPreviewHMD::GetActiveRenderBridge_GameThread(bool /* bUseSeparateRenderTarget */)
{
	check(IsInGameThread());

	return pBridge;
}

void FYvrPreviewHMD::CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY)
{
	check(IsInGameThread());

	if (!IsStereoEnabled())
	{
		return;
	}

	InOutSizeX = FMath::CeilToInt(IdealRenderTargetSize.X * PixelDensity);
	InOutSizeY = FMath::CeilToInt(IdealRenderTargetSize.Y * PixelDensity);

	check(InOutSizeX != 0 && InOutSizeY != 0);
}

bool FYvrPreviewHMD::NeedReAllocateViewportRenderTarget(const FViewport& Viewport)
{
	check(IsInGameThread());

	if (IsStereoEnabled())
	{
		const uint32 InSizeX = Viewport.GetSizeXY().X;
		const uint32 InSizeY = Viewport.GetSizeXY().Y;
		const FIntPoint RenderTargetSize = Viewport.GetRenderTargetTextureSizeXY();
		uint32 NewSizeX = InSizeX, NewSizeY = InSizeY;
		CalculateRenderTargetSize(Viewport, NewSizeX, NewSizeY);
		if (NewSizeX != RenderTargetSize.X || NewSizeY != RenderTargetSize.Y)
		{
			return true;
		}
	}
	return false;
}

static const uint32 DPSwapChainLength = 1;
#if ENGINE_MINOR_VERSION > 25
bool FYvrPreviewHMD::AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples)
#else
bool FYvrPreviewHMD::AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples /*= 1*/)
#endif
{
	check(IsInRenderingThread());
	if (!IsStereoEnabled())
	{
		return false;
	}

	TArray<FTextureRHIRef> SwapChainTextures;
	FTextureRHIRef BindingTexture;

	if (pBridge != nullptr && pBridge->GetPCSwapChain() != nullptr && pBridge->GetPCSwapChain()->GetTexture2D() != nullptr && pBridge->GetPCSwapChain()->GetTexture2D()->GetSizeX() == SizeX && pBridge->GetPCSwapChain()->GetTexture2D()->GetSizeY() == SizeY)
	{
		OutTargetableTexture = (FTexture2DRHIRef&)pBridge->GetPCSwapChain()->GetTextureRef();
		OutShaderResourceTexture = OutTargetableTexture;
		return true;
	}

	for (uint32 SwapChainIter = 0; SwapChainIter < DPSwapChainLength; ++SwapChainIter)
	{
		FRHIResourceCreateInfo CreateInfo;
		FTexture2DRHIRef TargetableTexture, ShaderResourceTexture;

		RHICreateTargetableShaderResource2D(SizeX, SizeY, PF_R8G8B8A8, 1, TexCreate_None, TexCreate_RenderTargetable | TexCreate_Shared, false, CreateInfo, TargetableTexture, ShaderResourceTexture, NumSamples);

		check(TargetableTexture == ShaderResourceTexture);

		SwapChainTextures.Add((FTextureRHIRef&)TargetableTexture);

		if (BindingTexture == nullptr)
		{
			BindingTexture = GDynamicRHI->RHICreateAliasedTexture((FTextureRHIRef&)TargetableTexture);
		}
	}
	if (YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_CONNECTED)
	{
		pBridge->CreateHMDSwapChain(HMDBindingTexture, MoveTemp(HMDRHITextureSwapChain));
	}


	pBridge->CreatePCSwapChain(BindingTexture, MoveTemp(SwapChainTextures));

	// These are the same.
	OutTargetableTexture = (FTexture2DRHIRef&)BindingTexture;
	OutShaderResourceTexture = (FTexture2DRHIRef&)BindingTexture;

	return true;
}

bool FYvrPreviewHMD::Startup()
{
	// grab a pointer to the renderer module for displaying our mirror window
	static const FName RendererModuleName("Renderer");
	RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);

	// create shared texture after connected
	if (YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_CONNECTED)
	{
		onConnected();
	}
	else {
		YvrPreviewManager::GetInstance()->connectEvent.AddRaw(this, &FYvrPreviewHMD::onConnected);
	}


	static const auto PixelDensityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("vr.PixelDensity"));
	if (PixelDensityCVar)
	{
		PixelDensity = 1;
	}
	// enforce finishcurrentframe
	static IConsoleVariable* CFCFVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.finishcurrentframe"));
	CFCFVar->Set(false);

	pBridge = new BridgeBaseImpl(this);
	ensure(pBridge != nullptr);

	CreateSpectatorScreenController();

	InitializedSucceeded = true;
	UE_LOG(LogYvrPreview, Log, TEXT("YvrLivePreview Initialize Success"));

	return true;
}

void FYvrPreviewHMD::onConnected() 
{
	YvrPreviewManager::GetInstance()->GetResolution(IdealRenderTargetSize[0], IdealRenderTargetSize[1]);
	if (!CreateSharedTexture2D())
	{
		UE_LOG(LogYvrPreview, Log, TEXT("CreateSharedTexture2D failure"));
	}
	UE_LOG(LogYvrPreview, Log, TEXT("RenderTargetSize: %i * %i"), IdealRenderTargetSize[0], IdealRenderTargetSize[1]);
}

// Copy Texture
void FYvrPreviewHMD::TransferImage_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* SrcTexture, FIntRect SrcRect, FRHITexture2D* DstTexture, FIntRect DstRect, bool bNoAlpha) const
{
	check(IsInRenderingThread());
	bool sRGBSource = false;
	FIntPoint SrcSize;
	FIntPoint DstSize;
	if (SrcTexture->GetTexture2D() && DstTexture->GetTexture2D())
	{
		SrcSize = FIntPoint(SrcTexture->GetTexture2D()->GetSizeX(), SrcTexture->GetTexture2D()->GetSizeY());
		DstSize = FIntPoint(DstTexture->GetTexture2D()->GetSizeX(), DstTexture->GetTexture2D()->GetSizeY());
	}
	else
	{
		return;
	}

	if (SrcRect.IsEmpty())
	{
		SrcRect = FIntRect(FIntPoint::ZeroValue, SrcSize);
	}

	if (DstRect.IsEmpty())
	{
		DstRect = FIntRect(FIntPoint::ZeroValue, DstSize);
	}

	const uint32 viewportWidth = DstRect.Width();
	const uint32 viewportHeight = DstRect.Height();
	float U = SrcRect.Min.X / (float)SrcSize.X;
	float V = SrcRect.Min.Y / (float)SrcSize.Y;
	float USize = SrcRect.Width() / (float)SrcSize.X;
	float VSize = SrcRect.Height() / (float)SrcSize.Y;

	FRHITexture* SrcTextureRHI = SrcTexture;
#if ENGINE_MINOR_VERSION > 25
	RHICmdList.Transition(FRHITransitionInfo(SrcTextureRHI, ERHIAccess::Unknown, ERHIAccess::SRVGraphics));
#else
	RHICmdList.TransitionResources(EResourceTransitionAccess::EReadable, &SrcTextureRHI, 1);
#endif
	FGraphicsPipelineStateInitializer GraphicsPSOInit;

	if (bNoAlpha)
	{
		GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGB, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero>::GetRHI();
	}
	else
	{
		GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_SourceAlpha, BF_Zero, BO_Add, BF_One, BF_Zero>::GetRHI();
	}

	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	const auto FeatureLevel = GMaxRHIFeatureLevel;
	auto ShaderMap = GetGlobalShaderMap(FeatureLevel);
	TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
#if ENGINE_MINOR_VERSION > 24
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
#else
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
#endif

	if (DstTexture->GetTexture2D())
	{
		sRGBSource &= ((SrcTexture->GetFlags() & TexCreate_SRGB) != 0);
		uint32 NumMips = SrcTexture->GetNumMips();

		for (uint32 MipIndex = 0; MipIndex < NumMips; MipIndex++)
		{
			FRHIRenderPassInfo RPInfo(DstTexture, ERenderTargetActions::Load_Store);
			RPInfo.ColorRenderTargets[0].MipIndex = MipIndex;

			RHICmdList.BeginRenderPass(RPInfo, TEXT("CopyTexture"));
			{
				const uint32 ViewportWidth = viewportWidth >> MipIndex;
				const uint32 ViewportHeight = viewportHeight >> MipIndex;
				const FIntPoint TargetSize(viewportWidth, viewportHeight);

				if (bNoAlpha)
				{
					RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0.0f, DstRect.Max.X, DstRect.Max.Y, 1.0f);
					DrawClearQuad(RHICmdList, FLinearColor::Black);
				}

				RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
				FRHISamplerState* SamplerState = DstRect.Size() == SrcRect.Size() ? TStaticSamplerState<SF_Point>::GetRHI() : TStaticSamplerState<SF_Bilinear>::GetRHI();
#if ENGINE_MINOR_VERSION > 24
				if (!sRGBSource)
				{
					TShaderMapRef<FScreenPSMipLevel> PixelShader(ShaderMap);
					GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
					SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
					PixelShader->SetParameters(RHICmdList, SamplerState, SrcTextureRHI, MipIndex);
				}
				else
				{
					TShaderMapRef<FScreenPSsRGBSourceMipLevel> PixelShader(ShaderMap);
					GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
					SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
					PixelShader->SetParameters(RHICmdList, SamplerState, SrcTextureRHI, MipIndex);
				}

				RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0.0f, DstRect.Min.X + ViewportWidth, DstRect.Min.Y + ViewportHeight, 1.0f);
#else
				if (!sRGBSource)
				{
					TShaderMapRef<FScreenPS> PixelShader(ShaderMap);
					GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
					SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
					PixelShader->SetParameters(RHICmdList, SamplerState, SrcTextureRHI);
				}
				else {
					TShaderMapRef<FScreenPSsRGBSource> PixelShader(ShaderMap);
					GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
					SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
					PixelShader->SetParameters(RHICmdList, SamplerState, SrcTextureRHI);
				}

				RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0.0f, DstRect.Max.X, DstRect.Max.Y, 1.0f);
#endif

				RendererModule->DrawRectangle(
					RHICmdList,
					0, 0, ViewportWidth, ViewportHeight,
					U, V, 1, VSize,
					TargetSize,
					FIntPoint(1, 1),
#if ENGINE_MINOR_VERSION > 24
					VertexShader,
#else
					* VertexShader,
#endif
					EDRF_Default);
			}
			RHICmdList.EndRenderPass();
		}
	}
}

//necessary, brush the rt on the Spectator screen, which is the window on the PC side
void FYvrPreviewHMD::CreateSpectatorScreenController()
{
	SpectatorScreenController = MakeUnique<FDefaultSpectatorScreenController>(this);
}

FIntRect FYvrPreviewHMD::GetFullFlatEyeRect_RenderThread(FTexture2DRHIRef EyeTexture) const
{
	static FVector2D SrcNormRectMin(0.05f, 0.2f);
	static FVector2D SrcNormRectMax(0.45f, 0.8f);
	return FIntRect(EyeTexture->GetSizeX() * SrcNormRectMin.X, EyeTexture->GetSizeY() * SrcNormRectMin.Y, EyeTexture->GetSizeX() * SrcNormRectMax.X, EyeTexture->GetSizeY() * SrcNormRectMax.Y);
}

void FYvrPreviewHMD::CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* SrcTexture, FIntRect SrcRect, FRHITexture2D* DstTexture, FIntRect DstRect, bool bClearBlack, bool bNoAlpha) const
{
	check(IsInRenderingThread());

	if (YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_CONNECTED)
	{
		FRHITexture2D* HMDTexture2DRHI = pBridge->GetHMDSwapChain()->GetTexture2D();
#if ENGINE_MINOR_VERSION > 25
		FRHICopyTextureInfo CopyInfoHMD;
		RHICmdList.CopyTexture(SrcTexture, HMDTexture2DRHI, CopyInfoHMD);
#else
		TransferImage_RenderThread(RHICmdList, SrcTexture, FIntRect(), HMDTexture2DRHI, FIntRect(), true);
#endif
		// send texture
		uint32 SwapChainIndex = pBridge->GetHMDSwapChain()->GetSwapChainIndex_RHIThread();
		YvrPreviewManager::GetInstance()->SendPresent(uint64_t(HMDDstTextureHandles[SwapChainIndex]));
		HMDDstSwapChainTextures[SwapChainIndex];
	}

	FRHITexture2D* DstTexture2D = DstTexture->GetTexture2D();
	FRHITextureCube* DstTextureCube = DstTexture->GetTextureCube();
	FRHITexture2D* SrcTexture2D = SrcTexture->GetTexture2DArray() ? SrcTexture->GetTexture2DArray() : SrcTexture->GetTexture2D();
	FRHITextureCube* SrcTextureCube = SrcTexture->GetTextureCube();
	FIntPoint DstSize;
	FIntPoint SrcSize;

	if (DstTexture2D && SrcTexture2D)
	{
		DstSize = FIntPoint(DstTexture2D->GetSizeX(), DstTexture2D->GetSizeY());
		SrcSize = FIntPoint(SrcTexture2D->GetSizeX(), SrcTexture2D->GetSizeY());
	}
	else if (DstTextureCube && SrcTextureCube)
	{
		DstSize = FIntPoint(DstTextureCube->GetSize(), DstTextureCube->GetSize());
		SrcSize = FIntPoint(SrcTextureCube->GetSize(), SrcTextureCube->GetSize());

	}
	else
	{
		return;
	}

	if (DstRect.IsEmpty())
	{
		DstRect = FIntRect(FIntPoint::ZeroValue, DstSize);
	}

	if (SrcRect.IsEmpty())
	{
		SrcRect = FIntRect(FIntPoint::ZeroValue, SrcSize);
	}

	const uint32 ViewportWidth = DstRect.Width();
	const uint32 ViewportHeight = DstRect.Height();
	const FIntPoint TargetSize(ViewportWidth, ViewportHeight);

	const float SrcTextureWidth = SrcTexture->GetSizeX();
	const float SrcTextureHeight = SrcTexture->GetSizeY();
	float U = 0.f, V = 0.f, USize = 1.f, VSize = 1.f;
	if (!SrcRect.IsEmpty())
	{
		U = SrcRect.Min.X / SrcTextureWidth;
		V = SrcRect.Min.Y / SrcTextureHeight;
		USize = SrcRect.Width() / SrcTextureWidth;
		VSize = SrcRect.Height() / SrcTextureHeight;
	}
	FRHITexture* SrcTextureRHI = SrcTexture;
#if ENGINE_MINOR_VERSION > 25
	RHICmdList.Transition(FRHITransitionInfo(SrcTexture, ERHIAccess::Unknown, ERHIAccess::SRVGraphics));
#else
	RHICmdList.TransitionResources(EResourceTransitionAccess::EReadable, &SrcTextureRHI, 1);
#endif

	// #todo-renderpasses Possible optimization here - use DontLoad if we will immediately clear the entire target
	FRHIRenderPassInfo RPInfo(DstTexture, ERenderTargetActions::Load_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("CopyTexture"));
	{
		if (bClearBlack)
		{
			const FIntRect ClearRect(0, 0, DstTexture->GetSizeX(), DstTexture->GetSizeY());
			RHICmdList.SetViewport(ClearRect.Min.X, ClearRect.Min.Y, 0, ClearRect.Max.X, ClearRect.Max.Y, 1.0f);
			DrawClearQuad(RHICmdList, FLinearColor::Black);
		}

		RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0, DstRect.Max.X, DstRect.Max.Y, 1.0f);

		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.BlendState = bNoAlpha ? TStaticBlendState<>::GetRHI() : TStaticBlendState<CW_RGBA, BO_Add, BF_SourceAlpha, BF_InverseSourceAlpha, BO_Add, BF_One, BF_InverseSourceAlpha>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;

		const auto FeatureLevel = GMaxRHIFeatureLevel;
		auto ShaderMap = GetGlobalShaderMap(FeatureLevel);

		TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
#if ENGINE_MINOR_VERSION > 24
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
#else
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
#endif
		const bool bSameSize = DstRect.Size() == SrcRect.Size();
		FRHISamplerState* PixelSampler = bSameSize ? TStaticSamplerState<SF_Point>::GetRHI() : TStaticSamplerState<SF_Bilinear>::GetRHI();

#if ENGINE_MINOR_VERSION > 24
		if ((SrcTexture->GetFlags() & TexCreate_SRGB) != 0)
		{
			TShaderMapRef<FScreenPSsRGBSource> PixelShader(ShaderMap);
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
			PixelShader->SetParameters(RHICmdList, PixelSampler, SrcTexture);
		}
		else
		{
			TShaderMapRef<FScreenPS> PixelShader(ShaderMap);
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
			PixelShader->SetParameters(RHICmdList, PixelSampler, SrcTexture);
		}
#else
		if ((SrcTexture->GetFlags() & TexCreate_SRGB) != 0)
		{
			TShaderMapRef<FScreenPSsRGBSource> PixelShader(ShaderMap);
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
			PixelShader->SetParameters(RHICmdList, PixelSampler, SrcTexture);
		}
		else
		{
			TShaderMapRef<FScreenPS> PixelShader(ShaderMap);
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
			PixelShader->SetParameters(RHICmdList, PixelSampler, SrcTexture);
		}
#endif

		RendererModule->DrawRectangle(
			RHICmdList,
			0, 0,
			ViewportWidth, ViewportHeight,
			U, V,
			USize, VSize,
			TargetSize,
			FIntPoint(1, 1),
#if ENGINE_MINOR_VERSION > 24
			VertexShader,
#else
			* VertexShader,
#endif
			EDRF_Default);
	}
	RHICmdList.EndRenderPass();
}

void FYvrPreviewHMD::RenderTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* BackBuffer, FRHITexture2D* SrcTexture, FVector2D WindowSize) const
{
	check(IsInRenderingThread());

	if (bSplashIsShown || !IsBackgroundLayerVisible())
	{
		FRHIRenderPassInfo RPInfo(SrcTexture, ERenderTargetActions::DontLoad_Store);
		RHICmdList.BeginRenderPass(RPInfo, TEXT("Clear"));
		{
			DrawClearQuad(RHICmdList, FLinearColor(0, 0, 0, 0));
		}
		RHICmdList.EndRenderPass();
	}

	check(SpectatorScreenController);
	SpectatorScreenController->RenderSpectatorScreen_RenderThread(RHICmdList, BackBuffer, SrcTexture, WindowSize);
}

void FYvrPreviewHMD::PostRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
}

#if ENGINE_MINOR_VERSION >26
bool FYvrPreviewHMD::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	return GEngine && GEngine->IsStereoscopic3D(Context.Viewport);
}
#endif

int ColorRGBA(int alpha, int red, int green, int blue)
{
	return (alpha << 24 | red << 16 | green << 8 | blue);
}

static constexpr uint32 YvrDPSwapChainLength = 3;

bool FYvrPreviewHMD::CreateSharedTexture2D()
{
	auto device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
	if (device == nullptr)
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC sharedTexture2Ddesc;
	ZeroMemory(&sharedTexture2Ddesc, sizeof(sharedTexture2Ddesc));
	sharedTexture2Ddesc.Height = IdealRenderTargetSize[1]; //m_lastSize.Height;
	sharedTexture2Ddesc.Width = IdealRenderTargetSize[0]; // m_lastSize.Width;
	sharedTexture2Ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	sharedTexture2Ddesc.ArraySize = 1;
	sharedTexture2Ddesc.MipLevels = 1;
	sharedTexture2Ddesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexture2Ddesc.SampleDesc.Count = 1;
	sharedTexture2Ddesc.CPUAccessFlags = 0;
	sharedTexture2Ddesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	sharedTexture2Ddesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

	uint32_t m_color = ColorRGBA(255, 108, 108, 255);

	std::vector<uint32_t> textureMap(IdealRenderTargetSize[0] * IdealRenderTargetSize[1], m_color);
	uint32_t* pData = textureMap.data();

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pData;
	sd.SysMemPitch = IdealRenderTargetSize[0] * sizeof(uint32_t);
	sd.SysMemSlicePitch = IdealRenderTargetSize[0] * IdealRenderTargetSize[1] * sizeof(uint32_t);

	for (uint32 SwapChainIter = 0; SwapChainIter < YvrDPSwapChainLength; ++SwapChainIter)
	{
		TRefCountPtr<ID3D11Texture2D> HMDDstTexture;
		void* HMDDstTextureHandle;
		TRefCountPtr<IDXGIResource> HMDDXGIResource;
		FTexture2DRHIRef HMDTempTexture;

		if (device->CreateTexture2D(&sharedTexture2Ddesc, &sd, HMDDstTexture.GetInitReference()) != S_OK)
		{
			return false;
		}

		HMDDstSwapChainTextures.Add(HMDDstTexture);

		if (HMDDstSwapChainTextures[SwapChainIter])
		{
			HMDDstSwapChainTextures[SwapChainIter]->QueryInterface(HMDDXGIResource.GetInitReference());

			const HRESULT HMDSharedHandleResult = HMDDXGIResource->GetSharedHandle(&HMDDstTextureHandle);

			HMDDstTextureHandles.Add(HMDDstTextureHandle);

			if (FAILED(HMDSharedHandleResult))
			{
				return false;
			}
			else
			{
				CreateRHITexture(HMDDstSwapChainTextures[SwapChainIter], PF_R8G8B8A8, HMDTempTexture);
				HMDRHITextureSwapChain.Add((FTextureRHIRef&)HMDTempTexture);
			}

			if (HMDBindingTexture == nullptr)
			{
				HMDBindingTexture = GDynamicRHI->RHICreateAliasedTexture((FTextureRHIRef&)HMDTempTexture);
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool FYvrPreviewHMD::CreateRHITexture(ID3D11Texture2D* OpenedSharedResource, EPixelFormat Format, FTexture2DRHIRef& OutTexture)
{
	FD3D11DynamicRHI* DynamicRHI = static_cast<FD3D11DynamicRHI*>(GDynamicRHI);
	OutTexture = DynamicRHI->RHICreateTexture2DFromResource(
		Format, TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_SRGB, FClearValueBinding::None, OpenedSharedResource).GetReference();
	return OutTexture.IsValid();
}

void FYvrPreviewHMD::BridgeBaseImpl::BeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList)
{
}

void FYvrPreviewHMD::BridgeBaseImpl::BeginRendering_RHI()
{
	check(!IsRunningRHIInSeparateThread() || IsInRHIThread());
}

void FYvrPreviewHMD::BridgeBaseImpl::CreatePCSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures)
{
	check(IsInRenderingThread());
	check(SwapChainTextures.Num());
	PCSwapChain = CreateXRSwapChain(MoveTemp(SwapChainTextures), BindingTexture);
}

void FYvrPreviewHMD::BridgeBaseImpl::CreateHMDSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures)
{
	check(IsInRenderingThread());
	check(SwapChainTextures.Num());
	HMDSwapChain = CreateXRSwapChain(MoveTemp(SwapChainTextures), BindingTexture);
}

bool FYvrPreviewHMD::BridgeBaseImpl::Present(int& SyncInterval)
{
	//This  must return true
	check(IsRunningRHIInSeparateThread() ? IsInRHIThread() : IsInRenderingThread());

	//necessary, brush the RT to steam
	FinishRendering();
	// Increment swap chain index post-swap.
	PCSwapChain->IncrementSwapChainIndex_RHIThread();

	if (YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_CONNECTED)
	{
		GetHMDSwapChain()->IncrementSwapChainIndex_RHIThread();
	}
	SyncInterval = 0;

	return true;
}

bool FYvrPreviewHMD::BridgeBaseImpl::NeedsNativePresent()
{
	//This return value does not affect the PC display
	return true;
}

void FYvrPreviewHMD::BridgeBaseImpl::PostPresent()
{
}

class FYvrPreviewHMDPlugin : public IYvrPreviewHMDPlugin
{
public:
	virtual TSharedPtr< class IXRTrackingSystem, ESPMode::ThreadSafe > CreateTrackingSystem() override;

	FString GetModuleKeyName() const override
	{
		return FString(TEXT("YvrPreviewHMD"));
	}

	virtual void StartupModule() override
	{
		IHeadMountedDisplayModule::StartupModule();
	}

	virtual void ShutdownModule() override
	{
		IHeadMountedDisplayModule::ShutdownModule();
	}

	bool Initialize()
	{
		return true;
	}

	virtual bool IsHMDConnected() override
	{
		return true;
	}

#if PLATFORM_WINDOWS
	enum class D3DApiLevel
	{
		Undefined,
		Direct3D11,
		Direct3D12
	};
	// check D3D API
	static inline D3DApiLevel GetD3DApiLevel()
	{
		FString RHIString;
		{
			FString HardwareDetails = FHardwareInfo::GetHardwareDetailsString();
			FString RHILookup = NAME_RHI.ToString() + TEXT("=");

			if (!FParse::Value(*HardwareDetails, *RHILookup, RHIString))
			{
				// RHI might not be up yet. Let's check the command-line and see if DX12 was specified.
				// This will get hit on startup since we don't have RHI details during stereo device bringup.
				// This is not a final fix; we should probably move the stereo device init to later on in startup.
				bool bForceD3D12 = FParse::Param(FCommandLine::Get(), TEXT("d3d12")) || FParse::Param(FCommandLine::Get(), TEXT("dx12"));
				return bForceD3D12 ? D3DApiLevel::Direct3D12 : D3DApiLevel::Direct3D11;
			}
		}

		if (RHIString == TEXT("D3D11"))
		{
			return D3DApiLevel::Direct3D11;
		}
		if (RHIString == TEXT("D3D12"))
		{
			return D3DApiLevel::Direct3D12;
		}

		return D3DApiLevel::Undefined;
	}

#endif

};

IMPLEMENT_MODULE(FYvrPreviewHMDPlugin, YvrPreviewHMD)

TSharedPtr< class IXRTrackingSystem, ESPMode::ThreadSafe > FYvrPreviewHMDPlugin::CreateTrackingSystem()
{
	auto level = FYvrPreviewHMDPlugin::GetD3DApiLevel();

	if (level == FYvrPreviewHMDPlugin::D3DApiLevel::Direct3D11)
	{
		TSharedPtr<IXRTrackingSystem, ESPMode::ThreadSafe> PreviewHMD = FSceneViewExtensions::NewExtension<FYvrPreviewHMD>(this);
		if (PreviewHMD)
		{
			return PreviewHMD;
		}
	}

	return nullptr;
}


