#include "Progression/ALSAEQAStageObjectiveComponent.h"

#include "Progression/ALSAEQAProgressionComponent.h"
#include "Progression/ALSAEQAStageFlowComponent.h"
#include "Story/ALSAEQALegacyComponent.h"
#include "Save/ALSAEQASaveManager.h"
#include "Engine/GameInstance.h"

UALSAEQAStageObjectiveComponent::UALSAEQAStageObjectiveComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQAStageObjectiveComponent::BeginPlay()
{
    Super::BeginPlay();
    StageFlow = GetOwner() ? GetOwner()->FindComponentByClass<UALSAEQAStageFlowComponent>() : nullptr;

    int32 StageNumber = 1;
    if (UALSAEQAProgressionComponent* Progression = GetOwner() ? GetOwner()->FindComponentByClass<UALSAEQAProgressionComponent>() : nullptr)
    {
        StageNumber = Progression->GetCurrentStage();
    }

    if (ObjectiveRequirements.Num() == 0 && StageNumber == 1)
    {
        ObjectiveRequirements.Add(TEXT("RescueWorkers"), 5);
        ObjectiveRequirements.Add(TEXT("DefeatSlavers"), 1);
    }

    ResetObjectives();

    if (StageNumber == 1 && GetOwner() && GetOwner()->GetGameInstance())
    {
        if (UALSAEQASaveManager* SaveManager = GetOwner()->GetGameInstance()->GetSubsystem<UALSAEQASaveManager>())
        {
            if (ObjectiveRequirements.Contains(TEXT("RescueWorkers")))
            {
                ObjectiveProgress.FindOrAdd(TEXT("RescueWorkers")) = FMath::Clamp(
                    SaveManager->GetStageOneWorkersRescuedCount(), 0, ObjectiveRequirements.FindChecked(TEXT("RescueWorkers")));
            }
            if (ObjectiveRequirements.Contains(TEXT("DefeatSlavers")))
            {
                ObjectiveProgress.FindOrAdd(TEXT("DefeatSlavers")) = FMath::Clamp(
                    SaveManager->GetStageOneSlaversDefeatedCount(), 0, ObjectiveRequirements.FindChecked(TEXT("DefeatSlavers")));
            }
        }
    }

    FinalizeStageIfReady();
}

void UALSAEQAStageObjectiveComponent::ResetObjectives()
{
    ObjectiveProgress.Reset();
    for (const TPair<FName, int32>& Pair : ObjectiveRequirements)
    {
        if (!Pair.Key.IsNone() && Pair.Value > 0) ObjectiveProgress.Add(Pair.Key, 0);
    }
}

int32 UALSAEQAStageObjectiveComponent::GetObjectiveProgress(FName ObjectiveId) const
{
    if (const int32* Value = ObjectiveProgress.Find(ObjectiveId)) return *Value;
    return 0;
}

bool UALSAEQAStageObjectiveComponent::IsObjectiveComplete(FName ObjectiveId) const
{
    const int32* Requirement = ObjectiveRequirements.Find(ObjectiveId);
    return Requirement && *Requirement > 0 && GetObjectiveProgress(ObjectiveId) >= *Requirement;
}

int32 UALSAEQAStageObjectiveComponent::GetCompletedObjectiveCount() const
{
    int32 Count = 0;
    for (const TPair<FName, int32>& Pair : ObjectiveRequirements)
    {
        if (Pair.Value > 0 && IsObjectiveComplete(Pair.Key)) ++Count;
    }
    return Count;
}

bool UALSAEQAStageObjectiveComponent::AreAllObjectivesComplete() const
{
    if (ObjectiveRequirements.Num() == 0) return false;
    for (const TPair<FName, int32>& Pair : ObjectiveRequirements)
    {
        if (Pair.Value <= 0 || !IsObjectiveComplete(Pair.Key)) return false;
    }
    return true;
}

bool UALSAEQAStageObjectiveComponent::RegisterProgress(FName ObjectiveId, int32 Amount)
{
    if (ObjectiveId.IsNone() || Amount <= 0 || !ObjectiveRequirements.Contains(ObjectiveId) || IsObjectiveComplete(ObjectiveId)) return false;

    const int32 Requirement = ObjectiveRequirements.FindChecked(ObjectiveId);
    const int32 NewProgress = FMath::Clamp(GetObjectiveProgress(ObjectiveId) + Amount, 0, Requirement);
    ObjectiveProgress.FindOrAdd(ObjectiveId) = NewProgress;
    OnObjectiveProgress.Broadcast(ObjectiveId, NewProgress);

    if (NewProgress >= Requirement) OnObjectiveCompleted.Broadcast(ObjectiveId);

    FinalizeStageIfReady();
    return true;
}

bool UALSAEQAStageObjectiveComponent::CompleteObjective(FName ObjectiveId)
{
    if (!ObjectiveRequirements.Contains(ObjectiveId)) return false;

    const int32 Requirement = FMath::Max(1, ObjectiveRequirements.FindChecked(ObjectiveId));
    const int32 Current = GetObjectiveProgress(ObjectiveId);
    if (Current >= Requirement) return false;

    return RegisterProgress(ObjectiveId, Requirement - Current);
}

bool UALSAEQAStageObjectiveComponent::FinalizeStageIfReady()
{
    if (!StageFlow || !AreAllObjectivesComplete()) return false;

    // Persist the first clue in the canonical save before moving to Stage 2.
    // It is a narrative reward, never a third completion gate.
    if (AActor* Owner = GetOwner())
    {
        if (UALSAEQALegacyComponent* Legacy = Owner->FindComponentByClass<UALSAEQALegacyComponent>())
        {
            Legacy->AddFamilyClue(TEXT("Stage1_MineNetwork"));
            Legacy->SetLegacyFlag(TEXT("Stage1_FirstClueFound"), true);
        }

        if (UGameInstance* GameInstance = Owner->GetGameInstance())
        {
            if (UALSAEQASaveManager* SaveManager = GameInstance->GetSubsystem<UALSAEQASaveManager>())
            {
                SaveManager->AddFamilyClue(TEXT("Stage1_MineNetwork"));
                SaveManager->RegisterWorldFlag(TEXT("Stage1_FirstClueFound"));
                SaveManager->SaveProgress();
            }
        }
    }

    return StageFlow->CompleteCurrentStage();
}
