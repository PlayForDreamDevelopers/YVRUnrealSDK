#pragma once

#define PFDM_XR_ENP_API typedef
#define PFDM_XR_ENP_CALL
#include "pfdm_xr_enp.h"
#include "pfdm_xr_camera_types.h"
#include "pfdm_xr_camera_interface.h"
#undef PFDM_XR_ENP_API

class FYvrInterfaceWrapper
{
private:
	FYvrInterfaceWrapper()
	{
	}

	static setVSTCameraFrequency* Yvr_setVSTCameraFrequency_Interface;
	static getVSTCameraFrequency* Yvr_getVSTCameraFrequency_Interface;
	static setVSTCameraResolution* Yvr_setVSTCameraResolution_Interface;
	static getVSTCameraResolution* Yvr_getVSTCameraResolution_Interface;
	static setVSTCameraFormat* Yvr_setVSTCameraFormat_Interface;
	static getVSTCameraFormat* Yvr_getVSTCameraFormat_Interface;
	static setVSTCameraOutputSource* Yvr_setVSTCameraOutputSource_Interface;
	static getVSTCameraOutputSource* Yvr_getVSTCameraOutputSource_Interface;
	static getVSTCameraIntrinsicExtrinsic* Yvr_getVSTCameraIntrinsicExtrinsic_Interface;
	static openVSTCamera* Yvr_openVSTCamera_Interface;
	static closeVSTCamera* Yvr_closeVSTCamera_Interface;
	static acquireVSTCameraFrame* Yvr_acquireVSTCameraFrame_Interface;

public:
	static bool Init();
public:
	static pfdm_xr_camera_error_t OpenVSTCamera();
	static pfdm_xr_camera_error_t CloseVSTCamera();
	static pfdm_xr_camera_error_t AcquireVSTCameraFrame(vst_camera_frame_item_ext_t* out_frame);

	static pfdm_xr_camera_error_t SetVSTCameraFrequency(vst_camera_frequency_cfg_t freq);
	static pfdm_xr_camera_error_t GetVSTCameraFrequency(vst_camera_frequency_cfg_t* freq);
	static pfdm_xr_camera_error_t SetVSTCameraResolution(vst_camera_resolution_cfg_t resolution);
	static pfdm_xr_camera_error_t GetVSTCameraResolution(vst_camera_resolution_cfg_t* resolution);
	static pfdm_xr_camera_error_t SetVSTCameraFormat(vst_camera_format_cfg_t fmt);
	static pfdm_xr_camera_error_t GetVSTCameraFormat(vst_camera_format_cfg_t* fmt);
	static pfdm_xr_camera_error_t SetVSTCameraOutputSource(vst_camera_source_cfg_t source);
	static pfdm_xr_camera_error_t GetVSTCameraOutputSource(vst_camera_source_cfg_t* source);
	static pfdm_xr_camera_error_t GetVSTCameraIntrinsicExtrinsic(vst_camera_id_t id, vst_camera_intrinsic_extrinsic_t* params);
};