#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQACompanionCaptureComponent.generated.h"

UENUM(BlueprintType)
enum class EALSAEQACompanionCaptureState : uint8
{
    Free,
    Captured,
    RescueKnown,
    Rescued
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQACompanionCaptureStateChanged, EALSAEQACompanionCaptureState, NewState);

UCLASS(ClassGroup=(ALSAEQA), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQACompanionCaptureComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQACompanionCaptureComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion Rescue")
    bool Capture();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion Rescue")
    bool MarkRescueKnown();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion Rescue")
    bool Rescue();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion Rescue")
    void ResetToFree();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion Rescue")
    bool IsCaptured() const { return State == EALSAEQACompanionCaptureState::Captured || State == EALSAEQACompanionCaptureState::RescueKnown; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion Rescue")
    bool IsRescueKnown() const { return State == EALSAEQACompanionCaptureState::RescueKnown; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion Rescue")
    bool IsRescued() const { return State == EALSAEQACompanionCaptureState::Rescued; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion Rescue")
    EALSAEQACompanionCaptureState GetState() const { return State; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Companion Rescue")
    FALSAEQACompanionCaptureStateChanged OnStateChanged;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Companion Rescue", meta=(AllowPrivateAccess="true"))
    EALSAEQACompanionCaptureState State = EALSAEQACompanionCaptureState::Free;
};
