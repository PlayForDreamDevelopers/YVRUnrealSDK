#ifndef SHARED_MEMORY_IPREVIEWTOOLSERVICE_H
#define SHARED_MEMORY_IPREVIEWTOOLSERVICE_H

#include <string>

#define SERVICE＿VERSION "v1.0.0"
#define SERVICE_PREVIEW_TOOL "service_preview_tool"

enum
{
    ID_GET_VERSION_NAME = 0,
    ID_SET_GRAPHICS_DEVICE,
    ID_SET_REFERENCE_SPACETYPE,
    ID_GET_DISPLAY_RESOLUTION,
    ID_GET_TRACKING_DATA,
    ID_PRESENT,
    ID_PRESENT2,
};

class IPreviewToolService {

public:
    virtual std::string getVersionName() = 0;

    virtual void setGraphicsDevice(int deviceId) = 0;
    virtual void setXrReferenceSpaceType(unsigned int type) = 0;

    virtual void getDisplayResolution(int &width, int &height) = 0;

    virtual bool getTrackingData(void *out_trackingData) = 0;

    /** Submits queued layers for display. */
    virtual void present(uint64_t in_textureHandle) = 0;

    virtual void present(uint64_t in_textureHandle, uint64_t timestamp) = 0;
};

#endif //SHARED_MEMORY_IPREVIEWTOOLSERVICE_H
