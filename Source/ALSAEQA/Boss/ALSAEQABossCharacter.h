#pragma once

#include "CoreMinimal.h"
#include "AI/ALSAEQAEnemyCharacter.h"
#include "Boss/ALSAEQABossComponent.h"
#include "Boss/ALSAEQABossCharacter.generated.h"

UCLASS()
class ALSAEQA_API AALSAEQABossCharacter : public AALSAEQAEnemyCharacter
{
    GENERATED_BODY()

public:
    AALSAEQABossCharacter();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Boss")
    bool ReceiveBossDamage(const FALSAEQADamageInfo& DamageInfo);

    virtual float ReceiveALSAEQADamage_Implementation(const FALSAEQADamageInfo& DamageInfo) override;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Boss")
    UALSAEQABossComponent* GetBossComponent() const { return BossComponent; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Boss")
    EALSAEQABossPhase GetBossPhase() const { return BossComponent ? BossComponent->GetPhase() : EALSAEQABossPhase::PhaseOne; }

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleBossPhaseChanged(EALSAEQABossPhase NewPhase);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components")
    TObjectPtr<UALSAEQABossComponent> BossComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Boss")
    float BossMaxHealth = 1200.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Boss")
    float BossHealth = 1200.0f;
};
