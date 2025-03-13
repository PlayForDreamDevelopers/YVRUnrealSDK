#ifndef YVRLIB_TYPES_H
#define YVRLIB_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <array>
#include "openxr.h"

#define YVR_MAX_RENDER_LAYERS 16


/// Return result
typedef signed int yvrResult;

/// Return successfully result
typedef enum yvrSuccessResult_
{
    yvrSuccess = 0,
} yvrSuccessResult;

/// Error code define
typedef enum yvrErrorResult_
{
    yvrError_unkown = -1000,
    yvrError_UnSupported = -1001,
    yvrError_InvalidParameter = -1002,
    yvrError_InvalidOperation = -1003,
    yvrError_InternalError = -1004,

    yvrResult_EnumSize = 0x7ffffff
} yvrErrorResult;

/// A 2D vector.
typedef struct yvrVector2f_
{
    float x, y;
} yvrVector2f;

/// A 3D vector.
typedef struct yvrVector3f_
{
    float x, y, z;
} yvrVector3f;

/// A 4D vector.
typedef struct yvrVector4f_
{
    float x, y, z, w;
} yvrVector4f;

typedef struct yvrVector4s_
{
    int16_t x, y, z, w;
} yvrVector4s;

/// Quaternion.
typedef struct yvrQuatf_
{
    float x, y, z, w;
} yvrQuatf;

/// Row-major 4x4 matrix.
typedef struct yvrMatrix4f_
{
    float M[4][4];
} yvrMatrix4f;

/// Row-major 1x16 matrix. -> yvrMatrix
typedef struct yvrMatrix16f_
{
    float M[16];
} yvrMatrix16f;

typedef struct yvrRect4f_
{
    float x1, y1, x2, y2;
} yvrRect4f;

/// Event type define
typedef enum yvrEventType_
{
    // No event
    YVRLIB_EVENT_NONE = 0,
    // Events were lost
    YVRLIB_EVENT_DATA_LOST = 1,
    // The application's frames are visible to the user.
    YVRLIB_EVENT_VISIBILITY_GAINED = 2,
    // The application's frames are no longer visible to the user.
    YVRLIB_EVENT_VISIBILITY_LOST = 3,
    // The current activity is in the foreground and has input focus.
    YVRLIB_EVENT_FOCUS_GAINED = 4,
    // The current activity is in the background (but possibly still visible) and has lost input
    // focus.
    YVRLIB_EVENT_FOCUS_LOST = 5,
    // The display refresh rate has changed
    YVRLIB_EVENT_DISPLAY_REFRESH_RATE_CHANGE = 6,
} yvrEventType;

typedef struct yvrEventHeader_
{
    yvrEventType eventType;
    uint64_t eventTimeStamp;
} yvrEventHeader;

typedef struct yvrEventDataLost_
{
    yvrEventHeader eventHeader;
} yvrEventDataLost;

typedef struct yvrEventVisibilityGained_
{
    yvrEventHeader eventHeader;
} yvrEventVisibilityGained;

typedef struct yvrEventVisibilityLost_
{
    yvrEventHeader eventHeader;
} yvrEventVisibilityLost;

typedef struct yvrEventFocusGained_
{
    yvrEventHeader eventHeader;
} yvrEventFocusGained;

typedef struct yvrEventFocusLost_
{
    yvrEventHeader eventHeader;
} yvrEventFocusLost;

typedef struct yvrEventDataBody_
{
    yvrEventHeader eventHeader;
    unsigned char eventData[40];
} yvrEventDataBody;

#define YVRLIB_LARGEST_EVENT_TYPE yvrEventDataBody

typedef enum yvrEventSize_
{
    YVRLIB_MAX_EVENT_SIZE = sizeof(YVRLIB_LARGEST_EVENT_TYPE)
} yvrEventSize;

typedef enum yvrDeviceNode_
{
    LeftController = 0,
    RightController,
    Hmd,
    LeftEye,
    RightEye,
    NodeCount
} yvrDeviceNode;

//! \brief Enum used to indicate which controller is being used
typedef enum yvrController_ : uint8_t
{
    kControllerLeft = 0,
    kControllerRight = 1,
    kNumControllers = 2
} yvrController;

struct ANativeWindow;

//! \brief Enum used to indicate which eye is being used
typedef enum yvrEye_
{
    kLeftEye = 0,
    kRightEye = 1,
    kNumEyes = 2,
} yvrEye;

//! \brief Enum used to indicate which eye to apply a render layer
typedef enum yvrEyeMask_
{
    kEyeMaskLeft = 0x00000001,
    kEyeMaskRight = 0x00000002,
    kEyeMaskBoth = 0x00000003
} yvrEyeMask;

