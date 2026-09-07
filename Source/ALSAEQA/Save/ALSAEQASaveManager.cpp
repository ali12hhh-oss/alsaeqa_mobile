#include "Save/ALSAEQASaveManager.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

const FString UALSAEQAStageOneWorkerPrefix(TEXT("Stage1_Worker_"));
const FString UALSAEQAStageOneSlaverPrefix(TEXT("Stage1_Slaver_"));

const FString UALSAEQASaveManager::SaveSlotName(TEXT("ALSAEQA_Main"));

bool UALSAEQASaveManager::EnsureSaveData()
{
    if (SaveData) return true;
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
        SaveData = Cast<UALSAEQASaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
    if (!SaveData)
        SaveData = Cast<UALSAEQASaveGame>(UGameplayStatics::CreateSaveGameObject(UALSAEQASaveGame::StaticClass()));
    return SaveData != nullptr;
}

bool UALSAEQASaveManager::SaveProgress() { return EnsureSaveData() && UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0); }
bool UALSAEQASaveManager::LoadProgress() { SaveData = nullptr; return EnsureSaveData(); }
bool UALSAEQASaveManager::HasSavedJourney() const { return UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0); }

bool UALSAEQASaveManager::BeginNewJourney()
{
    if (!UGameplayStatics::DeleteGameInSlot(SaveSlotName, 0))
    {
        if (HasSavedJourney()) return false;
    }
    SaveData = Cast<UALSAEQASaveGame>(UGameplayStatics::CreateSaveGameObject(UALSAEQASaveGame::StaticClass()));
    if (!SaveData) return false;
    return SaveProgress();
}

void UALSAEQASaveManager::SetStage(int32 NewStage)
{
    if (!EnsureSaveData()) return;
    const int32 SafeStage = FMath::Max(1, NewStage);
    SaveData->LastCheckpoint.Stage = SafeStage;
    if (SafeStage >= FamilySearchStartStage)
        SaveData->CompanionStory.FamilySearchStage = FMath::Max(SaveData->CompanionStory.FamilySearchStage, FMath::Min(SafeStage, FamilySearchEndStage));
    if (SafeStage >= FamilySearchEndStage)
    {
        SaveData->CompanionStory.FamilySearchStage = FamilySearchEndStage;
        SaveData->CompanionStory.bFamilySearchCompleted = true;
    }
    SaveProgress();
}

bool UALSAEQASaveManager::SaveCheckpoint(FName CheckpointId, FName RegionId, int32 Stage, FVector PlayerLocation, FRotator PlayerRotation)
{
    if (!EnsureSaveData() || CheckpointId.IsNone()) return false;
    const int32 SafeStage = FMath::Max(1, Stage);
    SaveData->LastCheckpoint.CheckpointId = CheckpointId;
    SaveData->LastCheckpoint.RegionId = RegionId;
    SaveData->LastCheckpoint.Stage = SafeStage;
    SaveData->LastCheckpoint.PlayerLocation = PlayerLocation;
    SaveData->LastCheckpoint.PlayerRotation = PlayerRotation;
    SaveData->LastCheckpoint.RespawnCount = 0;
    if (SafeStage >= FamilySearchStartStage)
        SaveData->CompanionStory.FamilySearchStage = FMath::Max(SaveData->CompanionStory.FamilySearchStage, FMath::Min(SafeStage, FamilySearchEndStage));
    if (SafeStage >= FamilySearchEndStage)
    {
        SaveData->CompanionStory.FamilySearchStage = FamilySearchEndStage;
        SaveData->CompanionStory.bFamilySearchCompleted = true;
    }
    return SaveProgress();
}

bool UALSAEQSAStageOneSaveManagerPlaceholder(){ return true; }

bool UALSAEQASaveManager::RespawnAtLastCheckpoint()
{
    if (!EnsureSaveData() || SaveData->LastCheckpoint.CheckpointId.IsNone()) return false;
    SaveData->LastCheckpoint.RespawnCount++;
    SaveProgress();
    return true;
}

