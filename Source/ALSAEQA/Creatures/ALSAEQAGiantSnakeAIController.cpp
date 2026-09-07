#include "Creatures/ALSAEQAGiantSnakeAIController.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Creatures/ALSAEQAGiantSnake.h"

AALSAEQAGiantSnakeAIController::AALSAEQAGiantSnakeAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AALSAEQAGiantSnakeAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    SetSnakeState(EALSAEQAGiantSnakeState::Hidden);
}

void AALSAEQAGiantSnakeAIController::AcquireTarget(AActor* NewTarget)
{
    TargetActor = NewTarget;
    if (TargetActor)
    {
        SetSnakeState(EALSAEQAGiantSnakeState::Chase);
    }
}

void AALSAEQAGiantSnakeAIController::SetSnakeState(EALSAEQAGiantSnakeState NewState)
{
    State = NewState;

    AALSAEQAGiantSnake* Snake = Cast<AALSAEQAGiantSnake>(GetPawn());
    if (!Snake)
    {
        return;
    }

    switch (State)
    {
    case EALSAEQAGiantSnakeState::Ambush:
        StopMovement();
        Snake->StartAmbush();
        break;
    case EALSAEQAGiantSnakeState::Attack:
        StopMovement();
        break;
    case EALSAEQAGiantSnakeState::Coil:
        StopMovement();
        Snake->CoilAroundObstacle();
        break;
    case EALSAEQAGiantSnakeState::Retreat:
        StopMovement();
        Snake->Retreat();
        break;
    case EALSAEQAGiantSnakeState::Hidden:
        StopMovement();
        break;
    case EALSAEQAGiantSnakeState::Defeated:
        StopMovement();
        if (Snake->GetCharacterMovement())
        {
            Snake->GetCharacterMovement()->DisableMovement();
        }
        break;
    case EALSAEQAGiantSnakeState::Chase:
    default:
        break;
    }
}

void AALSAEQAGiantSnakeAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    AALSAEQAGiantSnake* Snake = Cast<AALSAEQAGiantSnake>(GetPawn());
    if (!Snake)
    {
        return;
    }

    if (Snake->IsDefeated())
    {
        SetSnakeState(EALSAEQAGiantSnakeState::Defeated);
        return;
    }

    if (!IsValid(TargetActor))
    {
        TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
    }

    if (!IsValid(TargetActor))
    {
        SetSnakeState(EALSAEQAGiantSnakeState::Hidden);
        return;
    }

    const float Distance = FVector::Dist(Snake->GetActorLocation(), TargetActor->GetActorLocation());

    if (Distance <= AttackRange)
    {
        SetSnakeState(EALSAEQAGiantSnakeState::Attack);
    }
    else if (Distance <= DetectionRange)
    {
        SetSnakeState(EALSAEQAGiantSnakeState::Chase);
        MoveToActor(TargetActor, AttackRange * 0.75f, true);
    }
    else
    {
        TargetActor = nullptr;
        SetSnakeState(EALSAEQAGiantSnakeState::Hidden);
    }
}
