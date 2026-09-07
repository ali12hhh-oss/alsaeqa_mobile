#include "MainMenu/ALSAEQAMainMenuSubsystem.h"

#include "Save/ALSAEQASaveManager.h"

void UALSAEQAMainMenuSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    RefreshState();
}

void UALSAEQAMainMenuSubsystem::RefreshState()
{
    State = FALSAEQAMainMenuState();

    if (!GetGameInstance()) return;
    UALSAEQASaveManager* Save = GetGameInstance()->GetSubsystem<UALSAEQASaveManager>();
    if (!Save) return;

    Save->LoadProgress();
    State.bHasSave = Save->HasSavedJourney();
    State.CurrentStage = Save->GetStage();

    const FALSAEQACheckpointData Checkpoint = Save->GetLastCheckpoint();
    State.CurrentRegion = Checkpoint.RegionId;
    State.CurrentCheckpoint = Checkpoint.CheckpointId;
    State.bCompanionRescued = static_cast<uint8>(Save->GetCompanionStory().State) >= static_cast<uint8>(EALSAEQACompanionStoryState::Rescued);
    State.CompanionFamilySearchStage = Save->GetFamilySearchStage();
    State.bThunderBeastOwned = Save->HasTamedMount(FName(TEXT("ThunderBeast")));

    OnStateChanged.Broadcast(State);
}

bool UALSAEQAMainMenuSubsystem::ContinueJourney()
{
    RefreshState();
    if (!State.bHasSave) return false;
    OnActionRequested.Broadcast(EALSAEQAMainMenuAction::ContinueJourney);
    return true;
}

bool UALSAEQAMainMenuSubsystem::BeginNewJourney(bool bConfirmExistingSave)
{
    RefreshState();
    if (State.bHasSave && !bConfirmExistingSave) return false;

    if (!GetGameInstance()) return false;
    UALSAEQASaveManager* Save = GetGameInstance()->GetSubsystem<UALSAEQASaveManager>();
    if (!Save || !Save->BeginNewJourney()) return false;

    RefreshState();
    OnActionRequested.Broadcast(EALSAEQAMainMenuAction::NewJourney);
    return true;
}

bool UALSAEQAMainMenuSubsystem::ConfirmNewJourney()
{
    return BeginNewJourney(true);
}

void UALSAEQAMainMenuSubsystem::NotifyAction(EALSAEQAMainMenuAction Action)
{
    OnActionRequested.Broadcast(Action);
}
