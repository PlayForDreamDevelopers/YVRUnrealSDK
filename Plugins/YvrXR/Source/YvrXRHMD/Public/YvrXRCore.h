// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "openxr.h"
#include "openxr_reflection.h"
#include "openxr_yvr.h"

#define XR_ENUM_CASE_STR(name, val) case name: return TEXT(#name);
constexpr const TCHAR* OpenXRResultToString(XrResult e)
{
	switch (e)
	{
		XR_LIST_ENUM_XrResult(XR_ENUM_CASE_STR);
		default: return TEXT("Unknown");
	}
}

#define XR_SESSION_STATE_STR(name, val) case name: return TEXT(#name);
constexpr const TCHAR* OpenXRSessionStateToString(XrSessionState e)
{
	switch (e)
	{
		XR_LIST_ENUM_XrSessionState(XR_SESSION_STATE_STR);
	default: return TEXT("Unknown");
	}
}

#if DO_CHECK
#define XR_ENSURE(x) [] (XrResult Result) \
	{ \
		return ensureMsgf(XR_SUCCEEDED(Result), TEXT("OpenXR call failed with result %s"), OpenXRResultToString(Result)); \
	} (x)
#else
#define XR_ENSURE(x) XR_SUCCEEDED(x)
#endif

FORCEINLINE FQuat ToFQuat(XrQuaternionf Quat)
{
	return FQuat((float)-Quat.z, (float)Quat.x, (float)Quat.y, (float)-Quat.w);
}

FORCEINLINE XrQuaternionf ToXrQuat(FQuat Quat)
{
	return XrQuaternionf{ (float)Quat.Y, (float)Quat.Z, (float)-Quat.X, (float)-Quat.W };
}

FORCEINLINE FVector ToFVector(XrVector3f Vector, float Scale = 1.0f)
{
	return FVector((float)-Vector.z * Scale, (float)Vector.x * Scale, (float)Vector.y * Scale);
}

FORCEINLINE XrVector3f ToXrVector(FVector Vector, float Scale = 1.0f)
{
	if (Vector.IsZero())
		return XrVector3f{ 0.0f, 0.0f, 0.0f };

	return XrVector3f{ (float)Vector.Y / Scale, (float)Vector.Z / Scale, (float)-Vector.X / Scale };
}

FORCEINLINE FTransform ToFTransform(XrPosef Transform, float Scale = 1.0f)
{
	return FTransform(ToFQuat(Transform.orientation), ToFVector(Transform.position, Scale));
}

FORCEINLINE XrPosef ToXrPose(FTransform Transform, float Scale = 1.0f)
{
	return XrPosef{ ToXrQuat(Transform.GetRotation()), ToXrVector(Transform.GetTranslation(), Scale) };
}

FORCEINLINE FTimespan ToFTimespan(XrTime Time)
{
	// XrTime is a nanosecond counter, FTimespan is a 100-nanosecond counter. 
	// We are losing some precision here.
	return FTimespan((Time + 50) / 100); 
}

FORCEINLINE XrTime ToXrTime(FTimespan Time)
{
	return Time.GetTicks() * 100;
}

FORCEINLINE FIntRect ToFIntRect(XrRect2Di Rect)
{
	return FIntRect(Rect.offset.x, Rect.offset.y, Rect.offset.x + Rect.extent.width, Rect.offset.y + Rect.extent.height);
}

FORCEINLINE XrRect2Di ToXrRect(FIntRect Rect)
{
	return XrRect2Di{ { Rect.Min.X, Rect.Min.Y }, { Rect.Width(), Rect.Height() } };
}

FORCEINLINE FVector2D ToFVector2D(XrExtent2Df Extent, float Scale = 1.0f)
{
	return FVector2D(Extent.width * Scale, Extent.height * Scale);
}

FORCEINLINE XrVector2f ToXrVector2f(FVector2D Vector, float Scale = 1.0f)
{
	return XrVector2f{ (float)Vector.X / Scale, (float)Vector.Y / Scale };
}

