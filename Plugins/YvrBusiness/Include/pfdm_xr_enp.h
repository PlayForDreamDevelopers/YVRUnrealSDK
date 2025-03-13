// Copyright® 2020-2024 PFDM Technology Co., Ltd. All rights reserved.

#ifndef PFDM_XR_ENP_H
#define PFDM_XR_ENP_H

#include <stdint.h>
#include <string>

/**
 * @brief pose data
 *
 */
typedef struct pfdm_xr_pose_t {
    int64_t timestamp;        //!< nanoseconds
    double x, y, z;           //!< position
    double rw, rx, ry, rz;    //!< rotation
    uint8_t confidence;       //!< 1:good 0:bad
    uint8_t warning_flag;     //!< warning flag
    double vx, vy, vz;        //!< linear velocity
    double ax, ay, az;        //!< linear acceleration
    double wx, wy, wz;        //!< angular velocity
    double w_ax, w_ay, w_az;  //!< angular acceleration
    uint8_t reserved[48];
} pfdm_xr_pose_t;

/**
 * @brief eye number
 *
 */
typedef enum pfdm_xr_eye_number_t {
    PFDM_XR_LEFT_EYE,
    PFDM_XR_RIGHT_EYE,
    PFDM_XR_EYE_NUMBER,
} pfdm_xr_eye_number_t;

/**
 * @brief enp native sdk interface error code
 *
 */
typedef enum pfdm_xr_enp_error_t {
    PFDM_XR_ENP_ERROR_NO_ERROR,
    PFDM_XR_ENP_ERROR_INTERNAL,
    PFDM_XR_ENP_ERROR_NOT_SUPPORTED,
    PFDM_XR_ENP_ERROR_TIMED_OUT,
    PFDM_XR_ENP_ERROR_ALLOCATION_FAILED,
    PFDM_XR_ENP_ERROR_INVALID_PARAMETER,
    PFDM_XR_ENP_ERROR_CONFIGURATION_PREMATURE,
    PFDM_XR_ENP_ERROR_INVALID_CONFIGURATION,
    PFDM_XR_ENP_ERROR_OPERATION_FAILED,
    PFDM_XR_ENP_ERROR_NOT_AVAILABLE,
    PFDM_XR_ENP_ERROR_ALREADY_STARTED,
    PFDM_XR_ENP_ERROR_NOT_STARTED
} pfdm_xr_enp_error_t;

/**
 * @brief enp native sdk version
 *
 */
typedef struct pfdm_xr_version_t {
    int major;
    int minor;
    int patch;
} pfdm_xr_version_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get enp native sdk version
 *
 * @param version
 * @return PFDM_XR_ENP_API
 */
PFDM_XR_ENP_API pfdm_xr_enp_error_t PFDM_XR_ENP_CALL pfdm_xr_get_api_version(pfdm_xr_version_t* version);

#ifdef __cplusplus
}
#endif

#endif  // PFDM_XR_ENP_H
