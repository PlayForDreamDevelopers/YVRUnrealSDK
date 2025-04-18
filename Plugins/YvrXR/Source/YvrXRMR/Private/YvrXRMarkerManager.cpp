#include "YvrXRMarkerManager.h"
#include "YvrXRMarkerComponent.h"

FYvrXRMarkerManager::FYvrXRMarkerManager()
{
}

FYvrXRMarkerManager::~FYvrXRMarkerManager()
{
}

FYvrXRMarkerManager* FYvrXRMarkerManager::GetInstance()
{
	static FYvrXRMarkerManager Instance;
	return &Instance;
}

void FYvrXRMarkerManager::Initialize(FYvrXRHMD* InYvrXRHMD)
{
	YvrXRHMD = InYvrXRHMD;
	if (YvrXRHMD)
	{
		YvrXRHMD->OnPollEventDelegate().AddRaw(FYvrXRMarkerManager::GetInstance(), &FYvrXRMarkerManager::PollEvent);
	}
}

void FYvrXRMarkerManager::PollEvent(XrEventDataBuffer EventData)
{
	if (EventData.type == XR_TYPE_EVENT_DATA_MARKER_TRACKING_UPDATE_VARJO)
	{
        bool newMarker = false;
        const auto& markerUpdate = reinterpret_cast<const XrEventDataMarkerTrackingUpdateVARJO&>(EventData);
        auto id = markerUpdate.markerId;
        if (!markers.Contains(id) && markerUpdate.isActive) {
            XrMarkerSpaceCreateInfoVARJO spaceInfo;
            spaceInfo.type = XR_TYPE_MARKER_SPACE_CREATE_INFO_VARJO;
            spaceInfo.markerId = id;
            spaceInfo.poseInMarkerSpace = XrPosef{ {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} };
            Marker marker;

            PFN_xrCreateMarkerSpaceVARJO xrCreateMarkerSpaceVARJO = nullptr;
            XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrCreateMarkerSpaceVARJO", reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateMarkerSpaceVARJO)));

            PFN_xrGetMarkerSizeVARJO xrGetMarkerSizeVARJO = nullptr;
            XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrGetMarkerSizeVARJO", reinterpret_cast<PFN_xrVoidFunction*>(&xrGetMarkerSizeVARJO)));

            XR_ENSURE(xrCreateMarkerSpaceVARJO(YvrXRHMD->GetSession(), &spaceInfo, &marker.space));
            XR_ENSURE(xrGetMarkerSizeVARJO(YvrXRHMD->GetSession(), id, &marker.size));

            newMarker = true;
            markers.Add(id, marker);
        }

        if (markerUpdate.isActive)
        {
            XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };

            XR_ENSURE(xrLocateSpace(markers[id].space, YvrXRHMD->GetTrackingSpace(), YvrXRHMD->GetDisplayTime(), &location));
            if (location.locationFlags & XR_SPACE_LOCATION_POSITION_TRACKED_BIT)
            {
                FTransform markerTransform{};
                const XrPosef& pose = location.pose;
                const float worldToMetersScale = GEngine->XRSystem->GetWorldToMetersScale();
                const FTransform trackerTransform = ToFTransform(pose, worldToMetersScale);
                const FTransform& trackingToWoldTransform = GEngine->XRSystem->GetTrackingToWorldTransform();
                const FVector2D size = { markers[id].size.width * worldToMetersScale, markers[id].size.height * worldToMetersScale };
                markerTransform = trackerTransform * trackingToWoldTransform;

                if (newMarker)
                {
                    UYvrXRMarkerDelegates::NewYvrMarkerDetected.Broadcast(id, markerTransform.GetLocation(), markerTransform.Rotator(), size);
                }
                else
                {
                    UYvrXRMarkerDelegates::YvrMarkerMoved.Broadcast(id, markerTransform.GetLocation(), markerTransform.Rotator(), size);
                }
            }
        }
        else
        {
            UYvrXRMarkerDelegates::YvrMarkerLost.Broadcast(id);
        }
	}
}

bool FYvrXRMarkerManager::SetYvrMarkerEnabled(bool Enabled)
{
    PFN_xrSetMarkerTrackingVARJO xrSetMarkerTrackingVARJO = nullptr;
    XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrSetMarkerTrackingVARJO", reinterpret_cast<PFN_xrVoidFunction*>(&xrSetMarkerTrackingVARJO)));
    XrResult Result = xrSetMarkerTrackingVARJO(YvrXRHMD->GetSession(), Enabled);

    if (XR_FAILED(Result))
    {
        return false;
    }
    return true;
}
