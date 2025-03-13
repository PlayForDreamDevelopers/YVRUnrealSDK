// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrPreviewManager.h"
#if PLATFORM_WINDOWS
#include "D3D11RHIPrivate.h"
#endif
#include <wrl/client.h>

DEFINE_LOG_CATEGORY(LogYvrPreview);

YvrPreviewManager::YvrPreviewManager()
{
}

YvrPreviewManager::~YvrPreviewManager()
{
}

void YvrPreviewManager::Start()
{
	ProxyService::instance().start();
	isStart = true;
	ProxyService::instance().bindService(SERVICE_PREVIEW_TOOL, this);
}

void YvrPreviewManager::onServiceConnected(IBinder* service)
{
	UE_LOG(LogYvrPreview, Log, TEXT("onServiceConnected"));

	isConnect = true;
	yvrPreviewToolService = dynamic_cast<IPreviewToolService*>(service);

	auto device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter> dsgIAdapter = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice = nullptr;
	DXGI_ADAPTER_DESC adapterDesc;
	d3dDevice = device;
	d3dDevice.As(&dxgiDevice);
	dxgiDevice->GetAdapter(dsgIAdapter.GetAddressOf());
	dsgIAdapter->GetDesc(&adapterDesc);
	yvrPreviewToolService->setGraphicsDevice(adapterDesc.DeviceId);

	connectEvent.Broadcast();

}

void YvrPreviewManager::onServiceDisconnected()
{
	isConnect = false;
	UE_LOG(LogYvrPreview, Log, TEXT("onServiceDisconnected"));
}

void YvrPreviewManager::Stop()
{
	UE_LOG(LogYvrPreview, Log, TEXT("start preview"));
	isStart = false;
	ProxyService::instance().stop();
}

YvrPreviewManager* YvrPreviewManager::GetInstance()
{
	static YvrPreviewManager Instance;
	return &Instance;
}

PreviewServiceState YvrPreviewManager::GetServiceState()
{
	if (isStart)
	{
		if (isConnect)
		{
			return PREVIEW_CONNECTED;
		}
		else
		{
			return PREVIEW_START;
		}
	}
	else
	{
		return PREVIEW_STOP;
	}
}

void YvrPreviewManager::GetHeadTrackingData(yvrPoseState* headPose)
{

	if (YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_CONNECTED)
	{
		yvrTrackingData yvrHMD;
		yvrPreviewToolService->getTrackingData(&yvrHMD);

		memcpy(headPose, &yvrHMD.headPose, sizeof(yvrPoseState));
		memcpy(&trackingData, &yvrHMD, sizeof(yvrTrackingData));
	}
}

void YvrPreviewManager::GetControllerPose(EControllerHand DeviceHand, yvrPoseState* controllerPose,bool* controllerConnect)
{
	if (DeviceHand == EControllerHand::Left)
	{
		memcpy(controllerPose, &trackingData.leftControllerPose, sizeof(yvrPoseState));
		memcpy(controllerConnect, &trackingData.controllerConnectedInfo.left_connected, sizeof(bool));
	}
	else if (DeviceHand == EControllerHand::Right)
	{
		memcpy(controllerPose, &trackingData.rightControllerPose, sizeof(yvrPoseState));
		memcpy(controllerConnect, &trackingData.controllerConnectedInfo.right_connected, sizeof(bool));
	}
}

void YvrPreviewManager::GetControllerState(EControllerHand DeviceHand, yvrControllerState* controllerState, bool* controllerConnect)
{
	if (DeviceHand == EControllerHand::Left)
	{
		memcpy(controllerState, &trackingData.leftControllerState, sizeof(yvrControllerState));
		memcpy(controllerConnect, &trackingData.controllerConnectedInfo.left_connected, sizeof(bool));
	}
	else if (DeviceHand == EControllerHand::Right)
	{
		memcpy(controllerState, &trackingData.rightControllerState, sizeof(yvrControllerState));
		memcpy(controllerConnect, &trackingData.controllerConnectedInfo.right_connected, sizeof(bool));
	}
}

void YvrPreviewManager::SendPresent(uint64_t in_textureHandle)
{
	if (YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_CONNECTED)
	{
		yvrPreviewToolService->present(in_textureHandle);
	}
}

void YvrPreviewManager::GetResolution(int& width, int& height)
{
	if (YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_CONNECTED)
	{
		yvrPreviewToolService->getDisplayResolution(width, height);
	}
}
