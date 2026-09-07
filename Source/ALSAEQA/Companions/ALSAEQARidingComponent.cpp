#include "Companions/ALSAEQARidingComponent.h"

#include "Companions/ALSAEQAMountActor.h"
#include "Companions/ALSAEQAMountComponent.h"

UALSAEQARidingComponent::UALSAEQARidingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UALSAEQARidingComponent::TryMount(AALSAEQAMountActor* Mount)
{
    if (!Mount || IsRiding())
    {
        return false;
    }

    if (!Mount->MountRider(GetOwner()))
    {
        return false;
    }

    CurrentMount = Mount;
    CurrentMountId = Mount->GetMountComponent() ? Mount->GetMountComponent()->GetMountProfile().MountId : NAME_None;
    OnRidingStateChanged.Broadcast(true);
    return true;
}

bool UALSAEQARidingComponent::Dismount()
{
    AALSAEQAMountActor* Mount = CurrentMount.Get();
    if (!Mount)
    {
        CurrentMount.Reset();
        CurrentMountId = NAME_None;
        return false;
    }

    if (!Mount->HasRider())
    {
        CurrentMount.Reset();
        CurrentMountId = NAME_None;
        OnRidingStateChanged.Broadcast(false);
        return true;
    }

    if (!Mount->DismountRider())
    {
        return false;
    }

    CurrentMount.Reset();
    CurrentMountId = NAME_None;
    OnRidingStateChanged.Broadcast(false);
    return true;
}

bool UALSAEQARidingComponent::MoveForward(float Value)
{
    if (!IsRiding())
    {
        return false;
    }
    return CurrentMount->MoveRiderForward(Value);
}

bool UALSAEQARidingComponent::MoveRight(float Value)
{
    if (!IsRiding())
    {
        return false;
    }
    return CurrentMount->MoveRiderRight(Value);
}

bool UALSAEQARidingComponent::SetSprint(bool bEnabled)
{
    if (!IsRiding())
    {
        return false;
    }
    return CurrentMount->SetRiderSprint(bEnabled);
}

bool UALSAEQARidingComponent::IsRiding() const
{
    return CurrentMount.IsValid() && CurrentMount->HasRider();
}
