#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Storm/ALSAEQAThunderEnvironmentInterface.h"
#include "ALSAEQAThunderEnvironmentComponent.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAThunderReaction : uint8
{
    Illuminate,
    Charge,
    Break,
    Reveal,
    Activate
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALSAEQAThunderEnvironmentChanged, EALSAEQAThunderReaction, Reaction, float, Charge);

UCLASS(ClassGroup=(ALSAEQA), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAThunderEnvironmentComponent : public UActorComponent, public IALSAEQAThunderEnvironmentInterface
{
    GENERATED_BODY()

public:
    UALSAEQAThunderEnvironmentComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Thunder Environment")
    bool ReceiveThunder(const FALSAEQADamageInfo& ThunderInfo);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Thunder Environment")
    void ResetReaction();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Thunder Environment")
    bool IsActivated() const { return bActivated; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Thunder Environment")
    float GetChargePercent() const { return ChargePercent; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Thunder Environment")
    EALSAEQAThunderReaction Reaction = EALSAEQAThunderReaction::Activate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Thunder Environment", meta=(ClampMin="0.1"))
    float RequiredCharge = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Thunder Environment", meta=(ClampMin="0.01"))
    float ChargePerDamage = 0.02f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Thunder Environment")
    bool bAllowRepeatedCharge = false;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Thunder Environment")
    FALSAEQAThunderEnvironmentChanged OnThunderEnvironmentChanged;

    virtual bool ReceiveThunderInteraction_Implementation(const FALSAEQADamageInfo& ThunderInfo) override;

private:
    float ChargePercent = 0.0f;
    bool bActivated = false;
};
