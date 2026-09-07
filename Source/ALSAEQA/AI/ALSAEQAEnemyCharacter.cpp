#include "AI/ALSAEQAEnemyCharacter.h"
#include "Systems/ALSAEQAHealthComponent.h"
#include "Systems/ALSAEQAInjuryComponent.h"
#include "Visual/ALSAEQAVisualAssetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Player/ALSAEQACharacter.h"
#include "Interaction/ALSAEQAWorkerPrisonerActor.h"
#include "Progression/ALSAEQAStageObjectiveComponent.h"
#include "Save/ALSAEQASaveManager.h"
#include "Engine/GameInstance.h"

AALSAEQAEnemyCharacter::AALSAEQAEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    HealthComponent = CreateDefaultSubobject<UALSAEQAHealthComponent>(TEXT("HealthComponent"));
    VisualAssetComponent = CreateDefaultSubobject<UALSAEQAVisualAssetComponent>(TEXT("VisualAssetComponent"));
    GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

void AALSAEQAEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (HealthComponent) HealthComponent->OnDeath.AddDynamic(this, &AALSAEQAEnemyCharacter::HandleDeath);
    AttackCooldownRemaining = 0.0f;
    WorkerAttackCooldownRemaining = 0.0f;

    if (bCountsAsStageOneSlaver && !StageOneSlaverId.IsNone() && GetGameInstance())
    {
        if (UALSAEQASaveManager* SaveManager = GetGameInstance()->GetSubsystem<UALSAEQASaveManager>())
        {
            if (SaveManager->HasStageOneSlaverDefeated(StageOneSlaverId))
            {
                bStageObjectiveReported = true;
                SetEnemyState(EALSAEQAEnemyState::Dead);
            }
        }
    }
}

void AALSAEQAEnemyCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (EnemyState == EALSAEQAEnemyState::Dead || !GetWorld()) return;

    AttackCooldownRemaining = FMath::Max(0.0f, AttackCooldownRemaining - DeltaSeconds);
    WorkerAttackCooldownRemaining = FMath::Max(0.0f, WorkerAttackCooldownRemaining - DeltaSeconds);

    int32 CurrentStage = 1;
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UALSAEQASaveManager* SaveManager = GameInstance->GetSubsystem<UALSAEQASaveManager>())
        {
            CurrentStage = FMath::Max(1, SaveManager->GetStage());
        }
    }

    const float Progress = static_cast<float>(CurrentStage - 1);
    const float StageSpeedMultiplier = 1.0f + Progress * 0.004f;
    const float StageDetectionMultiplier = 1.0f + Progress * 0.0025f;
    const float StageDamageMultiplier = 1.0f + Progress * 0.012f;
    const float StageAttackRateMultiplier = 1.0f + Progress * 0.003f;
    const float EffectiveDetectionRange = DetectionRange * StageDetectionMultiplier;
    const float EffectiveAttackRange = AttackRange * (1.0f + Progress * 0.0015f);

    float InjurySpeedMultiplier = 1.0f;
    if (const UALSAEQAInjuryComponent* Injury = FindComponentByClass<UALSAEQAInjuryComponent>())
    {
        InjurySpeedMultiplier = Injury->GetMovementSpeedMultiplier();
        if (Injury->IsKnockedOut() || Injury->IsDead()) return;
    }
    GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed * StageSpeedMultiplier * InjurySpeedMultiplier;

    AActor* Target = TargetActor.Get();

    if (CurrentStage == 1 && bStageOneMineGuard)
    {
        AALSAEQAWorkerPrisonerActor* ClosestEscapingWorker = nullptr;
        float ClosestWorkerDistanceSquared = FMath::Square(WorkerThreatRange);

        for (TActorIterator<AALSAEQAWorkerPrisonerActor> It(GetWorld()); It; ++It)
        {
            AALSAEQAWorkerPrisonerActor* Worker = *It;
            if (!IsValid(Worker) || Worker->IsSafe() || Worker->GetRescueState() != EALSAEQAWorkerRescueState::Escaping) continue;

            const float DistanceSquared = FVector::DistSquared2D(GetActorLocation(), Worker->GetActorLocation());
            if (DistanceSquared <= ClosestWorkerDistanceSquared)
            {
                ClosestWorkerDistanceSquared = DistanceSquared;
                ClosestEscapingWorker = Worker;
            }
        }

        if (ClosestEscapingWorker)
        {
            Target = ClosestEscapingWorker;
            SetTargetActor(Target);
        }
    }

    if (!IsValid(Target))
    {
        for (TActorIterator<AALSAEQACharacter> It(GetWorld()); It; ++It)
        {
            AALSAEQACharacter* Candidate = *It;
            if (IsValid(Candidate) && Candidate->GetHealthComponent() && !Candidate->GetHealthComponent()->IsDead() && FVector::DistSquared(GetActorLocation(), Candidate->GetActorLocation()) <= FMath::Square(EffectiveDetectionRange))
            {
                Target = Candidate;
                SetTargetActor(Target);
                break;
            }
        }
    }

    if (!IsValid(Target)) { SetEnemyState(EALSAEQAEnemyState::Idle); return; }

    const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    const bool bTargetIsWorker = Target->IsA<AALSAEQAWorkerPrisonerActor>();
    const float EffectiveTargetRange = bTargetIsWorker ? WorkerThreatRange : EffectiveDetectionRange;
    if (Distance > EffectiveTargetRange)
    {
        TargetActor.Reset();
        SetEnemyState(EALSAEQAEnemyState::Idle);
        return;
    }

    if (bTargetIsWorker)
    {
        AALSAEQAWorkerPrisonerActor* Worker = Cast<AALSAEQAWorkerPrisonerActor>(Target);
        if (!Worker || Worker->IsSafe() || Worker->GetRescueState() != EALSAEQAWorkerRescueState::Escaping)
        {
            TargetActor.Reset();
            SetEnemyState(EALSAEQAEnemyState::Alert);
            return;
        }

        if (Distance <= WorkerAttackRange)
        {
            SetEnemyState(EALSAEQAEnemyState::Attack);
            if (WorkerAttackCooldownRemaining <= 0.0f)
            {
                Worker->NotifyGuardPressure(this);
                WorkerAttackCooldownRemaining = WorkerAttackCooldown;
            }
            return;
        }

        SetEnemyState(EALSAEQAEnemyState::Chase);
        const FVector Direction = (Worker->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
        AddMovementInput(Direction, 1.0f);
        return;
    }

    if (Distance > EffectiveDetectionRange)
    {
        TargetActor.Reset();
        SetEnemyState(EALSAEQAEnemyState::Idle);
        return;
    }

    if (Distance <= EffectiveAttackRange)
    {
        SetEnemyState(EALSAEQAEnemyState::Attack);
        AALSAEQACharacter* Player = Cast<AALSAEQACharacter>(Target);
        if (Player && Player->GetHealthComponent() && !Player->GetHealthComponent()->IsDead() && AttackCooldownRemaining <= 0.0f)
        {
            FALSAEQADamageInfo Info;
            Info.Amount = AttackDamage * StageDamageMultiplier;
            Info.Type = EALSAEQADamageType::Physical;
            Info.Instigator = this;
            Info.HitLocation = Player->GetActorLocation();
            Info.HitRegion = EALSAEQADamageHitRegion::Auto;
            Player->GetHealthComponent()->ApplyDamageInfo(Info);
            AttackCooldownRemaining = AttackCooldown / StageAttackRateMultiplier;
        }
        return;
    }

    SetEnemyState(EALSAEQAEnemyState::Chase);
    const FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
    AddMovementInput(Direction, 1.0f);
}

