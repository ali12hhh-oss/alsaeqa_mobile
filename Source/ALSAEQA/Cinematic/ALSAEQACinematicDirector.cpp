#include "Cinematic/ALSAEQACinematicDirector.h"

#include "Engine/World.h"
#include "TimerManager.h"

UALSAEQACinematicDirector::UALSAEQACinematicDirector()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQACinematicDirector::BeginPlay()
{
    Super::BeginPlay();
}

bool UALSAEQACinematicDirector::PlayActionMoment(const FALSAEQACinematicRequest& Request)
{
    UWorld* World = GetWorld();
    if (!World || Request.Duration <= 0.0f)
    {
        return false;
    }

    StopActionMoment();

    PreviousTimeDilation = World->GetWorldSettings()->GetEffectiveTimeDilation();
    World->GetWorldSettings()->SetTimeDilation(FMath::Clamp(Request.SlowMotionScale, 0.05f, 1.0f));
    bActionMomentActive = true;
    OnCinematicEvent.Broadcast(Request.Event);

    World->GetTimerManager().SetTimer(
        RestoreTimeHandle,
        this,
        &UALSAEQACinematicDirector::RestoreNormalTime,
        Request.Duration,
        false);

    return true;
}

bool UALSAEQACinematicDirector::StartOpeningCinematic()
{
    if (bOpeningCinematicPlayed || bActionMomentActive)
    {
        return false;
    }

    bOpeningCinematicPlayed = true;
    OnCinematicEvent.Broadcast(EALSAEQACinematicEvent::Opening);
    HandleOpeningCinematic();
    return true;
}

void UALSAEQACinematicDirector::FinishOpeningCinematic()
{
    OnCinematicEvent.Broadcast(EALSAEQACinematicEvent::Opening);
}

bool UALSAEQACinematicDirector::StartStoryBeat(EALSAEQACinematicEvent Event)
{
    if (bActionMomentActive || Event == EALSAEQACinematicEvent::CombatFinisher ||
        Event == EALSAEQACinematicEvent::AbilityImpact)
    {
        return false;
    }

    OnCinematicEvent.Broadcast(Event);
    HandleStoryBeat(Event);
    return true;
}

void UALSAEQACinematicDirector::ResetOpeningCinematic()
{
    bOpeningCinematicPlayed = false;
}

void UALSAEQACinematicDirector::StopActionMoment()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(RestoreTimeHandle);
        World->GetWorldSettings()->SetTimeDilation(PreviousTimeDilation > 0.0f ? PreviousTimeDilation : 1.0f);
    }

    bActionMomentActive = false;
}

void UALSAEQACinematicDirector::RestoreNormalTime()
{
    if (UWorld* World = GetWorld())
    {
        World->GetWorldSettings()->SetTimeDilation(PreviousTimeDilation > 0.0f ? PreviousTimeDilation : 1.0f);
    }
    bActionMomentActive = false;
}