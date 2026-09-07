#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQACompanionFamilyGate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQACompanionFamilyGateChanged, bool, bUnlocked);

/** Keeps the companion family-search arc locked until a deliberately very late stage. */
UCLASS(ClassGroup=(Companions), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQACompanionFamilyGate : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQACompanionFamilyGate();

    /** Stage at which the companion's active family search begins. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Companion Family")
    int32 RequiredStage = 25;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Companion Family")
    bool bFamilyArcUnlocked = false;

    UPROPERTY(BlueprintAssignable, Category="Companion Family")
    FALSAEQACompanionFamilyGateChanged OnFamilyArcGateChanged;

    UFUNCTION(BlueprintCallable, Category="Companion Family")
    bool UpdateForStage(int32 CurrentStage);

    UFUNCTION(BlueprintPure, Category="Companion Family")
    bool IsFamilyArcUnlocked() const { return bFamilyArcUnlocked; }

    UFUNCTION(BlueprintPure, Category="Companion Family")
    int32 GetRequiredStage() const { return RequiredStage; }
};
