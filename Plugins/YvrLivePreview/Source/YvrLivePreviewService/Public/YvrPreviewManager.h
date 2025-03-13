// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "binder/IServiceConnection.h"
#include "interface/IPreviewToolService.h"
#include "proxy/ProxyService.h"
#include "YVR_Lib_Types.h"


#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogYvrPreview, Log, All);

DECLARE_MULTICAST_DELEGATE(FConnectDelegate);
#define SERVICE_PREVIEW_TOOL "service_preview_tool"

typedef enum PreviewServiceState
{
	PREVIEW_START = 0,
	PREVIEW_CONNECTED = 1,
	PREVIEW_STOP = 2,
};

/**
 * manage the data transfer between the server and Unreal
 */
class YVRLIVEPREVIEWSERVICE_API YvrPreviewManager : public  IServiceConnection
{
public:

	static YvrPreviewManager* GetInstance();
	void Start();
	void Stop();
	PreviewServiceState GetServiceState();

	// HMD
	void GetHeadTrackingData(yvrPoseState* headPose);
	void SendPresent(uint64_t in_textureHandle);
	void GetResolution(int& width, int& height);

	// Controller
	void GetControllerPose(EControllerHand DeviceHand, yvrPoseState* handPose, bool* controlleState);
	void GetControllerState(EControllerHand DeviceHand, yvrControllerState* controllerState, bool* controlleState);

	FConnectDelegate connectEvent;

private:
	YvrPreviewManager();
	~YvrPreviewManager();

	IPreviewToolService* yvrPreviewToolService;

	virtual void onServiceConnected(IBinder* service) override;
	virtual void onServiceDisconnected() override;
	bool isConnect = false;

	/** init */
	bool isStart = false;
	yvrTrackingData trackingData{};

};
