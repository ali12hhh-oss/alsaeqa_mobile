#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "ALSAEQADynamicStormSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAStormStateChanged, bool, bActive);

UCLASS()
class ALSAEQA_API UALSAEQADynamicStormSubsystem : public UWorldSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Storm")
    void StartStorm(float DurationSeconds = 60.0f, float StormIntensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Storm")
    void StopStorm();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Storm")
    bool IsStormActive() const { return bStormActive; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Storm")
    float GetIntensity() const { return Intensity; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Storm")
    float GetThunderMultiplier() const;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Storm")
    FALSAEQAStormStateChanged OnStormStateChanged;

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;

private:
    bool bStormActive = false;
    float Intensity = 0.0f;
    float RemainingTime = 0.0f;
};
