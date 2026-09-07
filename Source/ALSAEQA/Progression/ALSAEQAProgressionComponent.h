#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAPowerTypes.h"
#include "ALSAEQAProgressionStageTypes.h"
#include "ALSAEQAProgressionComponent.generated.h"

delegate void FALSAEQAProgressionChanged();

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAProgressionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAProgressionComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression")
    bool UnlockPower(EALSAEQAPower Power, FName DiscoveryId);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression")
    bool AcquireWeapon(EALSAEQAWeapon Weapon, FName DiscoveryId);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression")
    bool AdvanceStage(int32 NewStage);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    bool IsValidStage(int32 StageNumber) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    FALSAEQAProgressionStage GetStageDefinition(int32 StageNumber) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    bool HasPower(EALSAEQAPower Power) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    bool HasWeapon(EALSAEQAWeapon Weapon) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    bool IsPowerAvailable(EALSAEQAPower Power) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    bool IsWeaponAvailable(EALSAEQAWeapon Weapon) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    int32 GetPowerRequiredStage(EALSAEQAPower Power) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    int32 GetWeaponRequiredStage(EALSAEQAWeapon Weapon) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    bool HasCompletedDiscovery(FName DiscoveryId) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Progression")
    int32 GetCurrentStage() const { return CurrentStage; }

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Progression")
    void ResetProgression();

    const TSet<EALSAEQAPower>& GetUnlockedPowers() const { return UnlockedPowers; }
    const TSet<EALSAEQAWeapon>& GetAcquiredWeapons() const { return AcquiredWeapons; }

    FALSAEQAProgressionChanged OnProgressionChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Progression")
    int32 CurrentStage = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Progression")
    TSet<EALSAEQAPower> UnlockedPowers;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Progression")
    TSet<EALSAEQAWeapon> AcquiredWeapons;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Progression")
    TSet<FName> CompletedDiscoveryIds;
};
