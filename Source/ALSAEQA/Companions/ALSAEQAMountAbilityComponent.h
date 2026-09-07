#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAMountAbilityComponent.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAMountAbility : uint8
{
    LightningDash,
    ThunderRoar,
    LightningKick,
    StormCharge,
    LightningCrossing,
    StormLeap,
    LightningShield,
    StormSummon,
    ThunderSense,
    StormMode
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALSAEQAMountAbilityActivated, EALSAEQAMountAbility, Ability, float, Power);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAMountAbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAMountAbilityComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|MountAbility")
    bool TryActivate(EALSAEQAMountAbility Ability);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|MountAbility")
    bool CanActivate(EALSAEQAMountAbility Ability) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|MountAbility")
    bool IsUnlocked(EALSAEQAMountAbility Ability) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|MountAbility")
    float GetCooldownRemaining(EALSAEQAMountAbility Ability) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|MountAbility")
    bool IsStormModeActive() const { return bStormModeActive; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|MountAbility")
    bool IsLightningShieldActive() const { return bLightningShieldActive; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|MountAbility")
    FALSAEQAMountAbilityActivated OnAbilityActivated;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    bool IsThunderBeast() const;
    int32 GetCurrentStage() const;
    bool ExecuteAbility(EALSAEQAMountAbility Ability, float& OutPower);
    bool ExecuteThunderRoar(float Power);
    bool ExecuteLightningKick(float Power);
    bool ExecuteStormSummon(float Power);
    void SetCooldown(EALSAEQAMountAbility Ability, float Seconds);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|MountAbility", meta=(AllowPrivateAccess="true"))
    float LightningDashDistance = 1100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|MountAbility", meta=(AllowPrivateAccess="true"))
    float ThunderRoarRadius = 650.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|MountAbility", meta=(AllowPrivateAccess="true"))
    float ThunderRoarDamage = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|MountAbility", meta=(AllowPrivateAccess="true"))
    float LightningKickDamage = 55.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|MountAbility", meta=(AllowPrivateAccess="true"))
    float StormSummonDamage = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|MountAbility", meta=(AllowPrivateAccess="true"))
    float ShieldDuration = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|MountAbility", meta=(AllowPrivateAccess="true"))
    float StormModeDuration = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|MountAbility", meta=(AllowPrivateAccess="true"))
    float StormModeMultiplier = 1.5f;

    TMap<EALSAEQAMountAbility, float> Cooldowns;
    float LightningShieldRemaining = 0.0f;
    float StormModeRemaining = 0.0f;
    bool bLightningShieldActive = false;
    bool bStormModeActive = false;
};
