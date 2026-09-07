#include "Progression/ALSAEQAStage1MineResponseCoordinator.h"

#include "Interaction/ALSAEQAWorkerPrisonerActor.h"
#include "AI/ALSAEQAEnemyCharacter.h"
#include "EngineUtils.h"
#include "Engine/GameInstance.h"
#include "Save/ALSAEQASaveManager.h"
#include "Player/ALSAEQACharacter.h"

UALSAEQAStage1MineResponseCoordinator::UALSAEQAStage1MineResponseCoordinator()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQAStage1MineResponseCoordinator::BeginPlay()
{
    Super::BeginPlay();
    if (!GetOwner() || !GetOwner()->GetWorld()) return;

    for (TActorIterator<AALSAEQAWorkerPrisonerActor> It(GetOwner()->GetWorld()); It; ++It)
    {
        RegisterWorker(*It);
    }

    for (TActorIterator<AALSAEQAEnemyCharacter> It(GetOwner()->GetWorld()); It; ++It)
    {
        RegisterMineGuard(*It);
    }

    int32 SavedWorkers = 0;
    if (UGameInstance* GI = GetOwner()->GetGameInstance())
    {
        if (UALSAEQASaveManager* Save = GI->GetSubsystem<UALSAEQASaveManager>())
        {
            SavedWorkers = Save->GetStageOneWorkersRescuedCount();
        }
    }

    WorkersRescued = FMath::Clamp(SavedWorkers, 0, Workers.Num());
    ResponseLevel = FMath::Clamp(WorkersRescued, 0, MaxResponseLevel);
    ValidateStage1Setup();
    RefreshGuardAssignments();
}

void UALSAEQAStage1MineResponseCoordinator::RegisterWorker(AALSAEQAWorkerPrisonerActor* Worker)
{
    if (!IsValid(Worker) || Workers.Contains(Worker) || !Worker->CountsAsStageOneWorker()) return;
    Workers.Add(Worker);
    Worker->OnWorkerEscapeStarted.AddDynamic(this, &UALSAEQAStage1MineResponseCoordinator::HandleWorkerEscapeStarted);
    Worker->OnWorkerRescued.AddDynamic(this, &UALSAEQAStage1MineResponseCoordinator::HandleWorkerRescued);
}

void UALSAEQAStage1MineResponseCoordinator::RegisterMineGuard(AALSAEQAEnemyCharacter* Guard)
{
    if (!IsValid(Guard) || Guards.Contains(Guard) || !Guard->IsStageOneMineGuard()) return;
    Guards.Add(Guard);
}

void UALSAEQAStage1MineResponseCoordinator::HandleWorkerEscapeStarted(AALSAEQAWorkerPrisonerActor* Worker)
{
    if (!IsValid(Worker)) return;
    ResponseLevel = FMath::Clamp(FMath::Max(ResponseLevel, WorkersRescued + 1), 1, MaxResponseLevel);
    RefreshGuardAssignments();
    PlayMineResponsePresentation(ResponseLevel, Worker);
}

void UALSAEQAStage1MineResponseCoordinator::HandleWorkerRescued(AALSAEQAWorkerPrisonerActor* Worker)
{
    if (!IsValid(Worker)) return;
    WorkersRescued = FMath::Clamp(WorkersRescued + 1, 0, Workers.Num());
    ResponseLevel = FMath::Clamp(WorkersRescued, 0, MaxResponseLevel);
    RefreshGuardAssignments();
    PlayMineResponsePresentation(ResponseLevel, Worker);
}

void UALSAEQAStage1MineResponseCoordinator::ValidateStage1Setup() const
{
    if (Workers.Num() < RequiredWorkers)
    {
        UE_LOG(LogTemp, Error, TEXT("ALSAEQA Stage 1 requires at least %d counted workers; found %d."), RequiredWorkers, Workers.Num());
    }

    if (Guards.Num() < MinimumMineGuards)
    {
        UE_LOG(LogTemp, Error, TEXT("ALSAEQA Stage 1 requires at least %d mine guards; found %d."), MinimumMineGuards, Guards.Num());
    }

    if (Guards.Num() <= Workers.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("ALSAEQA Stage 1 requires more mine guards than counted workers; guards=%d workers=%d."), Guards.Num(), Workers.Num());
    }
}

void UALSAEQAStage1MineResponseCoordinator::RefreshGuardAssignments()
{
    if (!GetOwner() || !GetOwner()->GetWorld()) return;

    AALSAEQACharacter* Hero = nullptr;
    for (TActorIterator<AALSAEQACharacter> It(GetOwner()->GetWorld()); It; ++It)
    {
        if (IsValid(*It)) { Hero = *It; break; }
    }

    TArray<AALSAEQAWorkerPrisonerActor*> EscapingWorkers;
    for (AALSAEQAWorkerPrisonerActor* Worker : Workers)
    {
        if (IsValid(Worker) && Worker->GetRescueState() == EALSAEQAWorkerRescueState::Escaping)
        {
            EscapingWorkers.Add(Worker);
        }
    }

    int32 Assigned = 0;
    const int32 DesiredAssignments = FMath::Min(Guards.Num(), ResponseLevel * FMath::Max(1, GuardsPerResponseLevel));

    for (AALSAEQAEnemyCharacter* Guard : Guards)
    {
        if (!IsValid(Guard) || Guard->GetEnemyState() == EALSAEQAEnemyState::Dead || Assigned >= DesiredAssignments) continue;

        AALSAEQAWorkerPrisonerActor* NearestWorker = nullptr;
        float BestDistanceSq = TNumericLimits<float>::Max();
        for (AALSAEQAWorkerPrisonerActor* Worker : EscapingWorkers)
        {
            if (!IsValid(Worker)) continue;
            const float DistanceSq = FVector::DistSquared2D(Guard->GetActorLocation(), Worker->GetActorLocation());
            if (DistanceSq < BestDistanceSq)
            {
                BestDistanceSq = DistanceSq;
                NearestWorker = Worker;
            }
        }

        if (NearestWorker)
        {
            Guard->SetTargetActor(NearestWorker);
            NearestWorker->NotifyGuardPressure(Guard);
            ++Assigned;
        }
    }

    if (Hero && Assigned == 0 && ResponseLevel > 0)
    {
        for (AALSAEQAEnemyCharacter* Guard : Guards)
        {
            if (IsValid(Guard) && Guard->GetEnemyState() != EALSAEQAEnemyState::Dead)
            {
                Guard->SetTargetActor(Hero);
            }
        }
    }
}
