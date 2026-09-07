#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Combat/ALSAEQADamageReceiver.h"
#include "Creatures/ALSAEQAGiantSnake.generated.h"

class UALSAEQAHealthComponent;
class UALSAEQAResistanceComponent;
class UALSAEQAStatusComponent;

UCLASS()
class ALSAEQA_API AALSAEQAGiantSnake : public ACharacter, public IALSAEQADamageReceiver
{
    GENERATED_BODY()

public:
    AALSAEQAGiantSnake();

    virtual float ReceiveALSAEQADamage_Implementation(const FALSAEQADamageInfo& DamageInfo) override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Snake")
    void StartAmbush();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Snake")
    void CoilAroundObstacle();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Snake")
    void Retreat();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Snake")
    bool IsDefeated() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Snake")
    TObjectPtr<UALSAEQAHealthComponent> HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Snake")
    TObjectPtr<UALSAEQAResistanceComponent> ResistanceComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Snake")
    TObjectPtr<UALSAEQAStatusComponent> StatusComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Snake")
    float AttackRange = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Snake")
    float BiteDamage = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Snake")
    float ChargeDamage = 55.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Snake")
    float VenomDuration = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Snake")
    float AttackCooldown = 1.8f;

private:
    float AttackTimer = 0.0f;
    bool bUseChargeAttack = false;
};
