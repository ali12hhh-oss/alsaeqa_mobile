#pragma once

#include "CoreMinimal.h"
#include "Interaction/ALSAEQAInteractable.h"
#include "ALSAEQAWorkerPrisonerActor.generated.h"

class AALSAEQAWorkerPrisonerActor;

UENUM(BlueprintType)
enum class EALSAEQAWorkerRescueMethod : uint8
{
    BreakChain UMETA(DisplayName="Break Chain"),
    OpenCage UMETA(DisplayName="Open Cage"),
    ReleaseLift UMETA(DisplayName="Release Lift"),
    CutBinding UMETA(DisplayName="Cut Binding"),
    EscortOut UMETA(DisplayName="Escort Out")
};

UENUM(BlueprintType)
enum class EALSAEQAWorkerRescueState : uint8
{
    Captive,
    BeingRescued,
    Escaping,
    Safe
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAWorkerRescuedSignature, AALSAEQAWorkerPrisonerActor*, Worker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAWorkerEscapeStartedSignature, AALSAEQAWorkerPrisonerActor*, Worker);

UCLASS(Blueprintable)
class ALSAEQA_API AALSAEQAWorkerPrisonerActor : public ALSAEQAInteractable
{
    GENERATED_BODY()

public:
    AALSAEQAWorkerPrisonerActor();
    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Rescue") bool Rescue(AActor* Rescuer);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Rescue") void SetSafePoint(FVector NewSafePoint);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Rescue") void NotifyGuardPressure(AActor* Guard);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Rescue") bool IsRescued() const { return bRescued; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Rescue") bool IsRescueInProgress() const { return bRescueInProgress; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Rescue") bool IsSafe() const { return RescueState == EALSAEQAWorkerRescueState::Safe; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Rescue") EALSAEQAWorkerRescueState GetRescueState() const { return RescueState; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Rescue") EALSAEQAWorkerRescueMethod GetRescueMethod() const { return RescueMethod; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Rescue") FName GetRescueSequenceTag() const { return RescueSequenceTag; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Rescue") bool CountsAsStageOneWorker() const { return bCountsAsStageOneWorker; }

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue") FName WorkerId = NAME_None;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue") bool bCountsAsStageOneWorker = true;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue") EALSAEQAWorkerRescueMethod RescueMethod = EALSAEQAWorkerRescueMethod::BreakChain;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue") FName RescueSequenceTag = NAME_None;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue") FVector SafePoint = FVector::ZeroVector;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue", meta=(ClampMin="0.25", UIMin="0.25")) float RescueDuration = 1.8f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue", meta=(ClampMin="50.0", UIMin="50.0")) float RescueThreatRadius = 260.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue", meta=(ClampMin="50.0", UIMin="50.0")) float EscapeSpeed = 240.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue", meta=(ClampMin="50.0", UIMin="50.0")) float SafePointRadius = 90.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ALSAEQA|Rescue", meta=(ClampMin="0.1", UIMin="0.1")) float GuardPressurePause = 0.75f;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Rescue") FALSAEQAWorkerRescuedSignature OnWorkerRescued;
    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Rescue") FALSAEQAWorkerEscapeStartedSignature OnWorkerEscapeStarted;
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Rescue") void PlayRescuePresentation(EALSAEQAWorkerRescueMethod Method, FName SequenceTag);
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Rescue") void PlayRescueInterruptedPresentation();
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Rescue") void PlayWorkerEscapePresentation();
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Rescue") void PlayWorkerSafePresentation();
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Rescue") void PlayWorkerGuardPressurePresentation(AActor* Guard);

protected:
    virtual void BeginPlay() override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Rescue") FText RescuedInteractionPrompt = NSLOCTEXT("ALSAEQA", "WorkerRescuedPrompt", "تم إنقاذه");

private:
    bool IsRescueThreatening() const;
    void FinishRescue();
    void CancelRescue();
    void BeginEscape();
    void UpdateEscape(float DeltaSeconds);
    void CompleteSafeArrival();
    FTimerHandle RescueTimerHandle;
    TWeakObjectPtr<AActor> RescueInstigator;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Rescue", meta=(AllowPrivateAccess="true")) bool bRescued = false;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Rescue", meta=(AllowPrivateAccess="true")) bool bRescueInProgress = false;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Rescue", meta=(AllowPrivateAccess="true")) EALSAEQAWorkerRescueState RescueState = EALSAEQAWorkerRescueState::Captive;
    float GuardPressurePauseRemaining = 0.0f;
};