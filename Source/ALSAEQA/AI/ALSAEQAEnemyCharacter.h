#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Combat/ALSAEQADamageReceiver.h"
#include "AI/ALSAEQAEnemyCharacter.generated.h"

class UALSAEQAHealthComponent;
class UALSAEQAVisualAssetComponent;
class AALSAEQAWorkerPrisonerActor;

UENUM(BlueprintType)
enum class EALSAEQAEnemyState : uint8
{
    Idle,
    Patrol,
    Alert,
    Chase,
    Attack,
    Stunned,
    Dead
};

UCLASS()
class ALSAEQA_API AALSAEQAEnemyCharacter : public ACharacter, public IALSAEQADamageReceiver
{
    GENERATED_BODY()

public:
    AALSAEQAEnemyCharacter();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|AI") void SetEnemyState(EALSAEQAEnemyState NewState);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|AI") EALSAEQAEnemyState GetEnemyState() const { return EnemyState; }
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|AI") void SetTargetActor(AActor* NewTarget);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|AI") AActor* GetTargetActor() const { return TargetActor.Get(); }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Health") UALSAEQAHealthComponent* GetHealthComponent() const { return HealthComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Visual") UALSAEQAVisualAssetComponent* GetVisualAssetComponent() const { return VisualAssetComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Stage1") bool IsStageOneMineGuard() const { return bStageOneMineGuard; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Stage1") bool CountsAsStageOneSlaver() const { return bCountsAsStageOneSlaver; }
    virtual float ReceiveALSAEQADamage_Implementation(const FALSAEQADamageInfo& DamageInfo) override;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    UFUNCTION() void HandleDeath();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAVisualAssetComponent> VisualAssetComponent;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|AI") EALSAEQAEnemyState EnemyState = EALSAEQAEnemyState::Idle;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|AI") TWeakObjectPtr<AActor> TargetActor;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|AI") float ChaseSpeed = 430.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|AI") float DetectionRange = 1600.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Combat") float AttackRange = 180.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Combat") float AttackDamage = 12.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Combat") float AttackCooldown = 1.25f;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="ALSAEQA|Progression|Stage1") FName StageOneSlaverId = NAME_None;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="ALSAEQA|Progression|Stage1") bool bCountsAsStageOneSlaver = false;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="ALSAEQA|Progression|Stage1") bool bStageOneMineGuard = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Progression|Stage1", meta=(ClampMin="100.0", UIMin="100.0")) float WorkerThreatRange = 1100.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Progression|Stage1", meta=(ClampMin="80.0", UIMin="80.0")) float WorkerAttackRange = 150.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Progression|Stage1", meta=(ClampMin="0.25", UIMin="0.25")) float WorkerAttackCooldown = 1.75f;

private:
    float AttackCooldownRemaining = 0.0f;
    float WorkerAttackCooldownRemaining = 0.0f;
    bool bStageObjectiveReported = false;
};