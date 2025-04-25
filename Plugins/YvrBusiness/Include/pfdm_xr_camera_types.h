// Copyright® 2020-2025 PFDM Technology Co., Ltd. All rights reserved.

#ifndef PFDM_XR_CAMERA_TYPES_H
#define PFDM_XR_CAMERA_TYPES_H

#include "pfdm_xr_ent.h"

/**
 * @brief pfdm xr camera api error code
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
 * @brief pfdm xr camera frame item
 */
typedef struct pfdm_xr_camera_frame_item_t {
    uint32_t frame_number;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;
    uint32_t exposure_duration;  // nanosecond
    int64_t soe_timestamp;       // start of exposure time (nanosecond CLOCK_BOOT_TIME)
    int64_t soe_timestamp_q;     // start of exposure time (nanosecond with qtimer)
    uint32_t gain;
    uint32_t data_size;
    volatile uint8_t* data[PFDM_XR_EYE_NUMBER];
    uint8_t reserved[64];
} pfdm_xr_camera_frame_item_t;

#endif  // PFDM_XR_CAMERA_TYPES_H