//! \brief Enum used to indicate which colorspace is being used
typedef enum yvrColorSpace_
{
    kColorSpaceLinear = 0,
    kColorSpaceSRGB = 1,
    kNumColorSpaces
} yvrColorSpace;

/// Identifies a VR-related application thread.
typedef enum yvrPerfThreadType_
{
    YVRLIB_PERF_THREAD_TYPE_MAIN = 0,
    YVRLIB_PERF_THREAD_TYPE_RENDERER = 1,
} yvrPerfThreadType;

//! \brief Flags used to set YVR options
typedef enum yvrOptionFlags_
{
    kProtectedContent = (1 << 0),
    kMotionAwareFrames = (1 << 1),
    kFoveationSubsampled = (1 << 2),
    kPhaseSync = (1 << 3),
} yvrOptionFlags;

typedef enum yvrBatteryStatus_
{
    BATTERY_STATUS_UNKNOWN = 1,
    BATTERY_STATUS_CHARGING = 2,
    BATTERY_STATUS_DISCHARGING = 3,
    BATTERY_STATUS_NOT_CHARGING = 4,
    BATTERY_STATUS_FULL = 5,
} yvrBatteryStatus;

typedef enum yvrPowerSavingState_
{
    OUT_POWER_SAVING_MODE = 0,
    IN_POWER_SAVING_MODE = 1,
    POWER_SAVING_MODE_UNKOWN = 2,
} yvrPowerSavingState;

typedef enum yvrSystemProperty_
{
    YVRLIB_SYS_PROP_DISPLAY_PIXELS_WIDTH = 0,  // int
    YVRLIB_SYS_PROP_DISPLAY_PIXELS_HEIGHT,     // int
    YVRLIB_SYS_PROP_RENDER_PIXELS_WIDTH,       // int
    YVRLIB_SYS_PROP_RENDER_PIXELS_HEIGHT,      // int
    YVRLIB_SYS_PROP_DISPLAY_REFRESH_RATE,      // float
    YVRLIB_SYS_PROP_BATTERY_LEVEL,             // float
    YVRLIB_SYS_PROP_BATTERY_TEMPERATURE,       // float
    YVRLIB_SYS_PROP_CHARGE_STATUS,             // int
    YVRLIB_SYS_PROP_VOLUME_LEVEL,              // float
    YVRLIB_SYS_PROP_POWERSAVING_MODE,          // int
    YVRLIB_SYS_PROP_CPU_UTILIZATION,           // float
    YVRLIB_SYS_PROP_GPU_UTILIZATION,           // float
    YVRLIB_SYS_PROP_CPU_LEVEL,                 // int
    YVRLIB_SYS_PROP_GPU_LEVEL,                 // int
    YVRLIB_SYS_PROP_HMD_MOUNT,                 // bool
    YVRLIB_SYS_PROP_LEFT_EYE_FOV,              // float array
    YVRLIB_SYS_PROP_RIGHT_EYE_FOV,             // float array
    YVRLIB_SYS_PROP_EYE_IPD,                   // float
    YVRLIB_SYS_PROP_FOVEATION_LOW,             // float array
    YVRLIB_SYS_PROP_FOVEATION_MED,             // float array
    YVRLIB_SYS_PROP_FOVEATION_HIGH,            // float array
    YVRLIB_SYS_PROP_FOVEATION_TOP,             // float array
} yvrSystemProperty;

/// Position and orientation together.
typedef struct yvrPosef_
{
    union
    {
        yvrQuatf rotation;
        yvrQuatf orientation;
    };
    union
    {
        yvrVector3f position;
        yvrVector3f translation;
    };
} yvrPosef;

// pose status mask
typedef enum yvrPoseStatus_
{
    POSE_STATUS_INVALID = 0,                 //< Orientation and Position invalid.
    POSE_STATUS_ORIENTATION_VALID = 1 << 0,  //< Orientation is currently valid.
    POSE_STATUS_POSITION_VALID = 1 << 1,     //< Position is currently valid.
} yvrPoseStatus;

typedef struct yvrPoseState_
{
    yvrPosef pose;
    int32_t poseStatus; // see yvrPoseStatus
    uint64_t poseTimeStampNs;
    uint64_t poseFetchTimeNs;
    uint64_t expectedDisplayTimeNs;
    yvrVector3f angularVelocity;
    yvrVector3f linearVelocity;
    yvrVector3f angularAcceleration;
    yvrVector3f linearAcceleration;
} yvrPoseState;

