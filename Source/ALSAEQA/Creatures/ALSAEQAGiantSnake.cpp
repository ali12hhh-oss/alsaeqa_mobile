#include "Creatures/ALSAEQAGiantSnake.h"

#include "Systems/ALSAEQAHealthComponent.h"
#include "Combat/ALSAEQAResistanceComponent.h"
#include "Status/ALSAEQAStatusComponent.h"
#include "Combat/ALSAEQADamageReceiver.h"
#include "Save/ALSAEQASaveManager.h"
#include "Creatures/ALSAEQAGiantSnakeAIController.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

AALSAEQAGiantSnake::AALSAEQAGiantSnake()
{
    PrimaryActorTick.bCanEverTick = true;
    HealthComponent = CreateDefaultSubobject<UALSAEQAHealthComponent>(TEXT("HealthComponent"));
    ResistanceComponent = CreateDefaultSubobject<UALSAEQAResistanceComponent>(TEXT("ResistanceComponent"));
    StatusComponent = CreateDefaultSubobject<UALSAEQAStatusComponent>(TEXT("StatusComponent"));

    AIControllerClass = AALSAEQAGiantSnakeAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    GetCharacterMovement()->MaxWalkSpeed = 360.0f;
}

void AALSAEQAGiantSnake::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (IsDefeated())
    {
        return;
    }

    AttackTimer = FMath::Max(0.0f, AttackTimer - DeltaSeconds);
    if (AttackTimer > 0.0f)
    {
        return;
    }

    UWorld* World = GetWorld();
    APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
    APawn* PlayerPawn = PlayerController ? PlayerController->GetPawn() : nullptr;
    if (!IsValid(PlayerPawn))
    {
        return;
    }

    if (FVector::DistSquared(GetActorLocation(), PlayerPawn->GetActorLocation()) > FMath::Square(AttackRange))
    {
        return;
    }

    int32 Stage = 1;
    if (World && World->GetGameInstance())
    {
        if (UALSAEQASaveManager* SaveManager = World->GetGameInstance()->GetSubsystem<UALSAEQASaveManager>())
        {
            Stage = FMath::Max(1, SaveManager->GetStage());
        }
    }

    const float StageDamageMultiplier = 1.0f + (static_cast<float>(Stage - 1) * 0.012f);
    const bool bChargeAttack = bUseChargeAttack;
    const float Damage = (bChargeAttack ? ChargeDamage : BiteDamage) * StageDamageMultiplier;

    if (PlayerPawn->GetClass()->ImplementsInterface(UALSAEQADamageReceiver::StaticClass()))
    {
        FALSAEQADamageInfo DamageInfo;
        DamageInfo.Amount = Damage;
        DamageInfo.Type = EALSAEQADamageType::Physical;
        DamageInfo.Instigator = this;
        DamageInfo.HitLocation = PlayerPawn->GetActorLocation();
        IALSAEQADamageReceiver::Execute_ReceiveALSAEQADamage(PlayerPawn, DamageInfo);
    }

    if (bChargeAttack)
    {
        if (UALSAEQAStatusComponent* PlayerStatus = PlayerPawn->FindComponentByClass<UALSAEQAStatusComponent>())
        {
            PlayerStatus->ApplyPoison(6.0f * StageDamageMultiplier, VenomDuration);
        }
    }

    bUseChargeAttack = !bUseChargeAttack;
    AttackTimer = FMath::Max(0.35f, AttackCooldown / (1.0f + (static_cast<float>(Stage - 1) * 0.003f)));
}

float AALSAEQAGiantSnake::ReceiveALSAEQADamage_Implementation(const FALSAEQADamageInfo& DamageInfo)
{
    if (!HealthComponent || DamageInfo.Amount <= 0.0f || IsDefeated())
    {
        return 0.0f;
    }

    const float FinalDamage = ResistanceComponent
        ? ResistanceComponent->ModifyDamage(DamageInfo)
        : DamageInfo.Amount;

    HealthComponent->ApplyDamage(FinalDamage);
    return FinalDamage;
}

void AALSAEQAGiantSnake::StartAmbush()
{
    AttackTimer = 0.0f;
    bUseChargeAttack = true;
}

void AALSAEQAGiantSnake::CoilAroundObstacle()
{
    // Used by level scripting to make the snake a traversal obstacle as well as an enemy.
}

void AALSAEQAGiantSnake::Retreat()
{
    AttackTimer = FMath::Max(AttackTimer, 2.0f);
}

bool AALSAEQAGiantSnake::IsDefeated() const
{
    return !HealthComponent || HealthComponent->IsDead();
}
