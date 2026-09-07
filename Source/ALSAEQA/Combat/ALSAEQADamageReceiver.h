#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Combat/ALSAEQADamageTypes.h"
#include "ALSAEQADamageReceiver.generated.h"

UINTERFACE(BlueprintType)
class ALSAEQA_API UALSAEQADamageReceiver : public UInterface
{
    GENERATED_BODY()
};

class ALSAEQA_API IALSAEQADamageReceiver
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ALSAEQA|Combat")
    float ReceiveALSAEQADamage(const FALSAEQADamageInfo& DamageInfo);
};