//! \brief Enum used to indicate the type of texture passed in as a render layer
//! \sa yvrSubmitFrame
typedef enum yvrTextureType_
{
    kTypeTexture = 0,      //!< Standard texture
    kTypeTextureArray,     //!< Standard texture array (Left eye is first layer, right eye is second layer)
    kTypeImage,            //!< EGL Image texture
    kTypeEquiRectTexture,  //!< Equirectangular texture
    kTypeEquiRectImage,    //!< Equirectangular Image texture
    kTypeCubemapTexture,   //!< Cubemap texture (Not supporting cubemap image)
    kTypeVulkan,           //!< Vulkan texture
} yvrTextureType;

/// Flags used in yvrSubmitLayer.layerFlags
typedef enum yvrLayerFlags_
{
    kLayerFlagNone = 0x00000000,
    kLayerFlagHeadLocked = 0x00000001,
    kLayerFlagOpaque = 0x00000002,
    kLayerFlagSubsampled = 0x00000004,
} yvrLayerFlags;

/// Flags used in SubmitFrameDescriptionInternal.frameOptions
typedef enum yvrFrameOptions_
{
    kDisableDistortionCorrection = (1 << 0),  //!< Disables the lens distortion correction (useful for debugging)
    kDisableReprojection = (1 << 1),          //!< Disables re-projection
    EnableMotionToPhoton = (1 << 2),          //!< Enables motion to photon testing
    kDisableChromaticCorrection = (1 << 3)    //!< Disables the lens chromatic aberration correction (performance optimization)
} yvrFrameOptions;

/// Possible results of initialization.
typedef enum yvrInitializeStatus_
{
    YVRLIB_INITIALIZE_SUCCESS = 0,
    YVRLIB_INITIALIZE_UNKNOWN_ERROR = -1,
    YVRLIB_INITIALIZE_PERMISSIONS_ERROR = -2,
    YVRLIB_INITIALIZE_ALREADY_INITIALIZED = -3,
    YVRLIB_INITIALIZE_INTERNAL_ERROR = -4,
    YVRLIB_INITIALIZE_DEVICE_NOT_SUPPORTED = -5,
    YVRLIB_INITIALIZE_PARAMETERS_ERROR = -6,
} yvrInitializeStatus;

//! \brief Structure containing parameters needed to enter VR mode
//! \sa yvrlib_EnterVrMode, yvrOptionFlags
typedef struct yvrBeginParams_
{
    ANativeWindow* nativeWindow;
    yvrOptionFlags optionFlags;       //!< Flags to specify YVR options (yvrOptionFlags)
    yvrColorSpace colorSpace;  //!< Color space to utilize for the display surface
} yvrBeginParams;

typedef enum yvrButton_
{
    kButtonA = 0x00000001,
    kButtonB = 0x00000002,
    kButtonX = 0x00000004,
    kButtonY = 0x00000008,
    kButtonStart = 0x00000010,
    kButtonReserved = 0x00000020,
    kButtonLIndexTrigger = 0x00000040,
    kButtonRIndexTrigger = 0x00000080,
    kButtonLHandTrigger = 0x00000100,
    kButtonRHandTrigger = 0x00000200,
    kButtonLThumbstick = 0x00000400,
    kButtonRThumbstick = 0x00000800,
    kButtonLThumbstickUp = 0x00001000,
    kButtonLThumbstickDown = 0x00002000,
    kButtonLThumbstickLeft = 0x00004000,
    kButtonLThumbstickRight = 0x00008000,
    kButtonRThumbstickUp = 0x00010000,
    kButtonRThumbstickDown = 0x00020000,
    kButtonRThumbstickLeft = 0x00040000,
    kButtonRThumbstickRight = 0x00808000,
    yvrButton_EnumSize
} yvrButton;

typedef struct yvrControllerState_
{
    uint32_t Buttons;
    uint32_t Touches;
    float IndexTrigger;
    float Thumbstick[2];
    uint8_t BatteryPercentRemaining;
    uint8_t is_charging;
    //YVRLIB_PADDING(2)
} yvrControllerState;

typedef struct yvrControllerConnectedInfo_
{
    bool left_connected;
    bool right_connected;
    //YVRLIB_PADDING(6)
} yvrControllerConnectedInfo;

typedef struct yvrEyeFov_
{
    float UpFov;
    float DownFov;
    float LeftFov;
    float RightFov;
} yvrEyeFov;

typedef struct yvrTextureSwapChain yvrTextureSwapChain;

typedef enum yvrSwapChainCreateFlags_
{

} yvrSwapChainCreateFlags;

