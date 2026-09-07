#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Systems/ALSAEQAAbilityComponent.h"
#include "Companions/ALSAEQAMountAbilityComponent.h"
#include "Combat/ALSAEQAMeleeCombatComponent.h"
#include "ALSAEQACharacter.generated.h"

class UALSAEQAHealthComponent;
class UALSAEQAThunderChargeComponent;
class UALSAEQALegacyComponent;
class UALSAEQARidingComponent;
class UALSAEQAVisualAssetComponent;
class UALSAEQACinematicDirector;
class UALSAEQAProgressionComponent;
class UALSAEQAStageFlowComponent;
class UALSAEQAStageObjectiveComponent;
class UALSAEQAStage1MineResponseCoordinator;
class USpringArmComponent;
class UCameraComponent;
class AALSAEQAMountActor;

UCLASS()
class ALSAEQA_API AALSAEQACharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AALSAEQACharacter();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Combat") void PerformLightAttack();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Combat") void PerformHeavyAttack();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Thunder") void BeginThunderCharge();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Thunder") void ReleaseThunderCharge();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Thunder") void CancelThunderCharge();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void StartSprint();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void StopSprint();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void ToggleCrouch();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void StartListen();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void StopListen();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Movement") void PerformRoll();
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Movement") bool IsListening() const { return bListening; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Movement") bool IsRolling() const { return bRolling; }
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Movement") void PlayMovementPresentation(FName MovementEvent);
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Movement") void PlayRollPresentation();
    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Movement") void PlayListenPresentation(bool bStarted);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Interaction") bool InteractWithNearest();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding") bool MountOrDismount();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding") bool MountNearestTamedMount();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding") bool DismountCurrentMount();
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Riding") bool IsRiding() const;
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Abilities") bool ActivateAbility(EALSAEQAAbility Ability);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|MountAbility") bool ActivateMountAbility(EALSAEQAMountAbility Ability);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Riding") UALSAEQARidingComponent* GetRidingComponent() const { return RidingComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Abilities") UALSAEQAAbilityComponent* GetAbilityComponent() const { return AbilityComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Combat") UALSAEQAMeleeCombatComponent* GetMeleeCombatComponent() const { return MeleeCombatComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Health") UALSAEQAHealthComponent* GetHealthComponent() const { return HealthComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Thunder") UALSAEQAThunderChargeComponent* GetThunderChargeComponent() const { return ThunderChargeComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Legacy") UALSAEQALegacyComponent* GetLegacyComponent() const { return LegacyComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Visual") UALSAEQAVisualAssetComponent* GetVisualAssetComponent() const { return VisualAssetComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Cinematics") UALSAEQACinematicDirector* GetCinematicDirector() const { return CinematicDirector; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression") UALSAEQAProgressionComponent* GetProgressionComponent() const { return ProgressionComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Flow") UALSAEQAStageFlowComponent* GetStageFlowComponent() const { return StageFlowComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression|Objectives") UALSAEQAStageObjectiveComponent* GetStageObjectiveComponent() const { return StageObjectiveComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Stage1") UALSAEQAStage1MineResponseCoordinator* GetStage1MineResponseCoordinator() const { return Stage1MineResponseCoordinator; }

protected:
    virtual void BeginPlay() override;
    void MoveForward(float Value);
    void MoveRight(float Value);
    void LookUp(float Value);
    void Turn(float Value);
    void HandleMountInput();
    void ActivateMountLightningDash();
    void ActivateMountThunderRoar();
    void ActivateMountLightningKick();
    void ActivateMountStormCharge();
    void ActivateMountLightningCrossing();
    void ActivateMountStormLeap();
    void ActivateMountLightningShield();
    void ActivateMountStormSummon();
    void ActivateMountThunderSense();
    void ActivateMountStormMode();
    void FinishRoll();

    UFUNCTION()
    void HandlePlayerDeath();

    void RespawnAtCheckpoint();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UCameraComponent> FollowCamera;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAAbilityComponent> AbilityComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAMeleeCombatComponent> MeleeCombatComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAThunderChargeComponent> ThunderChargeComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQALegacyComponent> LegacyComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQARidingComponent> RidingComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAVisualAssetComponent> VisualAssetComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQACinematicDirector> CinematicDirector;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAProgressionComponent> ProgressionComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAStageFlowComponent> StageFlowComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAStageObjectiveComponent> StageObjectiveComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Components") TObjectPtr<UALSAEQAStage1MineResponseCoordinator> Stage1MineResponseCoordinator;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Movement") float WalkSpeed = 360.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Movement") float SprintSpeed = 620.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Movement") float CrouchSpeed = 180.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Movement") float RollStrength = 720.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Movement") float RollDuration = 0.42f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Movement") float RollCooldown = 0.65f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Movement") float ListenTurnRateScale = 0.35f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Interaction") float InteractionRange = 240.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Riding") float MountSearchRadius = 450.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Combat") float MeleeAttackRange = 180.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Combat") float MeleeAttackRadius = 90.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Thunder") float ThunderReleaseDamage = 30.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Thunder") float ThunderAttackRange = 650.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Thunder") float ThunderAttackRadius = 90.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Death") float DeathRespawnDelay = 1.75f;

private:
    int32 ApplyThunderReleaseToTargets(float Damage);
    bool PerformMeleeStrike(bool bHeavy);

    FTimerHandle RespawnTimerHandle;
    FTimerHandle RollTimerHandle;
    FTimerHandle RollCooldownTimerHandle;
    bool bDeathInProgress = false;
    bool bListening = false;
    bool bRolling = false;
    bool bRollReady = true;
};