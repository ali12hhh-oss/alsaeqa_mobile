#include "Storm/ALSAEQADynamicStormSubsystem.h"

void UALSAEQADynamicStormSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    bStormActive = false;
    Intensity = 0.0f;
    RemainingTime = 0.0f;
}

void UALSAEQADynamicStormSubsystem::Deinitialize()
{
    bStormActive = false;
    Intensity = 0.0f;
    RemainingTime = 0.0f;
    Super::Deinitialize();
}

void UALSAEQADynamicStormSubsystem::StartStorm(float DurationSeconds, float StormIntensity)
{
    bStormActive = true;
    RemainingTime = FMath::Max(DurationSeconds, 0.0f);
    Intensity = FMath::Clamp(StormIntensity, 0.0f, 1.0f);
    OnStormStateChanged.Broadcast(true);

    if (RemainingTime <= 0.0f)
    {
        StopStorm();
    }
}

void UALSAEQADynamicStormSubsystem::StopStorm()
{
    if (!bStormActive)
    {
        return;
    }

    bStormActive = false;
    RemainingTime = 0.0f;
    Intensity = 0.0f;
    OnStormStateChanged.Broadcast(false);
}

void UALSAEQADynamicStormSubsystem::Tick(float DeltaTime)
{
    if (!bStormActive)
    {
        return;
    }

    RemainingTime -= FMath::Max(DeltaTime, 0.0f);
    if (RemainingTime <= 0.0f)
    {
        StopStorm();
    }
}

bool UALSAEQADynamicStormSubsystem::IsTickable() const
{
    return !IsTemplate() && bStormActive;
}

TStatId UALSAEQADynamicStormSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UALSAEQADynamicStormSubsystem, STATGROUP_Tickables);
}

float UALSAEQADynamicStormSubsystem::GetThunderMultiplier() const
{
    if (!bStormActive)
    {
        return 1.0f;
    }

    // A storm is an opportunity, never a mandatory progression gate.
    return FMath::Lerp(1.0f, 1.75f, Intensity);
}
