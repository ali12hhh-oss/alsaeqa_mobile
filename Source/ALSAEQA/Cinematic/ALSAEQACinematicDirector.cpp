#include "Cinematic/ALSAEQACinematicDirector.h"

#include "Cinematic/ALSAEQACinematicPresetLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Save/ALSAEQASaveManager.h"

UALSAEQACinematicDirector::UALSAEQACinematicDirector()
{
    PrimaryComponentTick.bCanEverTick = false;
    BuildDefaultOpeningBeats();
}

void UALSAEQACinematicDirector::BuildDefaultOpeningBeats()
{
    OpeningBeats.Reset();
    OpeningBeats.Add({ EALSAEQAOpeningSequenceStage::StormBeforeMemory,       6.0f, false });
    OpeningBeats.Add({ EALSAEQAOpeningSequenceStage::FallOfTheThunderLegacy,  7.0f, false });
    OpeningBeats.Add({ EALSAEQAOpeningSequenceStage::SecondCaptorsAndTheHeir, 7.0f, false });
    OpeningBeats.Add({ EALSAEQAOpeningSequenceStage::TheCollapse,             7.0f, false });
    OpeningBeats.Add({ EALSAEQAOpeningSequenceStage::TheFirstAwakening,      10.0f, false });
    OpeningBeats.Add({ EALSAEQAOpeningSequenceStage::HandoffToStageOne,       8.0f, true  });
}

void UALSAEQACinematicDirector::BeginPlay()
{
    Super::BeginPlay();
}

UALSAEQASaveManager* UALSAEQACinematicDirector::GetSaveManager() const
{
    const UWorld* World = GetWorld();
    UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
    return GI ? GI->GetSubsystem<UALSAEQASaveManager>() : nullptr;
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

bool UALSAEQACinematicDirector::PlayActionMomentForEvent(EALSAEQACinematicEvent Event)
{
    return PlayActionMoment(UALSAEQACinematicPresetLibrary::MakeRequestForEvent(Event));
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

bool UALSAEQACinematicDirector::HasOpeningCinematicPlayed() const
{
    if (const UALSAEQASaveManager* Save = GetSaveManager())
    {
        return Save->HasCompletedOpeningCinematic();
    }
    return bLocalOpeningCompletedFallback;
}

EALSAEQAOpeningSequenceStage UALSAEQACinematicDirector::GetCurrentOpeningStage() const
{
    return OpeningBeats.IsValidIndex(CurrentOpeningStageIndex)
        ? OpeningBeats[CurrentOpeningStageIndex].Stage
        : EALSAEQAOpeningSequenceStage::StormBeforeMemory;
}

bool UALSAEQACinematicDirector::StartOpeningCinematic(bool bForceReplay)
{
    if (bOpeningInProgress || bActionMomentActive)
    {
        return false;
    }
    if (!bForceReplay && HasOpeningCinematicPlayed())
    {
        return false;
    }
    if (OpeningBeats.Num() == 0)
    {
        BuildDefaultOpeningBeats();
    }

    bOpeningInProgress = true;
    CurrentOpeningStageIndex = -1;
    OnCinematicEvent.Broadcast(EALSAEQACinematicEvent::Opening);
    AdvanceOpeningBeat();
    return true;
}

void UALSAEQACinematicDirector::AdvanceOpeningBeat()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    ++CurrentOpeningStageIndex;

    if (!OpeningBeats.IsValidIndex(CurrentOpeningStageIndex))
    {
        FinishOpeningCinematic();
        return;
    }

    const FALSAEQAOpeningSequenceBeat& Beat = OpeningBeats[CurrentOpeningStageIndex];
    OnOpeningStageEntered.Broadcast(Beat.Stage, Beat.Duration);
    HandleOpeningStage(Beat.Stage, Beat.Duration);

    World->GetTimerManager().SetTimer(
        OpeningBeatTimerHandle,
        this,
        &UALSAEQACinematicDirector::AdvanceOpeningBeat,
        FMath::Max(Beat.Duration, 0.05f),
        false);
}

bool UALSAEQACinematicDirector::CanSkipOpeningCinematic() const
{
    if (!bOpeningInProgress)
    {
        return false;
    }
    if (!OpeningBeats.IsValidIndex(CurrentOpeningStageIndex))
    {
        return true;
    }
    return !OpeningBeats[CurrentOpeningStageIndex].bCriticalBoundary;
}

bool UALSAEQACinematicDirector::SkipOpeningCinematic()
{
    if (!CanSkipOpeningCinematic())
    {
        return false;
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(OpeningBeatTimerHandle);
    }

    const int32 LastIndex = OpeningBeats.Num() - 1;
    if (OpeningBeats.IsValidIndex(LastIndex))
    {
        CurrentOpeningStageIndex = LastIndex;
        const FALSAEQAOpeningSequenceBeat& Beat = OpeningBeats[LastIndex];
        OnOpeningStageEntered.Broadcast(Beat.Stage, Beat.Duration);
        HandleOpeningStage(Beat.Stage, Beat.Duration);
    }

    FinishOpeningCinematic();
    return true;
}

void UALSAEQACinematicDirector::FinishOpeningCinematic()
{
    if (!bOpeningInProgress)
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(OpeningBeatTimerHandle);
    }

    bOpeningInProgress = false;
    bLocalOpeningCompletedFallback = true;

    if (UALSAEQASaveManager* Save = GetSaveManager())
    {
        Save->MarkOpeningCinematicCompleted();
    }

    OnCinematicEvent.Broadcast(EALSAEQACinematicEvent::OpeningCompleted);
    HandleOpeningCinematicCompleted();
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

void UALSAEQACinematicDirector::ResetOpeningCinematic(bool bAlsoClearSavedCompletion)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(OpeningBeatTimerHandle);
    }
    bOpeningInProgress = false;
    CurrentOpeningStageIndex = -1;
    bLocalOpeningCompletedFallback = false;

    if (bAlsoClearSavedCompletion)
    {
        if (UALSAEQASaveManager* Save = GetSaveManager())
        {
            Save->ResetOpeningCinematicCompleted();
        }
    }
}