void AALSAEQAEnemyCharacter::SetEnemyState(EALSAEQAEnemyState NewState)
{
    EnemyState = NewState;
    if (NewState == EALSAEQAEnemyState::Dead)
    {
        GetCharacterMovement()->DisableMovement();
        SetActorEnableCollision(false);
    }
}

void AALSAEQAEnemyCharacter::SetTargetActor(AActor* NewTarget)
{
    TargetActor = NewTarget;
    if (NewTarget) SetEnemyState(EALSAEQAEnemyState::Alert);
}

float AALSAEQAEnemyCharacter::ReceiveALSAEQADamage_Implementation(const FALSAEQADamageInfo& DamageInfo)
{
    if (!HealthComponent || HealthComponent->IsDead() || DamageInfo.Amount <= 0.0f) return 0.0f;
    HealthComponent->ApplyDamageInfo(DamageInfo);
    if (!HealthComponent->IsDead())
    {
        SetEnemyState(EALSAEQAEnemyState::Alert);
        if (DamageInfo.Instigator) SetTargetActor(DamageInfo.Instigator);
    }
    return DamageInfo.Amount;
}

void AALSAEQAEnemyCharacter::HandleDeath()
{
    if (bStageObjectiveReported) return;
    bStageObjectiveReported = true;
    SetEnemyState(EALSAEQAEnemyState::Dead);

    if (!bCountsAsStageOneSlaver || StageOneSlaverId.IsNone() || !GetWorld() || !GetGameInstance()) return;

    AALSAEQACharacter* Player = nullptr;
    for (TActorIterator<AALSAEQACharacter> It(GetWorld()); It; ++It)
    {
        if (AALSAEQACharacter* Candidate = *It)
        {
            if (IsValid(Candidate)) { Player = Candidate; break; }
        }
    }
    if (!Player) return;

    UALSAEQAStageObjectiveComponent* Objectives = Player->GetStageObjectiveComponent();
    UALSAEQASaveManager* SaveManager = GetGameInstance()->GetSubsystem<UALSAEQASaveManager>();
    if (!Objectives || !SaveManager) return;

    if (!SaveManager->RecordStageOneSlaverDefeated(StageOneSlaverId)) return;

    const int32 CurrentStage = SaveManager->GetStage();
    if (CurrentStage == 1) Objectives->RegisterProgress(TEXT("DefeatSlavers"), 1);
}