int32 UALSAEQASaveManager::GetStage() const { return SaveData ? FMath::Max(1, SaveData->LastCheckpoint.Stage) : 1; }
FALSAEQACheckpointData UALSAEQASaveManager::GetLastCheckpoint() const { return SaveData ? SaveData->LastCheckpoint : FALSAEQACheckpointData(); }
int32 UALSAEQASaveManager::GetRespawnCount() const { return SaveData ? SaveData->LastCheckpoint.RespawnCount : 0; }

bool UALSAEQASaveManager::RegisterMemory(FName MemoryId) { if (!EnsureSaveData() || MemoryId.IsNone() || SaveData->DiscoveredMemoryIds.Contains(MemoryId)) return false; SaveData->DiscoveredMemoryIds.Add(MemoryId); return true; }
bool UALSAEQASaveManager::RegisterWorldFlag(FName FlagId) { if (!EnsureSaveData() || FlagId.IsNone() || SaveData->PersistentWorldFlags.Contains(FlagId)) return false; SaveData->PersistentWorldFlags.Add(FlagId); return true; }
bool UALSAEQASaveManager::HasMemory(FName MemoryId) const { return SaveData && !MemoryId.IsNone() && SaveData->DiscoveredMemoryIds.Contains(MemoryId); }
bool UALSAEQASaveManager::HasWorldFlag(FName FlagId) const { return SaveData && !FlagId.IsNone() && SaveData->PersistentWorldFlags.Contains(FlagId); }

bool UALSAEQASaveManager::AddFamilyClue(FName ClueId) { if (!EnsureSaveData() || ClueId.IsNone() || SaveData->CompanionStory.FamilyClueIds.Contains(ClueId)) return false; SaveData->CompanionStory.FamilyClueIds.Add(ClueId); SaveData->CompanionStory.FamilyClueCount = SaveData->CompanionStory.FamilyClueIds.Num(); if (SaveData->CompanionStory.State < EALSAEQACompanionStoryState::FamilyClue) SaveData->CompanionStory.State = EALSAEQACompanionStoryState::FamilyClue; return true; }
bool UALSAEQASaveManager::HasFamilyClue(FName ClueId) const { return SaveData && !ClueId.IsNone() && SaveData->CompanionStory.FamilyClueIds.Contains(ClueId); }
bool UALSAEQASaveManager::RecordFamilyEvidence(FName EvidenceId) { if (!EnsureSaveData() || EvidenceId.IsNone() || !IsFamilySearchActive() || SaveData->CompanionStory.FamilyEvidenceIds.Contains(EvidenceId)) return false; SaveData->CompanionStory.FamilyEvidenceIds.Add(EvidenceId); SaveData->CompanionStory.FamilyEvidenceCount = SaveData->CompanionStory.FamilyEvidenceIds.Num(); SaveData->CompanionStory.State = EALSAEQACompanionStoryState::FamilyClue; return SaveProgress(); }
bool UALSAEQASaveManager::HasFamilyEvidence(FName EvidenceId) const { return SaveData && !EvidenceId.IsNone() && SaveData->CompanionStory.FamilyEvidenceIds.Contains(EvidenceId); }
bool UALSAEQASaveManager::SetFamilySearchStage(int32 NewStage) { if (!EnsureSaveData() || NewStage < FamilySearchStartStage || NewStage > FamilySearchEndStage || NewStage < SaveData->CompanionStory.FamilySearchStage) return false; SaveData->CompanionStory.FamilySearchStage = NewStage; if (NewStage >= FamilySearchEndStage) SaveData->CompanionStory.bFamilySearchCompleted = true; return SaveProgress(); }
bool UALSAEQASaveManager::IsFamilySearchActive() const { if (!SaveData) return false; const int32 Stage = SaveData->LastCheckpoint.Stage; return Stage >= FamilySearchStartStage && Stage <= FamilySearchEndStage; }
int32 UALSAEQASaveManager::GetFamilySearchStage() const { return SaveData ? SaveData->CompanionStory.FamilySearchStage : 0; }
int32 UALSAEQASaveManager::GetFamilyEvidenceCount() const { return SaveData ? SaveData->CompanionStory.FamilyEvidenceCount : 0; }
bool UALSAEQASaveManager::IsFamilySearchCompleted() const { return SaveData && SaveData->CompanionStory.bFamilySearchCompleted; }
bool UALSAEQASaveManager::CanBeginFamilySearch() const { return SaveData && SaveData->LastCheckpoint.Stage >= FamilySearchStartStage; }
bool UALSAEQASaveManager::SetCompanionState(EALSAEQACompanionStoryState NewState) { if (!EnsureSaveData()) return false; if (NewState == EALSAEQACompanionStoryState::FamilyRevelation && !CanBeginFamilySearch()) return false; SaveData->CompanionStory.State = NewState; return true; }

