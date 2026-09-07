#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/ALSAEQADamageTypes.h"
#include "ALSAEQAHealthComponent.generated.h"

class UALSAEQAInjuryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALSAEQAHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FALSAEQADeath);

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAHealthComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Health")
    void ApplyDamage(float Damage);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Health|Combat")
    void ApplyDamageInfo(const FALSAEQADamageInfo& DamageInfo);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Health")
    void Heal(float Amount);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Health")
    void ResetHealth();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Health")
    bool IsDead() const { return bDead; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Health")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Injury")
    UALSAEQAInjuryComponent* GetInjuryComponent() const { return InjuryComponent; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Health")
    FALSAEQAHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Health")
    FALSAEQADeath OnDeath;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Health")
    float Health = 100.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Health")
    bool bDead = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Injury")
    TObjectPtr<UALSAEQAInjuryComponent> InjuryComponent;

private:
    void ApplyInjuryFromDamage(const FALSAEQADamageInfo& DamageInfo);

    UFUNCTION()
    void HandleInjuryDeath();
};
