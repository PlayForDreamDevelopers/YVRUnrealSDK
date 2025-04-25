#include "YvrInterfaceWrapper.h"
#include "YvrCameraComponent.h"

#if PLATFORM_ANDROID
#include <dlfcn.h>
#endif

pfdm_xr_set_vst_camera_frequency* FYvrInterfaceWrapper::Yvr_set_vst_camera_frequency_Interface;
pfdm_xr_get_vst_camera_frequency* FYvrInterfaceWrapper::Yvr_get_vst_camera_frequency_Interface;
pfdm_xr_set_vst_camera_resolution* FYvrInterfaceWrapper::Yvr_set_vst_camera_resolution_Interface;
pfdm_xr_get_vst_camera_resolution* FYvrInterfaceWrapper::Yvr_get_vst_camera_resolution_Interface;
pfdm_xr_set_vst_camera_format* FYvrInterfaceWrapper::Yvr_set_vst_camera_format_Interface;
pfdm_xr_get_vst_camera_format* FYvrInterfaceWrapper::Yvr_get_vst_camera_format_Interface;
pfdm_xr_set_vst_camera_output_source* FYvrInterfaceWrapper::Yvr_set_vst_camera_output_source_Interface;
pfdm_xr_get_vst_camera_output_source* FYvrInterfaceWrapper::Yvr_get_vst_camera_output_source_Interface;
pfdm_xr_get_vst_camera_intrinsic_extrinsic* FYvrInterfaceWrapper::Yvr_get_vst_camera_intrinsic_extrinsic_Interface;
pfdm_xr_open_vst_camera* FYvrInterfaceWrapper::Yvr_open_vst_camera_Interface;
pfdm_xr_close_vst_camera* FYvrInterfaceWrapper::Yvr_close_vst_camera_Interface;
pfdm_xr_acquire_vst_camera_frame* FYvrInterfaceWrapper::Yvr_acquire_vst_camera_frame_Interface;

pfdm_xr_open_tracking_camera* FYvrInterfaceWrapper::Yvr_open_tracking_camera_Interface;
pfdm_xr_close_tracking_camera* FYvrInterfaceWrapper::Yvr_close_tracking_camera_Interface;
pfdm_xr_subscribe_frame* FYvrInterfaceWrapper::Yvr_subscribe_frame_Interface;
pfdm_xr_unsubscribe_frame* FYvrInterfaceWrapper::Yvr_unsubscribe_frame_Interface;
pfdm_xr_acquire_tracking_camera_frame* FYvrInterfaceWrapper::Yvr_acquire_tracking_camera_frame_Interface;
pfdm_xr_set_tracking_camera_fps* FYvrInterfaceWrapper::Yvr_set_tracking_camera_fps_Interface;
pfdm_xr_get_tracking_camera_fps* FYvrInterfaceWrapper::Yvr_get_tracking_camera_fps_Interface;

void* LoadEntryPoint(void* Handle, const char* EntryPointName)
{
#if PLATFORM_ANDROID
	void* ptr = dlsym(Handle, EntryPointName);
	if (ptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Load libpfdm_xr_native_sdk.so [%s] Success"), ANSI_TO_TCHAR(EntryPointName));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Load libpfdm_xr_native_sdk.so [%s] Failed"), ANSI_TO_TCHAR(EntryPointName));
	}
	return ptr;
#endif
	UE_LOG(LogTemp, Display, TEXT("Load libpfdm_xr_native_sdk.so [%s] Failed"), ANSI_TO_TCHAR(EntryPointName));
	return nullptr;
}

