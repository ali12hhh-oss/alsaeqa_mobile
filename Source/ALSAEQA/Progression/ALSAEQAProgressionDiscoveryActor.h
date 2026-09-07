#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALSAEQAProgressionDiscoveryTypes.h"
#include "ALSAEQAProgressionDiscoveryActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAProgressionDiscoveryEvent, FName, DiscoveryId);

UCLASS()
class ALSAEQA_API AALSAEQAProgressionDiscoveryActor : public AActor
{
    GENERATED_BODY()

public:
    AALSAEQAProgressionDiscoveryActor();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression")
    bool GrantTo(AActor* PlayerActor);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    bool IsCompletedFor(AActor* PlayerActor) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    const FALSAEQAProgressionDiscovery& GetDiscovery() const { return Discovery; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Progression")
    FALSAEQAProgressionDiscoveryEvent OnDiscoveryGranted;

protected:
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Progression")
    FALSAEQAProgressionDiscovery Discovery;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Progression")
    bool bConsumeAfterGrant = true;
};
