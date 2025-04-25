#include "YvrBusinessBPLibrary.h"
#include "YvrBusinessWrapper.h"
#include "YvrBusiness.h"
#include "YvrInterfaceWrapper.h"

UYvrBusinessBPLibrary::UYvrBusinessBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

FString UYvrBusinessBPLibrary::GetDeviceSn()
{
#if PLATFORM_ANDROID
	return FYvrBusinessWrapper::GetDeviceSn();
#endif
	return FString("current platform is windows");
}

FString UYvrBusinessBPLibrary::GetDeviceModel()
{
#if PLATFORM_ANDROID
	return FYvrBusinessWrapper::GetDeviceModel();
#endif
	return FString("current platform is windows");
}

FString UYvrBusinessBPLibrary::GetSoftwareVersion()
{
#if PLATFORM_ANDROID
	return FYvrBusinessWrapper::GetSoftwareVersion();
#endif
	return FString("current platform is windows");
}

bool UYvrBusinessBPLibrary::OpenVSTCamera()
{
	return FYvrInterfaceWrapper::OpenVSTCamera() == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::CloseVSTCamera()
{
	return FYvrInterfaceWrapper::CloseVSTCamera() == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::AcquireVSTCameraFrame(FVSTCameraFrame& OutFrame)
{
	pfdm_xr_vst_camera_frame_item_ext_t OutRawFrame;
	pfdm_xr_camera_error_t result = FYvrInterfaceWrapper::AcquireVSTCameraFrame(&OutRawFrame);

	if (result != pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR)
	{
		return false;
	}

	OutFrame.FrameItem.FrameNumber = OutRawFrame.frame.frame_number;
	OutFrame.FrameItem.Width = OutRawFrame.frame.width;
	OutFrame.FrameItem.Height = OutRawFrame.frame.height;
	OutFrame.FrameItem.Format = OutRawFrame.frame.format;
	OutFrame.FrameItem.ExposureDuration = OutRawFrame.frame.exposure_duration;
	OutFrame.FrameItem.SoeTimestamp = OutRawFrame.frame.soe_timestamp;
	OutFrame.FrameItem.SoeTimestampQ = OutRawFrame.frame.soe_timestamp_q;
	OutFrame.FrameItem.Gain = OutRawFrame.frame.gain;
	OutFrame.FrameItem.DataSize = OutRawFrame.frame.data_size;

	if (OutRawFrame.frame.data[0] != nullptr)
	{
		OutFrame.FrameItem.LeftEyeData.SetNum(OutFrame.FrameItem.DataSize);
		FMemory::Memcpy(OutFrame.FrameItem.LeftEyeData.GetData(), (void*)OutRawFrame.frame.data[0], sizeof(uint8) * OutFrame.FrameItem.DataSize);
	}

	if (OutRawFrame.frame.data[1] != nullptr)
	{
		OutFrame.FrameItem.RightEyeData.SetNum(OutFrame.FrameItem.DataSize);
		FMemory::Memcpy(OutFrame.FrameItem.RightEyeData.GetData(), (void*)OutRawFrame.frame.data[1], sizeof(uint8) * OutFrame.FrameItem.DataSize);
	}
	
	OutFrame.SixDofPose.Timestamp = OutRawFrame.six_dof_pose.timestamp;
	OutFrame.SixDofPose.Position = FVector(OutRawFrame.six_dof_pose.x, OutRawFrame.six_dof_pose.y, OutRawFrame.six_dof_pose.z);
	OutFrame.SixDofPose.Rotation = FQuat(OutRawFrame.six_dof_pose.rx, OutRawFrame.six_dof_pose.ry, OutRawFrame.six_dof_pose.rz, OutRawFrame.six_dof_pose.rw);
	OutFrame.SixDofPose.Confidence = OutRawFrame.six_dof_pose.confidence;
	OutFrame.SixDofPose.WarningFlag = OutRawFrame.six_dof_pose.warning_flag;
	OutFrame.SixDofPose.LinearVelocity = FVector(OutRawFrame.six_dof_pose.vx, OutRawFrame.six_dof_pose.vy, OutRawFrame.six_dof_pose.vz);
	OutFrame.SixDofPose.LinearAcceleration = FVector(OutRawFrame.six_dof_pose.ax, OutRawFrame.six_dof_pose.ay, OutRawFrame.six_dof_pose.az);
	OutFrame.SixDofPose.AngularVelocity = FVector(OutRawFrame.six_dof_pose.wx, OutRawFrame.six_dof_pose.wy, OutRawFrame.six_dof_pose.wz);
	OutFrame.SixDofPose.AngularAcceleration = FVector(OutRawFrame.six_dof_pose.w_ax, OutRawFrame.six_dof_pose.w_ay, OutRawFrame.six_dof_pose.w_az);

	return true;
}

bool UYvrBusinessBPLibrary::SetVSTCameraFrequency(EVSTCameraFrequency Frequency)
{
	return FYvrInterfaceWrapper::SetVSTCameraFrequency((pfdm_xr_vst_camera_frequency_cfg_t)Frequency) == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::GetVSTCameraFrequency(EVSTCameraFrequency& OutFrequency)
{
	pfdm_xr_vst_camera_frequency_cfg_t OutRawFrequency;
	pfdm_xr_camera_error_t result = FYvrInterfaceWrapper::GetVSTCameraFrequency(&OutRawFrequency);
	if (result != pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR)
	{
		return false;
	}

	OutFrequency = (EVSTCameraFrequency)OutRawFrequency;
	return true;
}

bool UYvrBusinessBPLibrary::SetVSTCameraResolution(EVSTCameraResolution Resolution)
{
	return FYvrInterfaceWrapper::SetVSTCameraResolution((pfdm_xr_vst_camera_resolution_cfg_t)Resolution) == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::GetVSTCameraResolution(EVSTCameraResolution& OutResolution)
{
	pfdm_xr_vst_camera_resolution_cfg_t OutRawResolution;
	pfdm_xr_camera_error_t result = FYvrInterfaceWrapper::GetVSTCameraResolution(&OutRawResolution);
	if (result != pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR)
	{
		return false;
	}

	OutResolution = (EVSTCameraResolution)OutRawResolution;
	return true;
}

bool UYvrBusinessBPLibrary::SetVSTCameraFormat(EVSTCameraFormat Format)
{
	return FYvrInterfaceWrapper::SetVSTCameraFormat((pfdm_xr_vst_camera_format_cfg_t)Format) == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::GetVSTCameraFormat(EVSTCameraFormat& OutFormat)
{
	pfdm_xr_vst_camera_format_cfg_t OutRawFormat;
	pfdm_xr_camera_error_t result = FYvrInterfaceWrapper::GetVSTCameraFormat(&OutRawFormat);
	if (result != pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR)
	{
		return false;
	}

	OutFormat = (EVSTCameraFormat)OutRawFormat;
	return true;
}

bool UYvrBusinessBPLibrary::SetVSTCameraOutputSource(EVSTCameraSource Source)
{
	return FYvrInterfaceWrapper::SetVSTCameraOutputSource((pfdm_xr_vst_camera_source_cfg_t)Source) == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::GetVSTCameraOutputSource(EVSTCameraSource& OutSource)
{
	pfdm_xr_vst_camera_source_cfg_t OutRawSource;
	pfdm_xr_camera_error_t result = FYvrInterfaceWrapper::GetVSTCameraOutputSource(&OutRawSource);
	if (result != pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR)
	{
		return false;
	}

	OutSource = (EVSTCameraSource)OutRawSource;
	return true;
}

bool UYvrBusinessBPLibrary::GetVSTCameraIntrinsicExtrinsic(EVSTCameraID ID, FVSTCameraIntrinsicExtrinsic& OutParams)
{
	pfdm_xr_vst_camera_intrinsic_extrinsic_t OutRawParams;
	pfdm_xr_camera_error_t result = FYvrInterfaceWrapper::GetVSTCameraIntrinsicExtrinsic((pfdm_xr_vst_camera_id_t)ID, &OutRawParams);
	if (result != pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR)
	{
		return false;
	}

	OutParams.FocalLength.X = OutRawParams.fx;
	OutParams.FocalLength.Y = OutRawParams.fy;

	OutParams.PrincipalPoint.Y = OutRawParams.cx;
	OutParams.PrincipalPoint.Y = OutRawParams.cy;

	OutParams.Position.X = OutRawParams.x;
	OutParams.Position.Y = OutRawParams.y;
	OutParams.Position.Z = OutRawParams.z;

	OutParams.Rotation.X = OutRawParams.rx;
	OutParams.Rotation.Y = OutRawParams.ry;
	OutParams.Rotation.Z = OutRawParams.rz;
	OutParams.Rotation.W = OutRawParams.rw;

	OutParams.Distortion.SetNum(16);
	FMemory::Memcpy(OutParams.Distortion.GetData(), OutRawParams.distortion, sizeof(float) * 16);
	
	return true;
}

bool UYvrBusinessBPLibrary::OpenTrackingCamera(ECameraType Type)
{
	return FYvrInterfaceWrapper::OpenTrackingCamera((pfdm_xr_camera_type_t)Type) == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::CloseTrackingCamera(ECameraType Type)
{
	return FYvrInterfaceWrapper::CloseTrackingCamera((pfdm_xr_camera_type_t)Type) == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::SubscribeFrame(ECameraType Type)
{
	return FYvrInterfaceWrapper::SubscribeFrame((pfdm_xr_camera_type_t)Type) == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::UnSubscribeFrame(ECameraType Type)
{
	return FYvrInterfaceWrapper::UnSubscribeFrame((pfdm_xr_camera_type_t)Type) == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::AcquireTrackingCameraFrame(ECameraType Type, FVSTCameraFrameItem& FrameOutput)
{
	pfdm_xr_camera_frame_item_t OutRawFrameOutput;
	pfdm_xr_camera_error_t result = FYvrInterfaceWrapper::AcquireTrackingCameraFrame((pfdm_xr_camera_type_t)Type, &OutRawFrameOutput);
	if (result != pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR)
	{
		return false;
	}

	FrameOutput.FrameNumber = OutRawFrameOutput.frame_number;
	FrameOutput.Width = OutRawFrameOutput.width;
	FrameOutput.Height = OutRawFrameOutput.height;
	FrameOutput.Format = OutRawFrameOutput.format;
	FrameOutput.ExposureDuration = OutRawFrameOutput.exposure_duration;
	FrameOutput.SoeTimestamp = OutRawFrameOutput.soe_timestamp;
	FrameOutput.SoeTimestampQ = OutRawFrameOutput.soe_timestamp_q;
	FrameOutput.Gain = OutRawFrameOutput.gain;
	FrameOutput.DataSize = OutRawFrameOutput.data_size;
	if (OutRawFrameOutput.data[0] != nullptr)
	{
		FrameOutput.LeftEyeData.SetNum(FrameOutput.DataSize);
		FMemory::Memcpy(FrameOutput.LeftEyeData.GetData(), (void*)OutRawFrameOutput.data[0], sizeof(uint8) * FrameOutput.DataSize);
	}

	if (OutRawFrameOutput.data[1] != nullptr)
	{
		FrameOutput.RightEyeData.SetNum(FrameOutput.DataSize);
		FMemory::Memcpy(FrameOutput.RightEyeData.GetData(), (void*)OutRawFrameOutput.data[1], sizeof(uint8) * FrameOutput.DataSize);
	}
	return true;
}

bool UYvrBusinessBPLibrary::SetTrackingCameraFps(ECameraType Type, int32 Frequency)
{
	return FYvrInterfaceWrapper::SetTrackingCameraFps((pfdm_xr_camera_type_t)Type, Frequency) == pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR;
}

bool UYvrBusinessBPLibrary::GetTrackingCameraFps(ECameraType Type, int32& Frequency)
{
    int32_t OutRawFrequency;
	pfdm_xr_camera_error_t result = FYvrInterfaceWrapper::GetTrackingCameraFps((pfdm_xr_camera_type_t)Type, &OutRawFrequency);
	if (result != pfdm_xr_camera_error_t::PFDM_XR_CAM_ERROR_NO_ERROR)
	{
		return false;
	}

	Frequency = (int32)OutRawFrequency;
	return true;
}

void UYvrBusinessBPLibrary::UpdateRenderTargetFromRGB(const TArray<uint8>& RawData, int32 Width, int32 Height, UTextureRenderTarget2D* RenderTarget2D, uint8 OverrideAlpha)
{
	if (!RenderTarget2D || Width * Height * 3 != RawData.Num())
	{
		return;
	}

	EPixelFormat Format = RenderTarget2D->GetFormat();
	if (RenderTarget2D->SizeX != Width || RenderTarget2D->SizeY != Height || Format != EPixelFormat::PF_B8G8R8A8)
	{
		RenderTarget2D->InitCustomFormat(Width, Height, EPixelFormat::PF_B8G8R8A8, false);
	}

	int32 PixelNum = Width * Height;
	TArray<uint8> TargetTextureData;
	TargetTextureData.SetNum(PixelNum * 4);
	for (int32 Index = 0; Index < PixelNum; ++Index)
	{
		int32 TargetIndex = Index * 4;
		int32 SourceIndex = Index * 3;
		TargetTextureData[TargetIndex + 0] = RawData[SourceIndex + 0];
		TargetTextureData[TargetIndex + 1] = RawData[SourceIndex + 1];
		TargetTextureData[TargetIndex + 2] = RawData[SourceIndex + 2];
		TargetTextureData[TargetIndex + 3] = OverrideAlpha;
	}

	FRenderTarget* RenderTarget = RenderTarget2D->GameThread_GetRenderTargetResource();
	FUpdateTextureRegion2D Region = FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);
	ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
		[=](FRHICommandListImmediate& RHICmdList)
		{
			FTexture2DRHIRef TextureRHI = RenderTarget->GetRenderTargetTexture();
			check(TextureRHI.IsValid());
			RHIUpdateTexture2D(
				TextureRHI,
				0,
				Region,
				Width * 4,
				TargetTextureData.GetData()
			);
		}
	);
}

void UYvrBusinessBPLibrary::UpdateRenderTargetFromYUVNV21(const TArray<uint8>& RawData, int32 Width, int32 Height, UTextureRenderTarget2D* RenderTarget2D, uint8 OverrideAlpha)
{
	if (!RenderTarget2D || Width * Height * 3 / 2 != RawData.Num())
	{
		return;
	}

	EPixelFormat Format = RenderTarget2D->GetFormat();
	if (RenderTarget2D->SizeX != Width || RenderTarget2D->SizeY != Height || Format != EPixelFormat::PF_B8G8R8A8)
	{
		RenderTarget2D->InitCustomFormat(Width, Height, EPixelFormat::PF_B8G8R8A8, false);
	}

	int32 PixelNum = Width * Height;
	TArray<uint8> TargetTextureData;
	TargetTextureData.SetNum(PixelNum * 4);

	int32 Index = 0;
	for (int32 i = 0; i < Height; ++i) {
		for (int32 j = 0; j < Width; ++j) {
			int32 y = (0xff & ((int32)RawData[i * Width + j]));
			int32 v = (0xff & ((int32)RawData[PixelNum + (i >> 1) * Width + (j & ~1) + 0]));
			int32 u = (0xff & ((int32)RawData[PixelNum + (i >> 1) * Width + (j & ~1) + 1]));
			y = y < 16 ? 16 : y;

			int32 r = (int)(1.164f * (y - 16) + 1.596f * (v - 128));
			int32 g = (int)(1.164f * (y - 16) - 0.813f * (v - 128) - 0.391f * (u - 128));
			int32 b = (int)(1.164f * (y - 16) + 2.018f * (u - 128));

			r = r < 0 ? 0 : (r > 255 ? 255 : r);
			g = g < 0 ? 0 : (g > 255 ? 255 : g);
			b = b < 0 ? 0 : (b > 255 ? 255 : b);

			TargetTextureData[Index++] = b;
			TargetTextureData[Index++] = g;
			TargetTextureData[Index++] = r;
			TargetTextureData[Index++] = OverrideAlpha;
		}
	}

	FUpdateTextureRegion2D Region = FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);
	ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
		[=](FRHICommandListImmediate& RHICmdList)
		{
			FRenderTarget* RenderTarget = RenderTarget2D->GetRenderTargetResource();
			FTexture2DRHIRef TextureRHI = RenderTarget->GetRenderTargetTexture();
			check(TextureRHI.IsValid());
			RHIUpdateTexture2D(
				TextureRHI,
				0,
				Region,
				Width * 4,
				TargetTextureData.GetData()
			);
		}
		);
}

void UYvrBusinessBPLibrary::UpdateRenderTargetFromUVNVY8(const TArray<uint8> &RawData, int32 Width, int32 Height, UTextureRenderTarget2D *RenderTarget2D, uint8 OverrideAlpha)
{
    if (!RenderTarget2D || Width * Height != RawData.Num())
    {
        return;
    }

    EPixelFormat Format = RenderTarget2D->GetFormat();
    if (RenderTarget2D->SizeX != Width || RenderTarget2D->SizeY != Height || Format != EPixelFormat::PF_B8G8R8A8)
    {
        RenderTarget2D->InitCustomFormat(Width, Height, EPixelFormat::PF_B8G8R8A8, false);
    }

    TArray<uint8> TargetTextureData;
    TargetTextureData.SetNum(Width * Height * 4);

    for (int32 i = 0; i < Height; ++i)
    {
        for (int32 j = 0; j < Width; ++j)
        {
            int32 Index = (i * Width + j) * 4;
            uint8 Y = RawData[i * Width + j];

            TargetTextureData[Index + 0] = Y;     // B
            TargetTextureData[Index + 1] = Y;     // G
            TargetTextureData[Index + 2] = Y;     // R
            TargetTextureData[Index + 3] = OverrideAlpha; // A
        }
    }
	
    FUpdateTextureRegion2D Region(0, 0, 0, 0, Width, Height);
    ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
		[=, TextureData = MoveTemp(TargetTextureData)](FRHICommandListImmediate& RHICmdList)
        {
			FRenderTarget* RenderTarget = RenderTarget2D->GetRenderTargetResource();
            FTexture2DRHIRef TextureRHI = RenderTarget->GetRenderTargetTexture();
            if (TextureRHI.IsValid())
            {
                RHIUpdateTexture2D(
                    TextureRHI,
                    0,
                    Region,
                    Width * 4,
                    TextureData.GetData()
                );
            }
        }
    );
}

