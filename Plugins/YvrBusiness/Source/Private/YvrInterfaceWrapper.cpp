#include "YvrInterfaceWrapper.h"

#if PLATFORM_ANDROID
#include <dlfcn.h>
#endif

setVSTCameraFrequency* FYvrInterfaceWrapper::Yvr_setVSTCameraFrequency_Interface;
getVSTCameraFrequency* FYvrInterfaceWrapper::Yvr_getVSTCameraFrequency_Interface;
setVSTCameraResolution* FYvrInterfaceWrapper::Yvr_setVSTCameraResolution_Interface;
getVSTCameraResolution* FYvrInterfaceWrapper::Yvr_getVSTCameraResolution_Interface;
setVSTCameraFormat* FYvrInterfaceWrapper::Yvr_setVSTCameraFormat_Interface;
getVSTCameraFormat* FYvrInterfaceWrapper::Yvr_getVSTCameraFormat_Interface;
setVSTCameraOutputSource* FYvrInterfaceWrapper::Yvr_setVSTCameraOutputSource_Interface;
getVSTCameraOutputSource* FYvrInterfaceWrapper::Yvr_getVSTCameraOutputSource_Interface;
getVSTCameraIntrinsicExtrinsic* FYvrInterfaceWrapper::Yvr_getVSTCameraIntrinsicExtrinsic_Interface;
openVSTCamera* FYvrInterfaceWrapper::Yvr_openVSTCamera_Interface;
closeVSTCamera* FYvrInterfaceWrapper::Yvr_closeVSTCamera_Interface;
acquireVSTCameraFrame* FYvrInterfaceWrapper::Yvr_acquireVSTCameraFrame_Interface;

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

	Yvr_setVSTCameraFrequency_Interface = reinterpret_cast<setVSTCameraFrequency*>(LoadEntryPoint(Handle, "setVSTCameraFrequency"));
	Yvr_getVSTCameraFrequency_Interface = reinterpret_cast<getVSTCameraFrequency*>(LoadEntryPoint(Handle, "getVSTCameraFrequency"));
	Yvr_setVSTCameraResolution_Interface = reinterpret_cast<setVSTCameraResolution*>(LoadEntryPoint(Handle, "setVSTCameraResolution"));
	Yvr_getVSTCameraResolution_Interface = reinterpret_cast<getVSTCameraResolution*>(LoadEntryPoint(Handle, "getVSTCameraResolution"));
	Yvr_setVSTCameraFormat_Interface = reinterpret_cast<setVSTCameraFormat*>(LoadEntryPoint(Handle, "setVSTCameraFormat"));
	Yvr_getVSTCameraFormat_Interface = reinterpret_cast<getVSTCameraFormat*>(LoadEntryPoint(Handle, "getVSTCameraFormat"));
	Yvr_setVSTCameraOutputSource_Interface = reinterpret_cast<setVSTCameraOutputSource*>(LoadEntryPoint(Handle, "setVSTCameraOutputSource"));
	Yvr_getVSTCameraOutputSource_Interface = reinterpret_cast<getVSTCameraOutputSource*>(LoadEntryPoint(Handle, "getVSTCameraOutputSource"));
	Yvr_getVSTCameraIntrinsicExtrinsic_Interface = reinterpret_cast<getVSTCameraIntrinsicExtrinsic*>(LoadEntryPoint(Handle, "getVSTCameraIntrinsicExtrinsic"));
	Yvr_openVSTCamera_Interface = reinterpret_cast<openVSTCamera*>(LoadEntryPoint(Handle, "openVSTCamera"));
	Yvr_closeVSTCamera_Interface = reinterpret_cast<closeVSTCamera*>(LoadEntryPoint(Handle, "closeVSTCamera"));
	Yvr_acquireVSTCameraFrame_Interface = reinterpret_cast<acquireVSTCameraFrame*>(LoadEntryPoint(Handle, "acquireVSTCameraFrame"));

	UE_LOG(LogTemp, Display, TEXT("FYvrInterfaceWrapper Init Success"));
	return true;
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::OpenVSTCamera()
{
	return Yvr_openVSTCamera_Interface ? Yvr_openVSTCamera_Interface() : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::CloseVSTCamera()
{
	return Yvr_closeVSTCamera_Interface ? Yvr_closeVSTCamera_Interface() : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::AcquireVSTCameraFrame(vst_camera_frame_item_ext_t* out_frame)
{
	return Yvr_acquireVSTCameraFrame_Interface ? Yvr_acquireVSTCameraFrame_Interface(out_frame) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SetVSTCameraFrequency(vst_camera_frequency_cfg_t freq)
{
	return Yvr_setVSTCameraFrequency_Interface ? Yvr_setVSTCameraFrequency_Interface(freq) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraFrequency(vst_camera_frequency_cfg_t* freq)
{
	return Yvr_getVSTCameraFrequency_Interface ? Yvr_getVSTCameraFrequency_Interface(freq) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SetVSTCameraResolution(vst_camera_resolution_cfg_t resolution)
{
	return Yvr_setVSTCameraResolution_Interface ? Yvr_setVSTCameraResolution_Interface(resolution) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraResolution(vst_camera_resolution_cfg_t* resolution)
{
	return Yvr_getVSTCameraResolution_Interface ? Yvr_getVSTCameraResolution_Interface(resolution) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SetVSTCameraFormat(vst_camera_format_cfg_t fmt)
{
	return Yvr_setVSTCameraFormat_Interface ? Yvr_setVSTCameraFormat_Interface(fmt) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraFormat(vst_camera_format_cfg_t* fmt)
{
	return Yvr_getVSTCameraFormat_Interface ? Yvr_getVSTCameraFormat_Interface(fmt) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::SetVSTCameraOutputSource(vst_camera_source_cfg_t source)
{
	return Yvr_setVSTCameraOutputSource_Interface ? Yvr_setVSTCameraOutputSource_Interface(source) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraOutputSource(vst_camera_source_cfg_t* source)
{
	return Yvr_getVSTCameraOutputSource_Interface ? Yvr_getVSTCameraOutputSource_Interface(source) : pfdm_xr_camera_error_t();
}

pfdm_xr_camera_error_t FYvrInterfaceWrapper::GetVSTCameraIntrinsicExtrinsic(vst_camera_id_t id, vst_camera_intrinsic_extrinsic_t* params)
{
	return Yvr_getVSTCameraIntrinsicExtrinsic_Interface ? Yvr_getVSTCameraIntrinsicExtrinsic_Interface(id, params) : pfdm_xr_camera_error_t();
}
