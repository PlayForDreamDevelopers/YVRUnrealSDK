
#include "YvrXRMarkerComponent.h"

#include "Misc/CoreDelegates.h"


UYvrXRMarkerDelegates::FNewYvrMarkerDetected UYvrXRMarkerDelegates::NewYvrMarkerDetected;
UYvrXRMarkerDelegates::FYvrMarkerMoved UYvrXRMarkerDelegates::YvrMarkerMoved;
UYvrXRMarkerDelegates::FYvrMarkerLost UYvrXRMarkerDelegates::YvrMarkerLost;

void UYvrXRMarkerComponent::OnRegister()
{
    Super::OnRegister();

    UYvrXRMarkerDelegates::NewYvrMarkerDetected.AddUObject(this, &UYvrXRMarkerComponent::FNewYvrMarkerDetected_Handler);
    UYvrXRMarkerDelegates::YvrMarkerMoved.AddUObject(this, &UYvrXRMarkerComponent::FYvrMarkerMoved_Handler);
    UYvrXRMarkerDelegates::YvrMarkerLost.AddUObject(this, &UYvrXRMarkerComponent::FYvrMarkerLost_Handler);
}

void UYvrXRMarkerComponent::OnUnregister()
{
    Super::OnUnregister();

    UYvrXRMarkerDelegates::NewYvrMarkerDetected.RemoveAll(this);
    UYvrXRMarkerDelegates::YvrMarkerMoved.RemoveAll(this);
    UYvrXRMarkerDelegates::YvrMarkerLost.RemoveAll(this);
}