bool UALSAEQASaveManager::SaveTamedMount(const FALSAEQAMountProfile& Profile)
{
    if (!EnsureSaveData() || Profile.MountId.IsNone() || Profile.TamingProgress < Profile.TamingRequired) return false;
    const int32 Existing = SaveData->TamedMountProfiles.IndexOfByPredicate([&Profile](const FALSAEQAMountProfile& Item){ return Item.MountId == Profile.MountId; });
    if (Existing == INDEX_NONE) SaveData->TamedMountProfiles.Add(Profile); else SaveData->TamedMountProfiles[Existing] = Profile;
    const int32 Index = Existing == INDEX_NONE ? SaveData->TamedMountProfiles.Num() - 1 : Existing;
    SaveData->TamedMountProfiles[Index].TamingProgress = FMath::Max(Profile.TamingProgress, Profile.TamingRequired);
    return SaveProgress();
}

bool UALSAEQASaveManager::HasTamedMount(FName MountId) const { return SaveData && !MountId.IsNone() && SaveData->TamedMountProfiles.ContainsByPredicate([MountId](const FALSAEQAMountProfile& Profile){ return Profile.MountId == MountId; }); }
FALSAEQAMountProfile UALSAEQASaveManager::GetTamedMount(FName MountId) const { if (SaveData && !MountId.IsNone()) { const FALSAEQAMountProfile* Found = SaveData->TamedMountProfiles.FindByPredicate([MountId](const FALSAEQAMountProfile& Profile){ return Profile.MountId == MountId; }); if (Found) return *Found; } return FALSAEQAMountProfile(); }

bool UALSAEQASaveManager::RecordStageOneWorkerRescued(FName WorkerId)
{
    if (!EnsureSaveData() || WorkerId.IsNone() || SaveData->RescuedStageOneWorkerIds.Contains(WorkerId)) return false;
    SaveData->RescuedStageOneWorkerIds.Add(WorkerId);
    return SaveProgress();
}

bool UALSAEQASaveManager::HasStageOneWorkerRescued(FName WorkerId) const
{
    return SaveData && !WorkerId.IsNone() && SaveData->RescuedStageOneWorkerIds.Contains(WorkerId);
}

bool UALSAEQASaveManager::RecordStageOneSlaverDefeated(FName SlaverId)
{
    if (!EnsureSaveData() || SlaverId.IsNone() || SaveData->DefeatedStageOneSlaverIds.Contains(SlaverId)) return false;
    SaveData->DefeatedStageOneSlaverIds.Add(SlaverId);
    return SaveProgress();
}

bool UALSAEQASaveManager::HasStageOneSlaverDefeated(FName SlaverId) const
{
    return SaveData && !SlaverId.IsNone() && SaveData->DefeatedStageOneSlaverIds.Contains(SlaverId);
}

int32 UALSAEQASaveManager::GetStageOneWorkersRescuedCount() const
{
    return SaveData ? SaveData->RescuedStageOneWorkerIds.Num() : 0;
}

int32 UALSAEQASaveManager::GetStageOneSlaversDefeatedCount() const
{
    return SaveData ? SaveData->DefeatedStageOneSlaverIds.Num() : 0;
}
