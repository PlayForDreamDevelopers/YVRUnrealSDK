// Copyright® 2020-2025 PFDM Technology Co., Ltd. All rights reserved.

#ifndef PFDM_XR_TRACKING_CAMERA_API_H
#define PFDM_XR_TRACKING_CAMERA_API_H

#include "pfdm_xr_camera_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief pfdm xr tracking camera type
 */
typedef enum pfdm_xr_camera_type_t {
    PFDM_XR_CAMERA_TYPE_TRACKING_MASTER,
    PFDM_XR_CAMERA_TYPE_TRACKING_SLAVE,
    PFDM_XR_CAMERA_TYPE_TRACKING_AUX,
    PFDM_XR_CAMERA_TYPE_EYE_TRACKING,
    PFDM_XR_CAMERA_TYPE_TOF,
    PFDM_XR_CAMERA_TYPE_MAX,
} pfdm_xr_camera_type_t;

/**
 * @brief pfdm xr tracking camera format configuration
 */
typedef enum pfdm_xr_tracking_camera_format_cfg_t {
    PFDM_XR_TRACKING_CAMERA_FMT_Y8,
    PFDM_XR_TRACKING_CAMERA_FMT_RAW8,
} pfdm_xr_tracking_camera_format_cfg_t;

/**
 * @brief xr tracking camera frame callback function
 */
typedef void (*pfdm_xr_frame_callback_t)(pfdm_xr_camera_frame_item_t* frame, void* user_data);

/**
 * @brief open tracking camera with camera type
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_open_tracking_camera(pfdm_xr_camera_type_t type);

/**
 * @brief close tracking camera with camera type
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_close_tracking_camera(pfdm_xr_camera_type_t type);

/**
 * @brief Subscribes to the camera stream based on the specified type. Utilizes a callback
 * method to provide frames to the subscriber.
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_subscribe_frame(pfdm_xr_camera_type_t type, pfdm_xr_frame_callback_t callback, void* user_data);

/**
 * @brief Unsubscribes from receiving frame updates from the camera
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_unsubscribe_frame(pfdm_xr_camera_type_t type);

/**
 * @brief Retrieves the current frame from the camera of the given type.
 * The call is non blocking and will return the latest frame available.
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_acquire_tracking_camera_frame(pfdm_xr_camera_type_t type, pfdm_xr_camera_frame_item_t* frame_output);

/**
 * @brief set tracking camera frame frequency with camera type, now only support eye tracking camera type with 90HZ and 120HZ
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_set_tracking_camera_fps(pfdm_xr_camera_type_t type, int32_t frequency);

/**
 * @brief get tracking camera frame frequency with camera type, should be called after pfdm_xr_open_tracking_camera
 */
PFDM_XR_ENT_API pfdm_xr_camera_error_t PFDM_XR_ENT_CALL pfdm_xr_get_tracking_camera_fps(pfdm_xr_camera_type_t type, int32_t* frequency);

#if defined(__cplusplus)
};  // extern "C"
#endif

#endif  // PFDM_XR_TRACKING_CAMERA_API_H
