#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Companions/ALSAEQACompanionStoryTypes.h"
#include "Companions/ALSAEQAMountTypes.h"
#include "Save/ALSAEQASaveGame.h"
#include "ALSAEQASaveManager.generated.h"

UCLASS()
class ALSAEQA_API UALSAEQASaveManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Save") bool SaveProgress();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Save") bool LoadProgress();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Save") void SetStage(int32 NewStage);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Save") bool SaveCheckpoint(FName CheckpointId, FName RegionId, int32 Stage, FVector PlayerLocation, FRotator PlayerRotation);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Save") bool RespawnAtLastCheckpoint();
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Save") bool HasSavedJourney() const;
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Save") bool BeginNewJourney();
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Save") int32 GetStage() const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Save") FALSAEQACheckpointData GetLastCheckpoint() const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Save") int32 GetRespawnCount() const;
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Legacy") bool RegisterMemory(FName MemoryId);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Legacy") bool RegisterWorldFlag(FName FlagId);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Legacy") bool HasMemory(FName MemoryId) const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Legacy") bool HasWorldFlag(FName FlagId) const;
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion") bool AddFamilyClue(FName ClueId);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion") bool HasFamilyClue(FName ClueId) const;
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion|Family Search") bool RecordFamilyEvidence(FName EvidenceId);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion|Family Search") bool HasFamilyEvidence(FName EvidenceId) const;
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion|Family Search") bool SetFamilySearchStage(int32 NewStage);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion|Family Search") bool IsFamilySearchActive() const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion|Family Search") int32 GetFamilySearchStage() const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion|Family Search") int32 GetFamilyEvidenceCount() const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion|Family Search") bool IsFamilySearchCompleted() const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion") bool CanBeginFamilySearch() const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion") FALSAEQACompanionStoryProgress GetCompanionStory() const { return SaveData ? SaveData->CompanionStory : FALSAEQACompanionStoryProgress(); }
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion") bool SetCompanionState(EALSAEQACompanionStoryState NewState);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Mounts") bool SaveTamedMount(const FALSAEQAMountProfile& Profile);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mounts") bool HasTamedMount(FName MountId) const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Mounts") FALSAEQAMountProfile GetTamedMount(FName MountId) const;

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Stage1") bool RecordStageOneWorkerRescued(FName WorkerId);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Stage1") bool HasStageOneWorkerRescued(FName WorkerId) const;
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Stage1") bool RecordStageOneSlaverDefeated(FName SlaverId);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Stage1") bool HasStageOneSlaverDefeated(FName SlaverId) const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Stage1") int32 GetStageOneWorkersRescuedCount() const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Stage1") int32 GetStageOneSlaversDefeatedCount() const;

private:
    static const FString SaveSlotName;
    static constexpr int32 FamilySearchStartStage = 25;
    static constexpr int32 FamilySearchEndStage = 40;
    UPROPERTY() UALSAEQASaveGame* SaveData = nullptr;
    bool EnsureSaveData();
};
