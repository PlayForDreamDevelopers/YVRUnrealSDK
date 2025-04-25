#pragma once

#define PFDM_XR_ENT_API typedef
#define PFDM_XR_ENT_CALL
#include "pfdm_xr_vst_camera_api.h"
#include "pfdm_xr_tracking_camera_api.h"
#include "pfdm_xr_camera_types.h"
#include "pfdm_xr_ent.h"
#undef PFDM_XR_ENT_API

typedef void (*FrameCallback)(pfdm_xr_vst_camera_frame_item_ext_t* frame, void* user_data);

class FYvrInterfaceWrapper
{
private:
	FYvrInterfaceWrapper()
	{
	}

	static pfdm_xr_set_vst_camera_frequency* Yvr_set_vst_camera_frequency_Interface;
	static pfdm_xr_get_vst_camera_frequency* Yvr_get_vst_camera_frequency_Interface;
	static pfdm_xr_set_vst_camera_resolution* Yvr_set_vst_camera_resolution_Interface;
	static pfdm_xr_get_vst_camera_resolution* Yvr_get_vst_camera_resolution_Interface;
	static pfdm_xr_set_vst_camera_format* Yvr_set_vst_camera_format_Interface;
	static pfdm_xr_get_vst_camera_format* Yvr_get_vst_camera_format_Interface;
	static pfdm_xr_set_vst_camera_output_source* Yvr_set_vst_camera_output_source_Interface;
	static pfdm_xr_get_vst_camera_output_source* Yvr_get_vst_camera_output_source_Interface;
	static pfdm_xr_get_vst_camera_intrinsic_extrinsic* Yvr_get_vst_camera_intrinsic_extrinsic_Interface;
	static pfdm_xr_open_vst_camera* Yvr_open_vst_camera_Interface;
	static pfdm_xr_close_vst_camera* Yvr_close_vst_camera_Interface;
	static pfdm_xr_acquire_vst_camera_frame* Yvr_acquire_vst_camera_frame_Interface;

	static pfdm_xr_open_tracking_camera* Yvr_open_tracking_camera_Interface;
	static pfdm_xr_close_tracking_camera* Yvr_close_tracking_camera_Interface;
	static pfdm_xr_subscribe_frame* Yvr_subscribe_frame_Interface;
	static pfdm_xr_unsubscribe_frame* Yvr_unsubscribe_frame_Interface;
	static pfdm_xr_acquire_tracking_camera_frame* Yvr_acquire_tracking_camera_frame_Interface;
	static pfdm_xr_set_tracking_camera_fps* Yvr_set_tracking_camera_fps_Interface;
	static pfdm_xr_get_tracking_camera_fps* Yvr_get_tracking_camera_fps_Interface;

public:
	static bool Init();
public:
	static pfdm_xr_camera_error_t OpenVSTCamera();
	static pfdm_xr_camera_error_t CloseVSTCamera();
	static pfdm_xr_camera_error_t AcquireVSTCameraFrame(pfdm_xr_vst_camera_frame_item_ext_t* out_frame);

	static pfdm_xr_camera_error_t SetVSTCameraFrequency(pfdm_xr_vst_camera_frequency_cfg_t freq);
	static pfdm_xr_camera_error_t GetVSTCameraFrequency(pfdm_xr_vst_camera_frequency_cfg_t* freq);
	static pfdm_xr_camera_error_t SetVSTCameraResolution(pfdm_xr_vst_camera_resolution_cfg_t resolution);
	static pfdm_xr_camera_error_t GetVSTCameraResolution(pfdm_xr_vst_camera_resolution_cfg_t* resolution);
	static pfdm_xr_camera_error_t SetVSTCameraFormat(pfdm_xr_vst_camera_format_cfg_t fmt);
	static pfdm_xr_camera_error_t GetVSTCameraFormat(pfdm_xr_vst_camera_format_cfg_t* fmt);
	static pfdm_xr_camera_error_t SetVSTCameraOutputSource(pfdm_xr_vst_camera_source_cfg_t source);
	static pfdm_xr_camera_error_t GetVSTCameraOutputSource(pfdm_xr_vst_camera_source_cfg_t* source);
	static pfdm_xr_camera_error_t GetVSTCameraIntrinsicExtrinsic(pfdm_xr_vst_camera_id_t id, pfdm_xr_vst_camera_intrinsic_extrinsic_t* params);

	static pfdm_xr_camera_error_t OpenTrackingCamera (pfdm_xr_camera_type_t type);
	static pfdm_xr_camera_error_t CloseTrackingCamera (pfdm_xr_camera_type_t type);
	static pfdm_xr_camera_error_t SubscribeFrame (pfdm_xr_camera_type_t type);
	static pfdm_xr_camera_error_t UnSubscribeFrame (pfdm_xr_camera_type_t type);
	static pfdm_xr_camera_error_t AcquireTrackingCameraFrame (pfdm_xr_camera_type_t type, pfdm_xr_camera_frame_item_t* frame_output);
	static pfdm_xr_camera_error_t SetTrackingCameraFps (pfdm_xr_camera_type_t type, int32_t frequency);
	static pfdm_xr_camera_error_t GetTrackingCameraFps (pfdm_xr_camera_type_t type, int32_t* frequency);
};