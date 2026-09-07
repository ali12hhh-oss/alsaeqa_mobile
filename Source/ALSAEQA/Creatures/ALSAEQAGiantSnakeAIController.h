#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ALSAEQAGiantSnakeAIController.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAGiantSnakeState : uint8
{
    Hidden,
    Ambush,
    Chase,
    Attack,
    Coil,
    Retreat,
    Defeated
};

UCLASS()
class ALSAEQA_API AALSAEQAGiantSnakeAIController : public AAIController
{
    GENERATED_BODY()

public:
    AALSAEQAGiantSnakeAIController();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|SnakeAI")
    void SetSnakeState(EALSAEQAGiantSnakeState NewState);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|SnakeAI")
    EALSAEQAGiantSnakeState GetSnakeState() const { return State; }

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|SnakeAI")
    void AcquireTarget(AActor* NewTarget);

protected:
    virtual void Tick(float DeltaSeconds) override;
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|SnakeAI")
    EALSAEQAGiantSnakeState State = EALSAEQAGiantSnakeState::Hidden;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|SnakeAI")
    TObjectPtr<AActor> TargetActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|SnakeAI")
    float DetectionRange = 1800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|SnakeAI")
    float AttackRange = 500.0f;
};