FORCEINLINE XrExtent2Df ToXrExtent2D(FVector2D Vector, float Scale = 1.0f)
{
	if (Vector.IsZero())
		return XrExtent2Df{ 0.0f, 0.0f };

	return XrExtent2Df{ (float)Vector.X / Scale, (float)Vector.Y / Scale };
}


FORCEINLINE FBox2D ToFBox2D(XrRect2Df Rect, float Scale = 1.0f)
{
	return FBox2D{ FVector2D(Rect.offset.x, Rect.offset.y) * Scale, FVector2D(Rect.offset.x + Rect.extent.width, Rect.offset.y + Rect.extent.height) * Scale };
}

FORCEINLINE FBox ToFBox(XrRect3DfFB Rect, float Scale = 1.0f)
{
	return FBox{ FVector(Rect.offset.x, Rect.offset.y, Rect.offset.z) * Scale, FVector(Rect.offset.x + Rect.extent.width, Rect.offset.y + Rect.extent.height, Rect.offset.z + Rect.extent.depth) * Scale };
}

FORCEINLINE FString FormatToHexString(uint64 From)
{
	if (From == 0)
	{
 		return TEXT("0");
	}

	FString To;

	static FString Hex = TEXT("0123456789abcdef");

	uint64 HexNumber = From;
	while (HexNumber)
	{
 		int32 Index = HexNumber & 0x0f;
 		To += Hex[Index];
 		HexNumber >>= 4;
	}

	return To.Reverse();
}

union FXrCompositionLayerUnion
{
	XrCompositionLayerBaseHeader Header;
	XrCompositionLayerProjection Projection;
	XrCompositionLayerQuad Quad;
	XrCompositionLayerCylinderKHR Cylinder;
	XrCompositionLayerEquirectKHR Equirect;
	XrCompositionLayerCubeKHR Cube;
};

/** List all OpenXR global entry points used by Unreal. */
#define ENUM_XR_ENTRYPOINTS_GLOBAL(EnumMacro) \
	EnumMacro(PFN_xrEnumerateApiLayerProperties,xrEnumerateApiLayerProperties) \
	EnumMacro(PFN_xrEnumerateInstanceExtensionProperties,xrEnumerateInstanceExtensionProperties) \
	EnumMacro(PFN_xrCreateInstance,xrCreateInstance)

