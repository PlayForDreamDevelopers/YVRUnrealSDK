#pragma once
#include "CoreMinimal.h"
#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
typedef jobject YvrMessageHandle;
#else
typedef void* YvrMessageHandle;
#endif

typedef FString YvrMessageType;

class FYvrBusinessWrapper
{
public:
#if PLATFORM_ANDROID
	// tobSDK
	static void Init();
	static FString GetDeviceSn();
	static FString GetDeviceModel();
	static FString GetSoftwareVersion();

#endif
};
