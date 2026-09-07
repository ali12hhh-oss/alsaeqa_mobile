#include "Interaction/ALSAEQAWorkerPrisonerActor.h"

#include "Player/ALSAEQACharacter.h"
#include "Progression/ALSAEQAProgressionComponent.h"
#include "Progression/ALSAEQAStageObjectiveComponent.h"
#include "Cinematic/ALSAEQACinematicDirector.h"
#include "Save/ALSAEQASaveManager.h"
#include "AI/ALSAEQAEnemyCharacter.h"
#include "EngineUtils.h"
#include "Engine/GameInstance.h"

AALSAEQAWorkerPrisonerActor::AALSAEQAWorkerPrisonerActor()
{
    PrimaryActorTick.bCanEverTick = true;
    InteractionPrompt = NSLOCTEXT("ALSAEQA", "WorkerRescuePrompt", "إنقاذ العامل");
}

void AALSAEQAWorkerPrisonerActor::BeginPlay()
{
    Super::BeginPlay();
    bRescued = false;
    bRescueInProgress = false;
    RescueState = EALSAEQAWorkerRescueState::Captive;
    GuardPressurePauseRemaining = 0.0f;

    if (!bCountsAsStageOneWorker || WorkerId.IsNone() || !GetGameInstance()) return;
    if (UALSAEQASaveManager* SaveManager = GetGameInstance()->GetSubsystem<UALSAEQASaveManager>())
    {
        if (SaveManager->HasStageOneWorkerRescued(WorkerId))
        {
            bRescued = true;
            RescueState = EALSAEQAWorkerRescueState::Safe;
            InteractionPrompt = RescuedInteractionPrompt;
        }
    }
}

void AALSAEQAWorkerPrisonerActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    GuardPressurePauseRemaining = FMath::Max(0.0f, GuardPressurePauseRemaining - DeltaSeconds);

    if (bRescueInProgress && IsRescueThreatening())
    {
        CancelRescue();
        return;
    }

    if (RescueState == EALSAEQAWorkerRescueState::Escaping) UpdateEscape(DeltaSeconds);
}

void AALSAEQAWorkerPrisonerActor::Interact_Implementation(AActor* Interactor)
{
    Rescue(Interactor);
}

void AALSAEQAWorkerPrisonerActor::SetSafePoint(FVector NewSafePoint)
{
    SafePoint = NewSafePoint;
}

void AALSAEQAWorkerPrisonerActor::NotifyGuardPressure(AActor* Guard)
{
    if (bRescued || RescueState != EALSAEQAWorkerRescueState::Escaping || !IsValid(Guard)) return;
    GuardPressurePauseRemaining = FMath::Max(GuardPressurePauseRemaining, GuardPressurePause);
    PlayWorkerGuardPressurePresentation(Guard);
}

bool AALSAEQAWorkerPrisonerActor::IsRescueThreatening() const
{
    UWorld* World = GetWorld();
    if (!World) return true;
    const FVector WorkerLocation = GetActorLocation();
    for (TActorIterator<AALSAEQAEnemyCharacter> It(World); It; ++It)
    {
        const AALSAEQAEnemyCharacter* Enemy = *It;
        if (!IsValid(Enemy) || Enemy->GetEnemyState() == EALSAEQAEnemyState::Dead || Enemy->GetEnemyState() == EALSAEQAEnemyState::Stunned) continue;
        const bool bNearWorker = FVector::DistSquared2D(Enemy->GetActorLocation(), WorkerLocation) <= FMath::Square(RescueThreatRadius);
        const bool bTargetingRescuer = RescueInstigator.IsValid() && Enemy->GetTargetActor() == RescueInstigator.Get();
        if (bNearWorker || bTargetingRescuer) return true;
    }
    return false;
}

bool AALSAEQAWorkerPrisonerActor::Rescue(AActor* Rescuer)
{
    if (bRescued || bRescueInProgress || RescueState != EALSAEQAWorkerRescueState::Captive || !bCountsAsStageOneWorker || WorkerId.IsNone() || !Rescuer) return false;
    AALSAEQACharacter* Hero = Cast<AALSAEQACharacter>(Rescuer);
    if (!Hero) return false;

    UALSAEQAProgressionComponent* Progression = Hero->GetProgressionComponent();
    UALSAEQAStageObjectiveComponent* Objectives = Hero->GetStageObjectiveComponent();
    if (!Progression || !Objectives || Progression->GetCurrentStage() != 1) return false;

    UALSAEQASaveManager* SaveManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    if (SaveManager && SaveManager->HasStageOneWorkerRescued(WorkerId))
    {
        bRescued = true;
        RescueState = EALSAEQAWorkerRescueState::Safe;
        InteractionPrompt = RescuedInteractionPrompt;
        return false;
    }

    RescueInstigator = Hero;
    bRescueInProgress = true;
    RescueState = EALSAEQAWorkerRescueState::BeingRescued;
    if (IsRescueThreatening())
    {
        bRescueInProgress = false;
        RescueState = EALSAEQAWorkerRescueState::Captive;
        RescueInstigator.Reset();
        return false;
    }

    InteractionPrompt = NSLOCTEXT("ALSAEQA", "WorkerRescueInProgressPrompt", "جارٍ الإنقاذ...");
    PlayRescuePresentation(RescueMethod, RescueSequenceTag);
    GetWorldTimerManager().SetTimer(RescueTimerHandle, this, &AALSAEQAWorkerPrisonerActor::FinishRescue, FMath::Max(0.25f, RescueDuration), false);
    return true;
}

