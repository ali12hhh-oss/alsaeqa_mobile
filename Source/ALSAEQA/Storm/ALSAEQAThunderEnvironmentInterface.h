#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Combat/ALSAEQADamageTypes.h"
#include "ALSAEQAThunderEnvironmentInterface.generated.h"

UINTERFACE(BlueprintType)
class ALSAEQA_API UALSAEQAThunderEnvironmentInterface : public UInterface
{
    GENERATED_BODY()
};

class ALSAEQA_API IALSAEQAThunderEnvironmentInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ALSAEQA|Thunder Environment")
    bool ReceiveThunderInteraction(const FALSAEQADamageInfo& ThunderInfo);
};