bool FYvrInterfaceWrapper::Init()
{
	void* Handle = nullptr;
#if PLATFORM_ANDROID
	Handle = FPlatformProcess::GetDllHandle(TEXT("libpfdm_xr_native_sdk.so"));
#endif
	if (Handle == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("GetDllHandle Failed"));
		return false;
	}

	Yvr_set_vst_camera_frequency_Interface = reinterpret_cast<pfdm_xr_set_vst_camera_frequency*>(LoadEntryPoint(Handle, "pfdm_xr_set_vst_camera_frequency"));
	Yvr_get_vst_camera_frequency_Interface = reinterpret_cast<pfdm_xr_get_vst_camera_frequency*>(LoadEntryPoint(Handle, "pfdm_xr_get_vst_camera_frequency"));
	Yvr_set_vst_camera_resolution_Interface = reinterpret_cast<pfdm_xr_set_vst_camera_resolution*>(LoadEntryPoint(Handle, "pfdm_xr_set_vst_camera_resolution"));
	Yvr_get_vst_camera_resolution_Interface = reinterpret_cast<pfdm_xr_get_vst_camera_resolution*>(LoadEntryPoint(Handle, "pfdm_xr_get_vst_camera_resolution"));
	Yvr_set_vst_camera_format_Interface = reinterpret_cast<pfdm_xr_set_vst_camera_format*>(LoadEntryPoint(Handle, "pfdm_xr_set_vst_camera_format"));
	Yvr_get_vst_camera_format_Interface = reinterpret_cast<pfdm_xr_get_vst_camera_format*>(LoadEntryPoint(Handle, "pfdm_xr_get_vst_camera_format"));
	Yvr_set_vst_camera_output_source_Interface = reinterpret_cast<pfdm_xr_set_vst_camera_output_source*>(LoadEntryPoint(Handle, "pfdm_xr_set_vst_camera_output_source"));
	Yvr_get_vst_camera_output_source_Interface = reinterpret_cast<pfdm_xr_get_vst_camera_output_source*>(LoadEntryPoint(Handle, "pfdm_xr_get_vst_camera_output_source"));
	Yvr_get_vst_camera_intrinsic_extrinsic_Interface = reinterpret_cast<pfdm_xr_get_vst_camera_intrinsic_extrinsic*>(LoadEntryPoint(Handle, "pfdm_xr_get_vst_camera_intrinsic_extrinsic"));
	Yvr_open_vst_camera_Interface = reinterpret_cast<pfdm_xr_open_vst_camera*>(LoadEntryPoint(Handle, "pfdm_xr_open_vst_camera"));
	Yvr_close_vst_camera_Interface = reinterpret_cast<pfdm_xr_close_vst_camera*>(LoadEntryPoint(Handle, "pfdm_xr_close_vst_camera"));
	Yvr_acquire_vst_camera_frame_Interface = reinterpret_cast<pfdm_xr_acquire_vst_camera_frame*>(LoadEntryPoint(Handle, "pfdm_xr_acquire_vst_camera_frame"));

	Yvr_open_tracking_camera_Interface = reinterpret_cast<pfdm_xr_open_tracking_camera*>(LoadEntryPoint(Handle, "pfdm_xr_open_tracking_camera"));
	Yvr_close_tracking_camera_Interface = reinterpret_cast<pfdm_xr_close_tracking_camera*>(LoadEntryPoint(Handle, "pfdm_xr_close_tracking_camera"));
	Yvr_subscribe_frame_Interface = reinterpret_cast<pfdm_xr_subscribe_frame*>(LoadEntryPoint(Handle, "pfdm_xr_subscribe_frame"));
	Yvr_unsubscribe_frame_Interface = reinterpret_cast<pfdm_xr_unsubscribe_frame*>(LoadEntryPoint(Handle, "pfdm_xr_unsubscribe_frame"));
	Yvr_acquire_tracking_camera_frame_Interface = reinterpret_cast<pfdm_xr_acquire_tracking_camera_frame*>(LoadEntryPoint(Handle, "pfdm_xr_acquire_tracking_camera_frame"));
	Yvr_set_tracking_camera_fps_Interface = reinterpret_cast<pfdm_xr_set_tracking_camera_fps*>(LoadEntryPoint(Handle, "pfdm_xr_set_tracking_camera_fps"));
	Yvr_get_tracking_camera_fps_Interface = reinterpret_cast<pfdm_xr_get_tracking_camera_fps*>(LoadEntryPoint(Handle, "pfdm_xr_get_tracking_camera_fps"));

	UE_LOG(LogTemp, Display, TEXT("FYvrInterfaceWrapper Init Success"));

	return true;
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::OpenVSTCamera()
{
	return Yvr_open_vst_camera_Interface ? Yvr_open_vst_camera_Interface() : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::CloseVSTCamera()
{
	return Yvr_close_vst_camera_Interface ? Yvr_close_vst_camera_Interface() : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::AcquireVSTCameraFrame(pfdm_xr_vst_camera_frame_item_ext_t* out_frame)
{
	return Yvr_acquire_vst_camera_frame_Interface ? Yvr_acquire_vst_camera_frame_Interface(out_frame) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SetVSTCameraFrequency(pfdm_xr_vst_camera_frequency_cfg_t freq)
{
	return Yvr_set_vst_camera_frequency_Interface ? Yvr_set_vst_camera_frequency_Interface(freq) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraFrequency(pfdm_xr_vst_camera_frequency_cfg_t* freq)
{
	return Yvr_get_vst_camera_frequency_Interface ? Yvr_get_vst_camera_frequency_Interface(freq) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SetVSTCameraResolution(pfdm_xr_vst_camera_resolution_cfg_t resolution)
{
	return Yvr_set_vst_camera_resolution_Interface ? Yvr_set_vst_camera_resolution_Interface(resolution) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraResolution(pfdm_xr_vst_camera_resolution_cfg_t* resolution)
{
	return Yvr_get_vst_camera_resolution_Interface ? Yvr_get_vst_camera_resolution_Interface(resolution) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SetVSTCameraFormat(pfdm_xr_vst_camera_format_cfg_t fmt)
{
	return Yvr_set_vst_camera_format_Interface ? Yvr_set_vst_camera_format_Interface(fmt) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraFormat(pfdm_xr_vst_camera_format_cfg_t* fmt)
{
	return Yvr_get_vst_camera_format_Interface ? Yvr_get_vst_camera_format_Interface(fmt) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SetVSTCameraOutputSource(pfdm_xr_vst_camera_source_cfg_t source)
{
	return Yvr_set_vst_camera_output_source_Interface ? Yvr_set_vst_camera_output_source_Interface(source) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraOutputSource(pfdm_xr_vst_camera_source_cfg_t* source)
{
	return Yvr_get_vst_camera_output_source_Interface ? Yvr_get_vst_camera_output_source_Interface(source) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraIntrinsicExtrinsic(pfdm_xr_vst_camera_id_t id, pfdm_xr_vst_camera_intrinsic_extrinsic_t* params)
{
	return Yvr_get_vst_camera_intrinsic_extrinsic_Interface ? Yvr_get_vst_camera_intrinsic_extrinsic_Interface(id, params) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::OpenTrackingCamera(pfdm_xr_camera_type_t type)
{
	return Yvr_open_tracking_camera_Interface ? Yvr_open_tracking_camera_Interface(type) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::CloseTrackingCamera(pfdm_xr_camera_type_t type)
{
	return Yvr_close_tracking_camera_Interface ? Yvr_close_tracking_camera_Interface(type) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SubscribeFrame(pfdm_xr_camera_type_t type)
{
	if(Yvr_subscribe_frame_Interface){
		pfdm_xr_camera_error_t error = 
		Yvr_subscribe_frame_Interface
		(type, 
			[](pfdm_xr_camera_frame_item_t* frame_output, void* user_data) {
				FVSTCameraFrameItem CameraFrameItem;
				CameraFrameItem.FrameNumber = frame_output->frame_number;
				CameraFrameItem.Width = frame_output->width;
				CameraFrameItem.Height = frame_output->height;
				CameraFrameItem.Format = frame_output->format;
				CameraFrameItem.ExposureDuration = frame_output->exposure_duration;
				CameraFrameItem.SoeTimestamp = frame_output->soe_timestamp;
				CameraFrameItem.SoeTimestampQ = frame_output->soe_timestamp_q;
				CameraFrameItem.Gain = frame_output->gain;
				CameraFrameItem.DataSize = frame_output->data_size;
				if (frame_output->data[0] != nullptr)
				{
					CameraFrameItem.LeftEyeData.SetNum(CameraFrameItem.DataSize);
					FMemory::Memcpy(CameraFrameItem.LeftEyeData.GetData(), (void*)frame_output->data[0], sizeof(uint8) * CameraFrameItem.DataSize);
				}
			
				if (frame_output->data[1] != nullptr)
				{
					CameraFrameItem.RightEyeData.SetNum(CameraFrameItem.DataSize);
					FMemory::Memcpy(CameraFrameItem.RightEyeData.GetData(), (void*)frame_output->data[1], sizeof(uint8) * CameraFrameItem.DataSize);
				}

				//UObject* myClassInstance = static_cast<UObject*>(user_data);

				UYvrBusinessDelegates::CameraFrameDelegate.Broadcast(CameraFrameItem);
			}, 
		nullptr);
		return error;
	}
	else {
		return pfdm_xr_camera_error_t();
	}
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::UnSubscribeFrame(pfdm_xr_camera_type_t type)
{
	return Yvr_unsubscribe_frame_Interface ? Yvr_unsubscribe_frame_Interface(type) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::AcquireTrackingCameraFrame(pfdm_xr_camera_type_t type, pfdm_xr_camera_frame_item_t *frame_output)
{
	return Yvr_acquire_tracking_camera_frame_Interface ? Yvr_acquire_tracking_camera_frame_Interface(type, frame_output) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SetTrackingCameraFps(pfdm_xr_camera_type_t type, int32_t frequency)
{
	return Yvr_set_tracking_camera_fps_Interface ? Yvr_set_tracking_camera_fps_Interface(type, frequency) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetTrackingCameraFps(pfdm_xr_camera_type_t type, int32_t *frequency)
{
	return Yvr_get_tracking_camera_fps_Interface ? Yvr_get_tracking_camera_fps_Interface(type, frequency) : pfdm_xr_camera_error_t();
}
