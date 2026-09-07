#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALSAEQAMountTypes.h"
#include "ALSAEQAMountActor.generated.h"

class UALSAEQAMountComponent;
class UALSAEQAMountAbilityComponent;

UCLASS(BlueprintType, Blueprintable)
class ALSAEQA_API AALSAEQAMountActor : public ACharacter
{
    GENERATED_BODY()

public:
    AALSAEQAMountActor();
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool InitializeWildMount(const FALSAEQAMountProfile& Profile);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool BeginTaming();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool AddTamingProgress(float Amount);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool Tame(const FALSAEQAMountProfile& Profile);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool MountRider(AActor* Rider);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool DismountRider();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool InjureMount();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool ReleaseMount();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool MoveRiderForward(float Value);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool MoveRiderRight(float Value);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount") bool SetRiderSprint(bool bEnabled);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount") bool HasRider() const { return Rider.IsValid(); }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount") bool IsRiderSprinting() const { return bRiderSprinting; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount") UALSAEQAMountComponent* GetMountComponent() const { return MountComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount") UALSAEQAMountAbilityComponent* GetMountAbilityComponent() const { return MountAbilityComponent; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount") AActor* GetRider() const { return Rider.Get(); }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Mount") TObjectPtr<UALSAEQAMountComponent> MountComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ALSAEQA|Mount") TObjectPtr<UALSAEQAMountAbilityComponent> MountAbilityComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Mount") FVector RiderOffset = FVector(0.0f, 0.0f, 90.0f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Mount") float SprintSpeedMultiplier = 1.35f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Mount") float StaminaPerSecondWhileSprinting = 8.0f;
    UPROPERTY(BlueprintReadOnly, Category="ALSAEQA|Mount") TWeakObjectPtr<AActor> Rider;
    UPROPERTY(BlueprintReadOnly, Category="ALSAEQA|Mount") bool bRiderSprinting = false;
    float BaseMovementSpeed = 650.0f;
};
