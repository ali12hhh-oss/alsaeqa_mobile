#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQALegacyComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQALegacyMemoryDiscovered, FName, MemoryId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQALegacyFlagChanged, FName, FlagId);

UCLASS(ClassGroup=(ALSAEQA), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQALegacyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQALegacyComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Legacy")
    bool DiscoverMemory(FName MemoryId);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Legacy")
    bool AddFamilyClue(FName ClueId);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Legacy")
    void SetLegacyFlag(FName FlagId, bool bEnabled = true);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Legacy")
    bool HasMemory(FName MemoryId) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Legacy")
    bool HasFamilyClue(FName ClueId) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Legacy")
    bool HasLegacyFlag(FName FlagId) const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Legacy")
    bool CanBeginCompanionFamilySearch() const;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Legacy")
    void SetCurrentStage(int32 Stage);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Legacy")
    int32 GetCurrentStage() const { return CurrentStage; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Legacy")
    int32 GetFamilyClueCount() const { return FamilyClueIds.Num(); }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Legacy")
    FALSAEQALegacyMemoryDiscovered OnMemoryDiscovered;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Legacy")
    FALSAEQALegacyFlagChanged OnLegacyFlagChanged;

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Legacy")
    TArray<FName> DiscoveredMemoryIds;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Legacy")
    TArray<FName> FamilyClueIds;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Legacy")
    TArray<FName> LegacyFlags;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="ALSAEQA|Legacy")
    int32 CurrentStage = 1;
};