void AALSAEQAWorkerPrisonerActor::FinishRescue()
{
    if (!bRescueInProgress || bRescued) return;
    if (IsRescueThreatening()) { CancelRescue(); return; }

    AALSAEQACharacter* Hero = Cast<AALSAEQACharacter>(RescueInstigator.Get());
    if (!Hero || !Hero->GetProgressionComponent() || !Hero->GetStageObjectiveComponent() || Hero->GetProgressionComponent()->GetCurrentStage() != 1)
    {
        CancelRescue();
        return;
    }

    bRescueInProgress = false;
    RescueInstigator.Reset();
    BeginEscape();
    InteractionPrompt = NSLOCTEXT("ALSAEQA", "WorkerEscapingPrompt", "العامل يهرب إلى مكان آمن");

    if (FVector::DistSquared2D(GetActorLocation(), SafePoint) <= FMath::Square(SafePointRadius)) CompleteSafeArrival();
}

void AALSAEQAWorkerPrisonerActor::BeginEscape()
{
    RescueState = EALSAEQAWorkerRescueState::Escaping;
    PlayWorkerEscapePresentation();
    OnWorkerEscapeStarted.Broadcast(this);
}

void AALSAEQAWorkerPrisonerActor::UpdateEscape(float DeltaSeconds)
{
    if (bRescued || !GetWorld() || GuardPressurePauseRemaining > 0.0f) return;
    const FVector Current = GetActorLocation();
    FVector ToSafe = SafePoint - Current;
    ToSafe.Z = 0.0f;
    const float Distance = ToSafe.Size();
    if (Distance <= SafePointRadius) { CompleteSafeArrival(); return; }

    const FVector Direction = ToSafe.GetSafeNormal();
    const FVector Desired = Current + Direction * FMath::Min(EscapeSpeed * DeltaSeconds, Distance);
    FHitResult Hit;
    SetActorLocation(Desired, true, &Hit, ETeleportType::None);
    SetActorRotation(Direction.Rotation());
}

void AALSAEQAWorkerPrisonerActor::CompleteSafeArrival()
{
    if (bRescued) return;
    AALSAEQACharacter* Hero = nullptr;
    if (GetWorld())
    {
        for (TActorIterator<AALSAEQACharacter> It(GetWorld()); It; ++It)
        {
            if (IsValid(*It)) { Hero = *It; break; }
        }
    }
    if (!Hero || !Hero->GetProgressionComponent() || !Hero->GetStageObjectiveComponent() || Hero->GetProgressionComponent()->GetCurrentStage() != 1) return;

    UALSAEQASaveManager* SaveManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    if (SaveManager && !SaveManager->RecordStageOneWorkerRescued(WorkerId)) return;
    if (!Hero->GetStageObjectiveComponent()->RegisterProgress(TEXT("RescueWorkers"), 1)) return;

    bRescued = true;
    RescueState = EALSAEQAWorkerRescueState::Safe;
    InteractionPrompt = RescuedInteractionPrompt;
    PlayWorkerSafePresentation();
    if (UALSAEQACinematicDirector* Cinematic = Hero->GetCinematicDirector()) Cinematic->StartStoryBeat(EALSAEQACinematicEvent::Rescue);
    OnWorkerRescued.Broadcast(this);
}

void AALSAEQAWorkerPrisonerActor::CancelRescue()
{
    if (!bRescueInProgress && RescueState != EALSAEQAWorkerRescueState::BeingRescued) return;
    GetWorldTimerManager().ClearTimer(RescueTimerHandle);
    bRescueInProgress = false;
    RescueInstigator.Reset();
    RescueState = EALSAEQAWorkerRescueState::Captive;
    InteractionPrompt = NSLOCTEXT("ALSAEQA", "WorkerRescuePrompt", "إنقاذ العامل");
    PlayRescueInterruptedPresentation();
}
