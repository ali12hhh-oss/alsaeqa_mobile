#include "Companions/ALSAEQAMountComponent.h"

UALSAEQAMountComponent::UALSAEQAMountComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQAMountComponent::SetState(EALSAEQAMountState NewState)
{
    if (MountState == NewState)
    {
        return;
    }

    MountState = NewState;
    OnStateChanged.Broadcast(NewState);
}

bool UALSAEQAMountComponent::InitializeWildMount(const FALSAEQAMountProfile& Profile)
{
    if (MountState != EALSAEQAMountState::Wild || Profile.MountId.IsNone())
    {
        return false;
    }

    MountProfile = Profile;
    MountProfile.MaxStamina = FMath::Max(MountProfile.MaxStamina, 1.0f);
    MountProfile.Stamina = FMath::Clamp(MountProfile.Stamina, 0.0f, MountProfile.MaxStamina);
    MountProfile.TamingRequired = FMath::Max(MountProfile.TamingRequired, 1.0f);
    MountProfile.TamingProgress = 0.0f;
    return true;
}

bool UALSAEQAMountComponent::BeginTaming()
{
    if (MountState != EALSAEQAMountState::Wild || MountProfile.MountId.IsNone())
    {
        return false;
    }

    SetState(EALSAEQAMountState::Taming);
    return true;
}

bool UALSAEQAMountComponent::AddTamingProgress(float Amount)
{
    if (MountState != EALSAEQAMountState::Taming || Amount <= 0.0f)
    {
        return false;
    }

    const float Required = FMath::Max(MountProfile.TamingRequired, 1.0f);
    MountProfile.TamingProgress = FMath::Clamp(MountProfile.TamingProgress + Amount, 0.0f, Required);
    const float NormalizedProgress = MountProfile.TamingProgress / Required;
    OnTamingProgressChanged.Broadcast(NormalizedProgress);

    if (MountProfile.TamingProgress >= Required)
    {
        SetState(EALSAEQAMountState::Tamed);
    }

    return true;
}

bool UALSAEQAMountComponent::TameMount(const FALSAEQAMountProfile& Profile)
{
    if (Profile.MountId.IsNone() || MountState == EALSAEQAMountState::Mounted || MountState == EALSAEQAMountState::Injured || MountState == EALSAEQAMountState::Released)
    {
        return false;
    }

    MountProfile = Profile;
    MountProfile.MaxStamina = FMath::Max(MountProfile.MaxStamina, 1.0f);
    MountProfile.Stamina = FMath::Clamp(MountProfile.Stamina, 0.0f, MountProfile.MaxStamina);
    MountProfile.TamingRequired = FMath::Max(MountProfile.TamingRequired, 1.0f);
    MountProfile.TamingProgress = MountProfile.TamingRequired;
    SetState(EALSAEQAMountState::Tamed);
    OnTamingProgressChanged.Broadcast(1.0f);
    return true;
}

bool UALSAEQAMountComponent::Mount()
{
    if (!HasTamedMount() || MountProfile.Stamina <= 0.0f)
    {
        return false;
    }

    SetState(EALSAEQAMountState::Mounted);
    OnMountChanged.Broadcast(MountProfile.MountId, true);
    return true;
}

bool UALSAEQAMountComponent::Dismount()
{
    if (!IsMounted())
    {
        return false;
    }

    SetState(EALSAEQAMountState::Tamed);
    OnMountChanged.Broadcast(MountProfile.MountId, false);
    return true;
}

bool UALSAEQAMountComponent::InjureMount()
{
    if (MountState == EALSAEQAMountState::Wild || MountState == EALSAEQAMountState::Released || MountState == EALSAEQAMountState::Injured)
    {
        return false;
    }

    if (MountState == EALSAEQAMountState::Mounted)
    {
        OnMountChanged.Broadcast(MountProfile.MountId, false);
    }

    SetState(EALSAEQAMountState::Injured);
    return true;
}

bool UALSAEQAMountComponent::HealMount(float Amount)
{
    if (!IsInjured() || Amount <= 0.0f)
    {
        return false;
    }

    MountProfile.Stamina = FMath::Clamp(MountProfile.Stamina + Amount, 0.0f, MountProfile.MaxStamina);
    if (MountProfile.Stamina > 0.0f)
    {
        SetState(EALSAEQAMountState::Tamed);
    }
    return true;
}

bool UALSAEQAMountComponent::RestoreStamina(float Amount)
{
    if (!HasTamedMount() || Amount <= 0.0f)
    {
        return false;
    }

    MountProfile.Stamina = FMath::Clamp(MountProfile.Stamina + Amount, 0.0f, MountProfile.MaxStamina);
    return true;
}

bool UALSAEQAMountComponent::ConsumeStamina(float Amount)
{
    if (!IsMounted() || Amount <= 0.0f || MountProfile.Stamina <= 0.0f)
    {
        return false;
    }

    MountProfile.Stamina = FMath::Clamp(MountProfile.Stamina - Amount, 0.0f, MountProfile.MaxStamina);
    return true;
}

bool UALSAEQAMountComponent::ReleaseMount()
{
    if (!HasTamedMount())
    {
        return false;
    }

    const FName ReleasedMountId = MountProfile.MountId;
    MountProfile = FALSAEQAMountProfile();
    SetState(EALSAEQAMountState::Released);
    OnMountChanged.Broadcast(ReleasedMountId, false);
    return true;
}
