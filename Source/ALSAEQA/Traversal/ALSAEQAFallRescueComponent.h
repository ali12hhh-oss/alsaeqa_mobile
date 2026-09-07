#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAFallRescueComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FALSAEQAFallEvent);

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAFallRescueComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAFallRescueComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Traversal")
    void BeginFall();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Traversal")
    bool TryRescue(const FVector& RescueLocation);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Traversal")
    void CancelFall();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Traversal")
    bool IsFalling() const { return bFalling; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Traversal")
    FALSAEQAFallEvent OnFallStarted;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Traversal")
    FALSAEQAFallEvent OnRescued;

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Traversal")
    bool bFalling = false;
};
