#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAAbilityComponent.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAAbility : uint8
{
    ThunderShock UMETA(DisplayName="Thunder Shock"),
    LightningDash UMETA(DisplayName="Lightning Dash"),
    ThunderJump UMETA(DisplayName="Thunder Jump"),
    StormFist UMETA(DisplayName="Storm Fist"),
    LightningShield UMETA(DisplayName="Lightning Shield"),
    StormSummon UMETA(DisplayName="Storm Summon"),
    ThunderEye UMETA(DisplayName="Thunder Eye"),
    ThunderEcho UMETA(DisplayName="Thunder Echo")
};

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAAbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAAbilityComponent();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Abilities") bool UnlockAbility(EALSAEQAAbility Ability);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Abilities") bool HasAbility(EALSAEQAAbility Ability) const;
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Abilities") bool TryActivateAbility(EALSAEQAAbility Ability);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Abilities") bool ConsumeEnergy(float Amount);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Abilities") float GetEnergy() const { return Energy; }
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Abilities") void RestoreEnergy(float Amount);

protected:
    virtual void BeginPlay() override;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Energy") float MaxEnergy = 100.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Energy") float Energy = 100.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Energy") TMap<EALSAEQAAbility, float> AbilityCosts;
    UPROPERTY(BlueprintReadOnly, Category="Abilities") TSet<EALSAEQAAbility> UnlockedAbilities;
};
