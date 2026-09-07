#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQACombatComponent.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAAttackType : uint8
{
    Light,
    Heavy
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAAttackStarted, EALSAEQAAttackType, AttackType);

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQACombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQACombatComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Combat")
    bool StartAttack(EALSAEQAAttackType AttackType);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Combat")
    void EndAttack();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Combat")
    bool IsAttacking() const { return bAttacking; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Combat")
    EALSAEQAAttackType GetCurrentAttack() const { return CurrentAttack; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Combat")
    FALSAEQAAttackStarted OnAttackStarted;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Combat")
    float LightAttackDamage = 18.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Combat")
    float HeavyAttackDamage = 42.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Combat")
    float AttackCooldown = 0.25f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Combat")
    bool bAttacking = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Combat")
    EALSAEQAAttackType CurrentAttack = EALSAEQAAttackType::Light;

    float LastAttackTime = -1000.0f;
};
