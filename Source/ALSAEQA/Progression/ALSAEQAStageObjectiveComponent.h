#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAStageObjectiveComponent.generated.h"

class UALSAEQAStageFlowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALSAEQAObjectiveProgressSignature, FName, ObjectiveId, int32, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAObjectiveCompletedSignature, FName, ObjectiveId);

/**
 * Runtime objective gate for the canonical adventure.
 * Gameplay actors report progress here; the component completes the current
 * stage only when every configured objective has been satisfied.
 */
UCLASS(ClassGroup=(ALSAEQA), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAStageObjectiveComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAStageObjectiveComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression|Objectives")
    bool RegisterProgress(FName ObjectiveId, int32 Amount = 1);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression|Objectives")
    bool CompleteObjective(FName ObjectiveId);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression|Objectives")
    void ResetObjectives();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Objectives")
    bool IsObjectiveComplete(FName ObjectiveId) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Objectives")
    int32 GetObjectiveProgress(FName ObjectiveId) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Objectives")
    bool AreAllObjectivesComplete() const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Objectives")
    int32 GetCompletedObjectiveCount() const;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression|Objectives")
    bool FinalizeStageIfReady();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Progression|Objectives")
    TMap<FName, int32> ObjectiveRequirements;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Progression|Objectives")
    FALSAEQAObjectiveProgressSignature OnObjectiveProgress;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Progression|Objectives")
    FALSAEQAObjectiveCompletedSignature OnObjectiveCompleted;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TObjectPtr<UALSAEQAStageFlowComponent> StageFlow;

    UPROPERTY()
    TMap<FName, int32> ObjectiveProgress;
};
