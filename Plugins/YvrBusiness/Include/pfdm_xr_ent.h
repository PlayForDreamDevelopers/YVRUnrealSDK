// Copyright® 2020-2025 PFDM Technology Co., Ltd. All rights reserved.

#ifndef PFDM_XR_ENT_H
#define PFDM_XR_ENT_H

#include <stdint.h>
#include <string>

// #ifdef _WIN32
// #define PFDM_XR_ENT_CALL __cdecl
// #ifdef PFDM_XR_EXPORTING
// #define PFDM_XR_ENT_API __declspec(dllexport)
// #else
// #define PFDM_XR_ENT_API
// #endif /* PFDM_XR_EXPORTING */
// #elif __GNUC__ >= 4
// #define PFDM_XR_ENT_API __attribute__((visibility("default")))
// #define PFDM_XR_ENT_CALL
// #else
// #define PFDM_XR_ENT_API
// #define PFDM_XR_ENT_CALL
// #endif /* _WIN32 */

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

typedef enum pfdm_xr_eye_number_t {
    PFDM_XR_LEFT_EYE,
    PFDM_XR_RIGHT_EYE,
    PFDM_XR_EYE_NUMBER,
} pfdm_xr_eye_number_t;

/**
 * @brief pfdm xr ent api error code
 */
typedef enum pfdm_xr_ent_error_t { PFDM_XR_ENT_ERROR_NO_ERROR, PFDM_XR_ENT_ERROR_INTERNAL, PFDM_XR_ENT_ERROR_NOT_SUPPORTED } pfdm_xr_ent_error_t;

/**
 * @brief pfdm xr ent api version
 */
typedef struct pfdm_xr_ent_api_version_t {
    int32_t major;
    int32_t minor;
    int32_t patch;
} pfdm_xr_ent_api_version_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get pfdm xr ent api version
 */
PFDM_XR_ENT_API pfdm_xr_ent_error_t PFDM_XR_ENT_CALL pfdm_xr_get_ent_api_version(pfdm_xr_ent_api_version_t* version);

#ifdef __cplusplus
}
#endif

#endif  // PFDM_XR_ENT_H
