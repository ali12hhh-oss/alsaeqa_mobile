#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/ALSAEQAEnemyStateTypes.h"
#include "ALSAEQABossComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQABossPhaseChanged, EALSAEQABossPhase, NewPhase);

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQABossComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQABossComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Boss")
    bool SetHealthPercent(float NewPercent);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Boss")
    void SetDefeated();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Boss")
    EALSAEQABossPhase GetPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Boss")
    bool IsDefeated() const { return CurrentPhase == EALSAEQABossPhase::Defeated; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Boss")
    FALSAEQABossPhaseChanged OnPhaseChanged;

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Boss")
    EALSAEQABossPhase CurrentPhase = EALSAEQABossPhase::PhaseOne;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Boss")
    float HealthPercent = 1.0f;
};
