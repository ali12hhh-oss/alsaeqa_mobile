#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAStatusComponent.generated.h"

class UALSAEQAHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALSAEQAStatusChanged, FName, StatusId, bool, bActive);

USTRUCT(BlueprintType)
struct ALSAEQA_API FALSAEQAStatusEffect
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName StatusId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RemainingTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamagePerSecond = 0.0f;
};

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAStatusComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAStatusComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Status")
    void ApplyPoison(float DamagePerSecond, float Duration);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Status")
    void ClearStatus(FName StatusId);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Status")
    bool HasStatus(FName StatusId) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Status")
    float GetRemainingTime(FName StatusId) const;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Status")
    FALSAEQAStatusChanged OnStatusChanged;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Status", SaveGame)
    TArray<FALSAEQAStatusEffect> ActiveStatuses;

private:
    UPROPERTY()
    TObjectPtr<UALSAEQAHealthComponent> HealthComponent;

    float PoisonTickAccumulator = 0.0f;
    static constexpr float PoisonTickInterval = 0.1f;
};
