#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAThunderChargeComponent.generated.h"

delegate void FALSAEQAThunderChargeChanged(float ChargePercent, bool bFullyCharged);

delegate void FALSAEQAThunderReleased(float ChargePercent);

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAThunderChargeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAThunderChargeComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Thunder")
    bool BeginCharge();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Thunder")
    float ReleaseCharge();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Thunder")
    void CancelCharge();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Thunder")
    bool IsCharging() const { return bCharging; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Thunder")
    float GetChargePercent() const { return ChargePercent; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Thunder")
    float GetDamageMultiplier() const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Thunder")
    bool IsFullyCharged() const { return ChargePercent >= 1.0f; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Thunder")
    float FullChargeTime = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Thunder")
    float MinimumReleasePercent = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Thunder")
    float MaxDamageMultiplier = 3.0f;

    FALSAEQAThunderChargeChanged OnChargeChanged;
    FALSAEQAThunderReleased OnThunderReleased;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    bool bCharging = false;
    float ChargePercent = 0.0f;
};
