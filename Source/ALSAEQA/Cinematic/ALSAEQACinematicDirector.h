#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cinematic/ALSAEQACinematicTypes.h"
#include "ALSAEQACinematicDirector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQACinematicEvent, EALSAEQACinematicEvent, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALSAEQAOpeningStageEvent, EALSAEQAOpeningSequenceStage, Stage, float, Duration);

class UALSAEQASaveManager;

// Owns cinematic *state and timing* only. The authored film itself (cine cameras,
// facial/body animation, VFX, audio) is built in Unreal Editor with Sequencer/Blueprint
// bound to the events below, per Docs/OPENING_CINEMATIC.md's authoring contract.
UCLASS(ClassGroup=(ALSAEQA), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQACinematicDirector : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQACinematicDirector();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    bool PlayActionMoment(const FALSAEQACinematicRequest& Request);

    // Convenience wrapper: looks up a professionally-tuned request from
    // UALSAEQACinematicPresetLibrary for this event and plays it.
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    bool PlayActionMomentForEvent(EALSAEQACinematicEvent Event);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    void StopActionMoment();

    // Begins the six-beat authored opening sequence (Docs/OPENING_CINEMATIC.md).
    // No-ops if the opening has already been completed and persisted, unless
    // bForceReplay is set (dev tools / a "watch intro again" menu option only).
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics|Opening")
    bool StartOpeningCinematic(bool bForceReplay = false);

    // Called automatically when the final authored beat elapses, or manually from a
    // Level Sequence's "Finished" event once the real cinematic is authored. Idempotent.
    // This is the only place completion is persisted, so an interrupted/never-started
    // opening can never be mistaken for a finished one.
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics|Opening")
    void FinishOpeningCinematic();

    // Safely skips the remainder of the opening. Refuses while the current beat is the
    // final hand-off (bCriticalBoundary), so the title reveal / Stage 1 camera hand-off
    // always plays out cleanly rather than being interrupted mid-transition.
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics|Opening")
    bool SkipOpeningCinematic();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Cinematics|Opening")
    bool CanSkipOpeningCinematic() const;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    bool StartStoryBeat(EALSAEQACinematicEvent Event);

    // Dev/testing utility. bAlsoClearSavedCompletion additionally clears the persisted
    // save flag so a "watch intro again" flow can be tested end to end.
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics|Opening")
    void ResetOpeningCinematic(bool bAlsoClearSavedCompletion = false);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Cinematics")
    bool IsActionMomentActive() const { return bActionMomentActive; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Cinematics|Opening")
    bool IsOpeningCinematicPlaying() const { return bOpeningInProgress; }

    // True only once the sequence has actually reached its final beat and
    // FinishOpeningCinematic() has run and persisted - never true merely because
    // StartOpeningCinematic() was called.
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Cinematics|Opening")
    bool HasOpeningCinematicPlayed() const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Cinematics|Opening")
    EALSAEQAOpeningSequenceStage GetCurrentOpeningStage() const;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Cinematics")
    FALSAEQACinematicEvent OnCinematicEvent;

    // Fired once per authored beat as the opening sequence steps through it, so
    // Sequencer/Blueprint can trigger the matching camera/audio/VFX for that shot.
    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Cinematics|Opening")
    FALSAEQAOpeningStageEvent OnOpeningStageEntered;

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Cinematics|Opening")
    void HandleOpeningStage(EALSAEQAOpeningSequenceStage Stage, float Duration);

    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Cinematics|Opening")
    void HandleOpeningCinematicCompleted();

    // Blueprint/Sequencer implementation point for authored story shots such
    // as rescue beats and the automatic Stage 1 -> Stage 2 transition.
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Cinematics")
    void HandleStoryBeat(EALSAEQACinematicEvent Event);

private:
    // Authored beat table - exact timings from Docs/OPENING_CINEMATIC.md. Editable per
    // level/BP subclass in case a region needs a different cut, but defaults to canon.
    UPROPERTY(EditDefaultsOnly, Category="ALSAEQA|Cinematics|Opening")
    TArray<FALSAEQAOpeningSequenceBeat> OpeningBeats;

    FTimerHandle RestoreTimeHandle;
    FTimerHandle OpeningBeatTimerHandle;
    float PreviousTimeDilation = 1.0f;
    bool bActionMomentActive = false;
    bool bOpeningInProgress = false;
    bool bLocalOpeningCompletedFallback = false;
    int32 CurrentOpeningStageIndex = -1;

    void RestoreNormalTime();
    void AdvanceOpeningBeat();
    void BuildDefaultOpeningBeats();
    UALSAEQASaveManager* GetSaveManager() const;
};