typedef enum yvrSwapChainUsageFlags_
{

} yvrSwapChainUsageFlags;

typedef struct yvrSwapChainCreateInfo_
{
    int64_t Format;
    int Width;
    int Height;
    int Levels;
    int FaceCount;
    int ArraySize;
    int BufferCount;
    uint64_t CreateFlags;
    uint64_t UsageFlags;
} yvrSwapChainCreateInfo;

typedef struct yvrSubmitLayer_
{
    int SwapChainIndex;
    yvrTextureType imageType;
    yvrVector4f VertexCoord[4];
    yvrVector2f TextureCoord[4];
    yvrMatrix4f TexCoordsMatrix;
    yvrEyeMask eyeMask;
    uint32_t layerFlags;
    yvrTextureSwapChain* ColorSwapChain;
} yvrSubmitLayer;

typedef struct yvrLayerProjection_
{
    yvrSubmitLayer mTextures[kNumEyes];
} yvrLayerProjection;

typedef struct yvrSubmitFrameDescription_
{
    uint64_t FrameIndex;
    int32_t minVsyncs;
    uint32_t LayerCount;
    uint32_t frameOptions;
    yvrPoseState HeadPoseState;
    yvrLayerProjection** Layers;
} yvrSubmitFrameDescription;

typedef enum yvrTrackingSpace_
{
    YVRLIB_TRACKING_SPACE_LOCAL = 0,            // Eye level origin - controlled by system recentering
    YVRLIB_TRACKING_SPACE_LOCAL_FLOOR = 1,      // Floor level origin - controlled by system recentering
    YVRLIB_TRACKING_SPACE_LOCAL_TILTED = 2,     // Tilted pose for "bed mode" - controlled by system recentering
    YVRLIB_TRACKING_SPACE_STAGE = 3,            // Floor level origin - controlled by Guardian setup
    YVRLIB_TRACKING_SPACE_LOCAL_FIXED_YAW = 4,  // Position of local space, but yaw stays constant
} yvrTrackingSpace;

typedef struct BoundaryTestResult_
{
    bool IsTriggering;
    float ClosestDistance;
    yvrVector3f ClosestPoint;
    yvrVector3f ClosestPointNormal;
} BoundaryTestResult;

typedef struct BoundaryPoint_
{
    yvrVector3f point;
} BoundaryPoint;

typedef struct BoundaryDimensions_
{
    yvrVector3f dimensions;
} BoundaryDimensions;

typedef struct yvrFoveation_
{
    yvrVector2f gain;  //!< Foveation Gain Rate [1, ...]
    float area;        //!< Foveation Area Size [0, ...]
    float minimum;     //!< Foveation Minimum Resolution [1, 1/2, 1/4, ..., 1/16, 0]
} yvrFoveation;

typedef enum yvrExtraLatencyMode_
{
    YVRLIB_EXTRA_LATENCY_MODE_OFF = 0,
    YVRLIB_EXTRA_LATENCY_MODE_ON = 1,
    YVRLIB_EXTRA_LATENCY_MODE_DYNAMIC = 2
} yvrExtraLatencyMode;


typedef struct yvrTrackingData_
{
    uint64_t timestamp;
    yvrPoseState headPose;
    yvrPoseState leftControllerPose;
    yvrPoseState rightControllerPose;
    yvrControllerState leftControllerState;
    yvrControllerState rightControllerState;
    yvrControllerConnectedInfo controllerConnectedInfo;

    XrHandJointLocationsEXT leftHandControllers;
    std::array<XrHandJointLocationEXT, XR_HAND_JOINT_COUNT_EXT> leftJointLocations;
    XrHandTrackingScaleFB scaleL;
    XrHandTrackingCapsulesStateFB capsuleStateL;
    XrHandTrackingAimStateFB aimStateL;
    std::array<XrHandJointVelocityEXT, XR_HAND_JOINT_COUNT_EXT> jointVelocitiesL;
    XrHandJointVelocitiesEXT velocitiesL;
    XrHandJointLocationsEXT rightHandControllers;
    std::array<XrHandJointLocationEXT, XR_HAND_JOINT_COUNT_EXT> rightJointLocations;
    XrHandTrackingScaleFB scaleR;
    XrHandTrackingCapsulesStateFB capsuleStateR;
    XrHandTrackingAimStateFB aimStateR;
    std::array<XrHandJointVelocityEXT, XR_HAND_JOINT_COUNT_EXT> jointVelocitiesR;
    XrHandJointVelocitiesEXT velocitiesR;
} yvrTrackingData;


#endif
