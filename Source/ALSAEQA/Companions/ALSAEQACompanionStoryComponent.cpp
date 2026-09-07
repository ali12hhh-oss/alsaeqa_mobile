#include "Companions/ALSAEQACompanionStoryComponent.h"

#include "Engine/World.h"
#include "Save/ALSAEQASaveManager.h"

UALSAEQACompanionStoryComponent::UALSAEQACompanionStoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQACompanionStoryComponent::BeginPlay()
{
    Super::BeginPlay();
    SynchronizeFromSaveManager(true);
}

void UALSAEQACompanionStoryComponent::SynchronizeFromSaveManager(bool bBroadcastUnlock)
{
    UWorld* World = GetWorld();
    UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    if (!SaveManager)
    {
        return;
    }

    const FALSAEQACompanionStoryProgress SavedProgress = SaveManager->GetCompanionStory();
    const bool bWasUnlocked = Progress.PersistentFlags.Contains(TEXT("FamilySearchUnlocked"));

    Progress = SavedProgress;

    if (SaveManager->CanBeginFamilySearch() && !Progress.bFamilySearchCompleted)
    {
        Progress.FamilySearchStage = FMath::Max(25, Progress.FamilySearchStage);
        if (!Progress.PersistentFlags.Contains(TEXT("FamilySearchUnlocked")))
        {
            Progress.PersistentFlags.Add(TEXT("FamilySearchUnlocked"));
        }

        if (bBroadcastUnlock && !bWasUnlocked)
        {
            OnFamilySearchUnlocked.Broadcast(Progress.FamilySearchStage);
        }
    }
}

bool UALSAEQACompanionStoryComponent::SetState(EALSAEQACompanionStoryState NewState)
{
    UWorld* World = GetWorld();
    UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;

    if (SaveManager && !SaveManager->SetCompanionState(NewState))
    {
        return false;
    }

    Progress.State = NewState;
    OnStateChanged.Broadcast(NewState);
    SynchronizeFromSaveManager(true);
    return true;
}

bool UALSAEQACompanionStoryComponent::DiscoverFamilyClue(FName ClueId)
{
    if (ClueId.IsNone())
    {
        return false;
    }

    UWorld* World = GetWorld();
    UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;

    // Clues are intentionally allowed before stage 25; they never start the family search.
    if (SaveManager && SaveManager->AddFamilyClue(ClueId))
    {
        SynchronizeFromSaveManager(true);
        return true;
    }

    return false;
}

bool UALSAEQACompanionStoryComponent::DiscoverMemory(FName MemoryId)
{
    if (MemoryId.IsNone())
    {
        return false;
    }

    UWorld* World = GetWorld();
    UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    return !SaveManager || SaveManager->RegisterMemory(MemoryId);
}

bool UALSAEQACompanionStoryComponent::RegisterRescue()
{
    if (!SetState(EALSAEQACompanionStoryState::Rescued))
    {
        return false;
    }

    ++Progress.RescueCount;
    return true;
}

void UALSAEQACompanionStoryComponent::SetCompanionId(FName NewCompanionId)
{
    if (!NewCompanionId.IsNone())
    {
        Progress.CompanionId = NewCompanionId;
    }
}

bool UALSAEQACompanionStoryComponent::IsFamilySearchUnlocked() const
{
    const UWorld* World = GetWorld();
    const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    const UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    return SaveManager && SaveManager->CanBeginFamilySearch();
}
