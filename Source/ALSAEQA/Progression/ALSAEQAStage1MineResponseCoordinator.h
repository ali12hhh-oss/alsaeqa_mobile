#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAStage1MineResponseCoordinator.generated.h"

class AALSAEQAWorkerPrisonerActor;
class AALSAEQAEnemyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAStage1WorkerEscapeStartedSignature, AALSAEQAWorkerPrisonerActor*, Worker);

/** Coordinates the living response of the mine during Stage 1. */
UCLASS(ClassGroup=(ALSAEQA), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAStage1MineResponseCoordinator : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAStage1MineResponseCoordinator();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Stage1|MineResponse")
    void RegisterWorker(AALSAEQAWorkerPrisonerActor* Worker);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Stage1|MineResponse")
    void RegisterMineGuard(AALSAEQAEnemyCharacter* Guard);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Stage1|MineResponse")
    int32 GetWorkersRescued() const { return WorkersRescued; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Stage1|MineResponse")
    int32 GetResponseLevel() const { return ResponseLevel; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Stage1|MineResponse")
    bool IsMineAlerted() const { return ResponseLevel > 0; }

    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Stage1|MineResponse")
    void PlayMineResponsePresentation(int32 NewResponseLevel, AALSAEQAWorkerPrisonerActor* Worker);

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void HandleWorkerEscapeStarted(AALSAEQAWorkerPrisonerActor* Worker);

    UFUNCTION()
    void HandleWorkerRescued(AALSAEQAWorkerPrisonerActor* Worker);

    void RefreshGuardAssignments();
    void ValidateStage1Setup() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Stage1|MineResponse", meta=(AllowPrivateAccess="true", ClampMin="5", UIMin="5"))
    int32 RequiredWorkers = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Stage1|MineResponse", meta=(AllowPrivateAccess="true", ClampMin="7", UIMin="7"))
    int32 MinimumMineGuards = 7;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Stage1|MineResponse", meta=(AllowPrivateAccess="true", ClampMin="1", UIMin="1"))
    int32 MaxResponseLevel = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Stage1|MineResponse", meta=(AllowPrivateAccess="true", ClampMin="1", UIMin="1"))
    int32 GuardsPerResponseLevel = 1;

    UPROPERTY()
    TArray<TObjectPtr<AALSAEQAWorkerPrisonerActor>> Workers;

    UPROPERTY()
    TArray<TObjectPtr<AALSAEQAEnemyCharacter>> Guards;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Stage1|MineResponse", meta=(AllowPrivateAccess="true"))
    int32 WorkersRescued = 0;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Stage1|MineResponse", meta=(AllowPrivateAccess="true"))
    int32 ResponseLevel = 0;
};
