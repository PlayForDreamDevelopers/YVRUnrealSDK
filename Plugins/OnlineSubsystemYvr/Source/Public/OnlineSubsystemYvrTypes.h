// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
typedef jobject YvrMessageHandle;
#else
typedef void* YvrMessageHandle;
#endif

typedef FString YvrMessageType;

typedef enum YvrUserPresenceStatus_ {
	YvrUserPresenceStatus_Unknown = 0,
	YvrUserPresenceStatus_Online,
	YvrUserPresenceStatus_Offline,
} YvrUserPresenceStatus;