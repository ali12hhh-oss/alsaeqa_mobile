#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cinematic/ALSAEQACinematicTypes.h"
#include "ALSAEQACinematicDirector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQACinematicEvent, EALSAEQACinematicEvent, Event);

UCLASS(ClassGroup=(ALSAEQA), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQACinematicDirector : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQACinematicDirector();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    bool PlayActionMoment(const FALSAEQACinematicRequest& Request);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    void StopActionMoment();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    bool StartOpeningCinematic();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    void FinishOpeningCinematic();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    bool StartStoryBeat(EALSAEQACinematicEvent Event);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Cinematics")
    void ResetOpeningCinematic();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Cinematics")
    bool IsActionMomentActive() const { return bActionMomentActive; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Cinematics")
    bool HasOpeningCinematicPlayed() const { return bOpeningCinematicPlayed; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Cinematics")
    FALSAEQACinematicEvent OnCinematicEvent;

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Cinematics")
    void HandleOpeningCinematic();

    // Blueprint/Sequencer implementation point for authored story shots such
    // as rescue beats and the automatic Stage 1 -> Stage 2 transition.
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Cinematics")
    void HandleStoryBeat(EALSAEQACinematicEvent Event);

private:
    FTimerHandle RestoreTimeHandle;
    float PreviousTimeDilation = 1.0f;
    bool bActionMomentActive = false;
    bool bOpeningCinematicPlayed = false;

    void RestoreNormalTime();
};