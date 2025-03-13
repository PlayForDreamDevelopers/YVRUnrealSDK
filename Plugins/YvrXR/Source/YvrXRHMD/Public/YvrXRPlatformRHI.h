// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#if PLATFORM_ANDROID
#define XR_USE_PLATFORM_ANDROID 1
#define XR_USE_GRAPHICS_API_OPENGL_ES 1
#define XR_USE_GRAPHICS_API_VULKAN 1
#endif

//-------------------------------------------------------------------------------------------------
// OpenGL
//-------------------------------------------------------------------------------------------------

#ifdef XR_USE_GRAPHICS_API_OPENGL_ES
#include "OpenGLDrvPrivate.h"
#include "OpenGLResources.h"
#endif // XR_USE_GRAPHICS_API_OPENGL_ES


//-------------------------------------------------------------------------------------------------
// Vulkan
//-------------------------------------------------------------------------------------------------

#ifdef XR_USE_GRAPHICS_API_VULKAN
#include "VulkanRHIPrivate.h"
#include "VulkanResources.h"
#endif // XR_USE_GRAPHICS_API_VULKAN

#if PLATFORM_ANDROID
#include <android_native_app_glue.h>
#endif

#include "openxr_platform.h"
