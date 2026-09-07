#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Companions/ALSAEQACompanionStoryTypes.h"
#include "ALSAEQACompanionCharacter.generated.h"

class UALSAEQAHealthComponent;
class UALSAEQACompanionStoryComponent;
class UALSAEQACompanionCaptureComponent;
class UALSAEQARidingComponent;
class UALSAEQAVisualAssetComponent;
class AALSAEQAMountActor;

UENUM(BlueprintType)
enum class EALSAEQACompanionBehaviorState : uint8
{
    Following,
    Assisting,
    Separated,
    Captured,
    Rescued
};

UCLASS(BlueprintType, Blueprintable)
class ALSAEQA_API AALSAEQACompanionCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AALSAEQACompanionCharacter();
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion") void SetBehaviorState(EALSAEQACompanionBehaviorState NewState);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion") void SetCompanionId(FName NewCompanionId);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion") bool CaptureCompanion();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion") bool MarkRescueKnown();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion") bool RescueCompanion();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion") bool BeginFamilySearch();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion|Hero Search") bool BeginHelpingHeroFamilySearch();
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion|Hero Search") bool IsHelpingHeroFamilySearch() const { return bHelpingHeroFamilySearch; }
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void ToggleCrouch();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void StartListen();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void StopListen();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void PerformRoll();
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Movement") bool IsListening() const { return bListening; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Movement") bool IsRolling() const { return bRolling; }
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Movement") void PlayMovementPresentation(FName MovementEvent);
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Movement") void PlayRollPresentation();
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Movement") void PlayListenPresentation(bool bStarted);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding") bool Mount(AALSAEQAMountActor* MountActor);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding") bool Dismount();
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion") FName GetCompanionId() const { return CompanionId; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion") EALSAEQACompanionBehaviorState GetBehaviorState() const { return BehaviorState; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion") UALSAEQACompanionStoryComponent* GetStoryComponent() const { return StoryComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion") UALSAEQACompanionCaptureComponent* GetCaptureComponent() const { return CaptureComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion") UALSAEQARidingComponent* GetRidingComponent() const { return RidingComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Visual") UALSAEQAVisualAssetComponent* GetVisualAssetComponent() const { return VisualAssetComponent; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Companion") TObjectPtr<UALSAEQAHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Companion") TObjectPtr<UALSAEQACompanionStoryComponent> StoryComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Companion") TObjectPtr<UALSAEQACompanionCaptureComponent> CaptureComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Companion") TObjectPtr<UALSAEQARidingComponent> RidingComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Visual") TObjectPtr<UALSAEQAVisualAssetComponent> VisualAssetComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="ALSAEQA|Companion") FName CompanionId = TEXT("PermanentCompanion");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Companion") float FollowDistance = 220.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Companion") float AssistRange = 900.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Companion") float FollowStartDistance = 320.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Companion") float FollowStopDistance = 140.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Companion") float CompanionWalkSpeed = 330.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Companion") float CompanionSprintSpeed = 560.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="ALSAEQA|Companion|Hero Search") bool bHelpingHeroFamilySearch = false;
    UPROPERTY(BlueprintReadOnly, Category="ALSAEQA|Companion") EALSAEQACompanionBehaviorState BehaviorState = EALSAEQACompanionBehaviorState::Following;

private:
    void UpdateFollowMovement(float DeltaSeconds);
    void FinishRoll();
    FTimerHandle RollTimerHandle;
    FTimerHandle RollCooldownTimerHandle;
    bool bListening = false;
    bool bRolling = false;
    bool bRollReady = true;
};