/** List all OpenXR instance entry points used by Unreal. */
#define ENUM_XR_ENTRYPOINTS(EnumMacro) \
	EnumMacro(PFN_xrDestroyInstance,xrDestroyInstance) \
	EnumMacro(PFN_xrGetInstanceProperties,xrGetInstanceProperties) \
	EnumMacro(PFN_xrPollEvent,xrPollEvent) \
	EnumMacro(PFN_xrResultToString,xrResultToString) \
	EnumMacro(PFN_xrStructureTypeToString,xrStructureTypeToString) \
	EnumMacro(PFN_xrGetSystem,xrGetSystem) \
	EnumMacro(PFN_xrGetSystemProperties,xrGetSystemProperties) \
	EnumMacro(PFN_xrEnumerateEnvironmentBlendModes,xrEnumerateEnvironmentBlendModes) \
	EnumMacro(PFN_xrCreateSession,xrCreateSession) \
	EnumMacro(PFN_xrDestroySession,xrDestroySession) \
	EnumMacro(PFN_xrEnumerateReferenceSpaces,xrEnumerateReferenceSpaces) \
	EnumMacro(PFN_xrCreateReferenceSpace,xrCreateReferenceSpace) \
	EnumMacro(PFN_xrGetReferenceSpaceBoundsRect,xrGetReferenceSpaceBoundsRect) \
	EnumMacro(PFN_xrCreateActionSpace,xrCreateActionSpace) \
	EnumMacro(PFN_xrLocateSpace,xrLocateSpace) \
	EnumMacro(PFN_xrDestroySpace,xrDestroySpace) \
	EnumMacro(PFN_xrEnumerateViewConfigurations,xrEnumerateViewConfigurations) \
	EnumMacro(PFN_xrGetViewConfigurationProperties,xrGetViewConfigurationProperties) \
	EnumMacro(PFN_xrEnumerateViewConfigurationViews,xrEnumerateViewConfigurationViews) \
	EnumMacro(PFN_xrEnumerateSwapchainFormats,xrEnumerateSwapchainFormats) \
	EnumMacro(PFN_xrCreateSwapchain,xrCreateSwapchain) \
	EnumMacro(PFN_xrDestroySwapchain,xrDestroySwapchain) \
	EnumMacro(PFN_xrEnumerateSwapchainImages,xrEnumerateSwapchainImages) \
	EnumMacro(PFN_xrAcquireSwapchainImage,xrAcquireSwapchainImage) \
	EnumMacro(PFN_xrWaitSwapchainImage,xrWaitSwapchainImage) \
	EnumMacro(PFN_xrReleaseSwapchainImage,xrReleaseSwapchainImage) \
	EnumMacro(PFN_xrBeginSession,xrBeginSession) \
	EnumMacro(PFN_xrEndSession,xrEndSession) \
	EnumMacro(PFN_xrRequestExitSession,xrRequestExitSession) \
	EnumMacro(PFN_xrWaitFrame,xrWaitFrame) \
	EnumMacro(PFN_xrBeginFrame,xrBeginFrame) \
	EnumMacro(PFN_xrEndFrame,xrEndFrame) \
	EnumMacro(PFN_xrLocateViews,xrLocateViews) \
	EnumMacro(PFN_xrStringToPath,xrStringToPath) \
	EnumMacro(PFN_xrPathToString,xrPathToString) \
	EnumMacro(PFN_xrCreateActionSet,xrCreateActionSet) \
	EnumMacro(PFN_xrDestroyActionSet,xrDestroyActionSet) \
	EnumMacro(PFN_xrCreateAction,xrCreateAction) \
	EnumMacro(PFN_xrDestroyAction,xrDestroyAction) \
	EnumMacro(PFN_xrSuggestInteractionProfileBindings,xrSuggestInteractionProfileBindings) \
	EnumMacro(PFN_xrAttachSessionActionSets,xrAttachSessionActionSets) \
	EnumMacro(PFN_xrGetCurrentInteractionProfile,xrGetCurrentInteractionProfile) \
	EnumMacro(PFN_xrGetActionStateBoolean,xrGetActionStateBoolean) \
	EnumMacro(PFN_xrGetActionStateFloat,xrGetActionStateFloat) \
	EnumMacro(PFN_xrGetActionStateVector2f,xrGetActionStateVector2f) \
	EnumMacro(PFN_xrGetActionStatePose,xrGetActionStatePose) \
	EnumMacro(PFN_xrSyncActions,xrSyncActions) \
	EnumMacro(PFN_xrEnumerateBoundSourcesForAction,xrEnumerateBoundSourcesForAction) \
	EnumMacro(PFN_xrGetInputSourceLocalizedName,xrGetInputSourceLocalizedName) \
	EnumMacro(PFN_xrApplyHapticFeedback,xrApplyHapticFeedback) \
	EnumMacro(PFN_xrStopHapticFeedback,xrStopHapticFeedback)

/** Declare all XR functions in a namespace to avoid conflicts with the loader exported symbols. */
#define DECLARE_XR_ENTRYPOINTS(Type,Func) extern Type YVRXRHMD_API Func;
namespace OpenXRDynamicAPI
{
	ENUM_XR_ENTRYPOINTS_GLOBAL(DECLARE_XR_ENTRYPOINTS);
	ENUM_XR_ENTRYPOINTS(DECLARE_XR_ENTRYPOINTS);
	DECLARE_XR_ENTRYPOINTS(PFN_xrGetInstanceProcAddr, xrGetInstanceProcAddr)
}
using namespace OpenXRDynamicAPI;

/**
 * Initialize essential OpenXR functions.
 * @returns true if initialization was successful.
 */
bool PreInitOpenXRCore(PFN_xrGetInstanceProcAddr InGetProcAddr);

/**
 * Initialize core OpenXR functions.
 * @returns true if initialization was successful.
 */
bool InitOpenXRCore(XrInstance Instance);
