#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAStageFlowComponent.generated.h"

class UALSAEQAProgressionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALSAEQAStageChangedSignature, int32, PreviousStage, int32, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAStageCompletionSignature, int32, CompletedStage);

/**
 * Drives the canonical adventure as one continuous sequence.
 * Gameplay systems report completion; this component chooses only the
 * immediate next stage from the canonical registry. There is deliberately
 * no public API for selecting an arbitrary stage.
 */
UCLASS(ClassGroup=(ALSAEQA), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAStageFlowComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAStageFlowComponent();

    /** Marks the current stage objective as complete and schedules the next stage automatically. */
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression|Flow")
    bool CompleteCurrentStage();

    /** Cancels a pending automatic transition without changing progression. */
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression|Flow")
    void CancelPendingTransition();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Flow")
    bool IsTransitionPending() const { return bTransitionPending; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Flow")
    int32 GetPendingStage() const { return PendingStage; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Progression|Flow")
    float TransitionDelay = 1.25f;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Progression|Flow")
    FALSAEQAStageCompletionSignature OnStageCompleted;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Progression|Flow")
    FALSAEQAStageChangedSignature OnAutomaticStageChanged;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TObjectPtr<UALSAEQAProgressionComponent> Progression;

    FTimerHandle TransitionTimerHandle;
    int32 PendingStage = INDEX_NONE;
    bool bTransitionPending = false;

    void ApplyPendingTransition();
};
