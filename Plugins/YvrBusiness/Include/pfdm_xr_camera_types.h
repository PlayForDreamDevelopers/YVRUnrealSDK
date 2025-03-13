// Copyright® 2020-2024 PFDM Technology Co., Ltd. All rights reserved.

#ifndef PFDM_XR_CAMERA_TYPES_H
#define PFDM_XR_CAMERA_TYPES_H

#include "pfdm_xr_enp.h"

/**
 * @brief camera interface error code
 *
 */
typedef enum pfdm_xr_camera_error_t {
    PFDM_XR_CAM_ERROR_NO_ERROR,
    PFDM_XR_CAM_ERROR_INTERNAL,
    PFDM_XR_CAM_ERROR_NOT_SUPPORTED,
    PFDM_XR_CAM_ERROR_TIMED_OUT,
    PFDM_XR_CAM_ERROR_ALLOCATION_FAILED,
    PFDM_XR_CAM_ERROR_INVALID_PARAMETER,
    PFDM_XR_CAM_ERROR_CONFIGURATION_PREMATURE,
    PFDM_XR_CAM_ERROR_INVALID_CONFIGURATION,
    PFDM_XR_CAM_ERROR_OPERATION_FAILED,
    PFDM_XR_CAM_ERROR_NOT_AVAILABLE,
    PFDM_XR_CAM_ERROR_ALREADY_STARTED,
    PFDM_XR_CAM_ERROR_NOT_STARTED
} pfdm_xr_camera_error_t;

/**
 * @brief vst camera resolution configuration
 * recommended to use lower resolutions whenever possible
 * for performance considerations
 */
typedef enum vst_camera_resolution_cfg_t {
    VST_RESOLUTION_660_616,
    VST_RESOLUTION_1320_1232,
    VST_RESOLUTION_2640_2464,
} vst_camera_resolution_cfg_t;

/**
 * @brief vst camera frequency configuration
 * when the screen refresh rate is 90Hz, the supported frequencies are 30Hz and 10Hz
 * when the screen refresh rate is 72Hz, the supported frequencies are 24Hz and 8Hz
 *
 * recommended to use lower frequencies whenever possible
 * for performance considerations
 */
typedef enum vst_camera_frequency_cfg_t {
    VST_FREQUENCY_8_HZ,
    VST_FREQUENCY_10_HZ,
    VST_FREQUENCY_24_HZ,
    VST_FREQUENCY_30_HZ,
} vst_camera_frequency_cfg_t;

/**
 * @brief vst camera format configuration
 *
 */
typedef enum vst_camera_format_cfg_t {
    VST_CAMERA_FMT_NV21,
} vst_camera_format_cfg_t;

/**
 * @brief vst camera output source configuration
 * when set to VST_CAMERA_BOTH_EYES, the data pointer in camera_frame_item_t will store both eyes frame data
 * otherwise, it will only store the corresponding frame images based on the configuration
 */
typedef enum vst_camera_source_cfg_t {
    VST_CAMERA_LEFT_EYE,
    VST_CAMERA_RIGHT_EYE,
    VST_CAMERA_BOTH_EYES,
} vst_camera_source_cfg_t;

typedef enum vst_camera_id_t {
    VST_CAMERA_ID_LEFT,
    VST_CAMERA_ID_RIGHT,
    VST_CAMERA_ID_MAX,
} vst_camera_id_t;

/**
 * @brief vst camera intrinsic extrinsic parameters
 *
 */
typedef struct vst_camera_intrinsic_extrinsic_t {
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
} vst_camera_intrinsic_extrinsic_t;

/**
 * @brief camera frame item
 *
 */
typedef struct camera_frame_item_t {
    uint32_t frame_number;                       // frame number
    uint32_t width;                              // width
    uint32_t height;                             // height
    uint32_t format;                             // format
    uint32_t exposure_duration;                  // exposure duration:ns
    int64_t soe_timestamp;                       // start of exposure time:ns (BOOTTIME)
    int64_t soe_timestamp_q;                     // nanoseconds in qtimer
    uint32_t gain;                               // gain
    uint32_t data_size;                          // datasize
    volatile uint8_t *data[PFDM_XR_EYE_NUMBER];  // data @see vst_camera_source_cfg_t
} camera_frame_item_t;

/**
 * @brief camera frame item extended
 *
 */
typedef struct vst_camera_frame_item_ext_t {
    camera_frame_item_t frame;
    pfdm_xr_pose_t six_dof_pose;  // six dof pose with start of exposure time
    uint8_t reserved[64];
} vst_camera_frame_item_ext_t;

#endif  // PFDM_XR_CAMERA_TYPES_H
