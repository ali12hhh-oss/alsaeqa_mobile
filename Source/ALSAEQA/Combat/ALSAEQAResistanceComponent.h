#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQADamageTypes.h"
#include "ALSAEQAResistanceComponent.generated.h"

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAResistanceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAResistanceComponent();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Combat|Resistance")
    float GetMultiplier(EALSAEQADamageType DamageType) const;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Combat|Resistance")
    float ModifyDamage(const FALSAEQADamageInfo& DamageInfo) const;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Combat|Resistance")
    void SetMultiplier(EALSAEQADamageType DamageType, float Multiplier);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resistance", meta=(ClampMin="0.0", ClampMax="2.0"))
    TMap<EALSAEQADamageType, float> DamageMultipliers;
};
