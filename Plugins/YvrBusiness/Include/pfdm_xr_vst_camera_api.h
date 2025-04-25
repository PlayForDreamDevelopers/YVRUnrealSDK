// Copyright® 2020-2025 PFDM Technology Co., Ltd. All rights reserved.

#ifndef PFDM_XR_VST_CAMERA_API_H
#define PFDM_XR_VST_CAMERA_API_H

#include "pfdm_xr_camera_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief vst camera resolution configuration
 * recommended to use lower resolutions whenever possible
 * for performance considerations
 */
typedef enum pfdm_xr_vst_camera_resolution_cfg_t {
    PFDM_XR_VST_RESOLUTION_660_616,
    PFDM_XR_VST_RESOLUTION_1320_1232,
    PFDM_XR_VST_RESOLUTION_2640_2464,
} pfdm_xr_vst_camera_resolution_cfg_t;

/**
 * @brief vst camera frequency configuration
 * when the screen refresh rate is 90Hz, the supported frequencies are 30Hz and 10Hz
 * when the screen refresh rate is 72Hz, the supported frequencies are 24Hz and 8Hz
 *
 * recommended to use lower frequencies whenever possible
 * for performance considerations
 */
typedef enum pfdm_xr_vst_camera_frequency_cfg_t {
    PFDM_XR_VST_FREQUENCY_8_HZ,
    PFDM_XR_VST_FREQUENCY_10_HZ,
    PFDM_XR_VST_FREQUENCY_24_HZ,
    PFDM_XR_VST_FREQUENCY_30_HZ,
} pfdm_xr_vst_camera_frequency_cfg_t;

/**
 * @brief vst camera format configuration, now only NV21 is supported
 *
 */
typedef enum pfdm_xr_vst_camera_format_cfg_t {
    PFDM_XR_VST_CAMERA_FMT_NV21,
} pfdm_xr_vst_camera_format_cfg_t;

/**
 * @brief vst camera output source configuration
 * when set to VST_CAMERA_BOTH_EYES, the data pointer in camera_frame_item_t will store both eyes frame data
 * otherwise, it will only store the corresponding frame images based on the configuration
 */
typedef enum pfdm_xr_vst_camera_source_cfg_t {
    PFDM_XR_VST_CAMERA_LEFT_EYE,
    PFDM_XR_VST_CAMERA_RIGHT_EYE,
    PFDM_XR_VST_CAMERA_BOTH_EYES,
} pfdm_xr_vst_camera_source_cfg_t;

typedef enum pfdm_xr_vst_camera_id_t {
    PFDM_XR_VST_CAMERA_ID_LEFT,
    PFDM_XR_VST_CAMERA_ID_RIGHT,
    PFDM_XR_VST_CAMERA_ID_MAX,
} pfdm_xr_vst_camera_id_t;

/**
 * @brief vst camera intrinsic extrinsic parameters
 */
typedef struct pfdm_xr_vst_camera_intrinsic_extrinsic_t {
    float fx;  // intrinsic parameters
    float fy;  // intrinsic parameters
    float cx;  // intrinsic parameters
    float cy;  // intrinsic parameters

    double x;   // extrinsic parameters
    double y;   // extrinsic parameters
    double z;   // extrinsic parameters
    double rw;  // extrinsic parameters
    double rx;  // extrinsic parameters
    double ry;  // extrinsic parameters
    double rz;  // extrinsic parameters

    float distortion[16];   // distortion parameters
    uint8_t reserved[128];  // reserved
} pfdm_xr_vst_camera_intrinsic_extrinsic_t;

/**
 * @brief camera frame item extended
 */
typedef struct pfdm_xr_vst_camera_frame_item_ext_t {
    pfdm_xr_camera_frame_item_t frame;
    pfdm_xr_pose_t six_dof_pose;  // six dof pose with start of exposure time
    uint8_t reserved[64];
} pfdm_xr_vst_camera_frame_item_ext_t;

/**
 * @brief set vst camera frequency, see pfdm_xr_vst_camera_frequency_cfg_t
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_set_vst_camera_frequency(pfdm_xr_vst_camera_frequency_cfg_t frequency);

/**
 * @brief get vst camera frequency, see pfdm_xr_vst_camera_frequency_cfg_t
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_get_vst_camera_frequency(pfdm_xr_vst_camera_frequency_cfg_t *freqency_output);

/**
 * @brief set vst camera resolution, see pfdm_xr_vst_camera_resolution_cfg_t
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_set_vst_camera_resolution(pfdm_xr_vst_camera_resolution_cfg_t resolution);

/**
 * @brief get vst camera resolution, see pfdm_xr_vst_camera_resolution_cfg_t
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_get_vst_camera_resolution(pfdm_xr_vst_camera_resolution_cfg_t *resolution_output);

/**
 * @brief set vst camera format, see pfdm_xr_vst_camera_format_cfg_t
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_set_vst_camera_format(pfdm_xr_vst_camera_format_cfg_t format);

/**
 * @brief get vst camera format, see pfdm_xr_vst_camera_format_cfg_t
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_get_vst_camera_format(pfdm_xr_vst_camera_format_cfg_t *format_output);

/**
 * @brief set vst camera output source, see pfdm_xr_vst_camera_source_cfg_t
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_set_vst_camera_output_source(pfdm_xr_vst_camera_source_cfg_t source);

/**
 * @brief get vst camera output source, see pfdm_xr_vst_camera_source_cfg_t
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_get_vst_camera_output_source(pfdm_xr_vst_camera_source_cfg_t *source_output);

/**
 * @brief get vst camera intrinsic extrinsic parameters with camera id, see pfdm_xr_vst_camera_intrinsic_extrinsic_t
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_get_vst_camera_intrinsic_extrinsic(pfdm_xr_vst_camera_id_t id, pfdm_xr_vst_camera_intrinsic_extrinsic_t *params_output);

/**
 * @brief open vst camera
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_open_vst_camera();

/**
 * @brief close vst camera
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_close_vst_camera();

/**
 * @brief acquire vst camera frame
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_acquire_vst_camera_frame(pfdm_xr_vst_camera_frame_item_ext_t *frame_output);

#if defined(__cplusplus)
};  // extern "C"
#endif

#endif  // PFDM_XR_VST_CAMERA_API_H
