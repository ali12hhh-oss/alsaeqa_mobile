#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/ALSAEQADamageTypes.h"
#include "ALSAEQAMeleeCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAAttackEvent, bool, bHeavy);

UCLASS(ClassGroup=(ALSAEQA), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAMeleeCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAMeleeCombatComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Combat")
    bool LightAttack();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Combat")
    bool HeavyAttack();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Combat")
    bool TryHitActor(AActor* Target, float Damage, EALSAEQADamageType DamageType);

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Combat")
    FALSAEQAAttackEvent OnAttackStarted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Combat")
    float LightDamage = 18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Combat")
    float HeavyDamage = 42.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Combat")
    float AttackCooldown = 0.35f;

protected:
    virtual void BeginPlay() override;

private:
    bool CanAttack() const;
    float LastAttackTime = -1000.0f;
};
