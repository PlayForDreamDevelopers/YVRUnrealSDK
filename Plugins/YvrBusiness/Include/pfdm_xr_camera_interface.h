// Copyright® 2015-2024 PFDM Technology Co., Ltd. All rights reserved.

#ifndef PFDM_XR_CAMERA_INTERFACE_H
#define PFDM_XR_CAMERA_INTERFACE_H

#include "pfdm_xr_camera_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief set vst camera frequency
 *
 * @param freq
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL setVSTCameraFrequency(vst_camera_frequency_cfg_t freq);

/**
 * @brief get vst camera frequency
 *
 * @param freq
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL getVSTCameraFrequency(vst_camera_frequency_cfg_t *freq);

/**
 * @brief set vst camera resolution
 *
 * @param resolution
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL setVSTCameraResolution(vst_camera_resolution_cfg_t resolution);

/**
 * @brief get vst camera resolution
 *
 * @param resolution
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL getVSTCameraResolution(vst_camera_resolution_cfg_t *resolution);

/**
 * @brief set vst camera format
 *
 * @param fmt
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL setVSTCameraFormat(vst_camera_format_cfg_t fmt);

/**
 * @brief get vst camera format
 *
 * @param fmt
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL getVSTCameraFormat(vst_camera_format_cfg_t *fmt);

/**
 * @brief set vst camera output source
 *
 * @param source
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL setVSTCameraOutputSource(vst_camera_source_cfg_t source);

/**
 * @brief get vst camera output source
 *
 * @param source
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL getVSTCameraOutputSource(vst_camera_source_cfg_t *source);

/**
 * @brief get vst camera intrinsic extrinsic parameters
 *
 * @param params
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL getVSTCameraIntrinsicExtrinsic(vst_camera_id_t id, vst_camera_intrinsic_extrinsic_t *params);

/**
 * @brief open vst camera
 *
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL openVSTCamera();

/**
 * @brief close vst camera
 *
 * @return PFDM_XR_ENP_API
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL closeVSTCamera();

/**
 * @brief acquire vst camera frame
 *
 * @param out_frame
 * @return pfdm_xr_camera_error_t
 */
PFDM_XR_ENP_API pfdm_xr_camera_error_t PFDM_XR_ENP_CALL acquireVSTCameraFrame(vst_camera_frame_item_ext_t *out_frame);

#if defined(__cplusplus)
};  // extern "C"
#endif

#endif  // PFDM_XR_CAMERA_INTERFACE_H
