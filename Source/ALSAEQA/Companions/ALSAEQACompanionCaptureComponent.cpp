#include "Companions/ALSAEQACompanionCaptureComponent.h"

UALSAEQACompanionCaptureComponent::UALSAEQACompanionCaptureComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UALSAEQACompanionCaptureComponent::Capture()
{
    if (State == EALSAEQACompanionCaptureState::Captured || State == EALSAEQACompanionCaptureState::RescueKnown)
    {
        return false;
    }

    State = EALSAEQACompanionCaptureState::Captured;
    OnStateChanged.Broadcast(State);
    return true;
}

bool UALSAEQACompanionCaptureComponent::MarkRescueKnown()
{
    if (State != EALSAEQACompanionCaptureState::Captured)
    {
        return false;
    }

    State = EALSAEQACompanionCaptureState::RescueKnown;
    OnStateChanged.Broadcast(State);
    return true;
}

bool UALSAEQACompanionCaptureComponent::Rescue()
{
    if (State != EALSAEQACompanionCaptureState::Captured && State != EALSAEQACompanionCaptureState::RescueKnown)
    {
        return false;
    }

    State = EALSAEQACompanionCaptureState::Rescued;
    OnStateChanged.Broadcast(State);
    return true;
}

void UALSAEQACompanionCaptureComponent::ResetToFree()
{
    if (State == EALSAEQACompanionCaptureState::Free)
    {
        return;
    }

    State = EALSAEQACompanionCaptureState::Free;
    OnStateChanged.Broadcast(State);
}
