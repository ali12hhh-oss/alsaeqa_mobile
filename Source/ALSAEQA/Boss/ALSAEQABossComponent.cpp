#include "Boss/ALSAEQABossComponent.h"

UALSAEQABossComponent::UALSAEQABossComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UALSAEQABossComponent::SetHealthPercent(float NewPercent)
{
    if (IsDefeated())
    {
        return false;
    }

    const float Clamped = FMath::Clamp(NewPercent, 0.0f, 1.0f);
    HealthPercent = Clamped;

    EALSAEQABossPhase NewPhase = CurrentPhase;
    if (Clamped <= 0.0f)
    {
        NewPhase = EALSAEQABossPhase::Defeated;
    }
    else if (Clamped <= 0.25f)
    {
        NewPhase = EALSAEQABossPhase::PhaseThree;
    }
    else if (Clamped <= 0.60f)
    {
        NewPhase = EALSAEQABossPhase::PhaseTwo;
    }
    else
    {
        NewPhase = EALSAEQABossPhase::PhaseOne;
    }

    if (NewPhase != CurrentPhase)
    {
        CurrentPhase = NewPhase;
        OnPhaseChanged.Broadcast(CurrentPhase);
    }

    return true;
}

void UALSAEQABossComponent::SetDefeated()
{
    HealthPercent = 0.0f;
    if (CurrentPhase != EALSAEQABossPhase::Defeated)
    {
        CurrentPhase = EALSAEQABossPhase::Defeated;
        OnPhaseChanged.Broadcast(CurrentPhase);
    }
}
