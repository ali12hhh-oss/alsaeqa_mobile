#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAMountTypes.h"
#include "ALSAEQAMountComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAMountStateChanged, EALSAEQAMountState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALSAEQAMountChanged, FName, MountId, bool, bMounted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAMountProgressChanged, float, Progress);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAMountComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAMountComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool InitializeWildMount(const FALSAEQAMountProfile& Profile);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool BeginTaming();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool AddTamingProgress(float Amount);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool TameMount(const FALSAEQAMountProfile& Profile);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool Mount();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool Dismount();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool InjureMount();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool HealMount(float Amount = 100.0f);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool RestoreStamina(float Amount = 100.0f);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool ConsumeStamina(float Amount);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mount")
    bool ReleaseMount();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount")
    bool HasTamedMount() const { return MountState == EALSAEQAMountState::Tamed || MountState == EALSAEQAMountState::Mounted; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount")
    bool IsMounted() const { return MountState == EALSAEQAMountState::Mounted; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount")
    bool IsInjured() const { return MountState == EALSAEQAMountState::Injured; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount")
    bool IsTaming() const { return MountState == EALSAEQAMountState::Taming; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount")
    EALSAEQAMountState GetState() const { return MountState; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount")
    FALSAEQAMountProfile GetMountProfile() const { return MountProfile; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount")
    float GetTamingProgress() const { return MountProfile.TamingProgress; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mount")
    float GetStamina() const { return MountProfile.Stamina; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Mount")
    FALSAEQAMountStateChanged OnStateChanged;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Mount")
    FALSAEQAMountChanged OnMountChanged;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Mount")
    FALSAEQAMountProgressChanged OnTamingProgressChanged;

private:
    void SetState(EALSAEQAMountState NewState);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Mount", meta=(AllowPrivateAccess="true"))
    EALSAEQAMountState MountState = EALSAEQAMountState::Wild;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Mount", meta=(AllowPrivateAccess="true"))
    FALSAEQAMountProfile MountProfile;
};
