#include "ALSAEQACompanionCharacter.h"
#include "Systems/ALSAEQAHealthComponent.h"
#include "Companions/ALSAEQACompanionStoryComponent.h"
#include "Companions/ALSAEQACompanionCaptureComponent.h"
#include "Companions/ALSAEQARidingComponent.h"
#include "Companions/ALSAEQAMountActor.h"
#include "Visual/ALSAEQAVisualAssetComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Save/ALSAEQASaveManager.h"
#include "Player/ALSAEQACharacter.h"

AALSAEQACompanionCharacter::AALSAEQACompanionCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    HealthComponent = CreateDefaultSubobject<UALSAEQAHealthComponent>(TEXT("HealthComponent"));
    StoryComponent = CreateDefaultSubobject<UALSAEQACompanionStoryComponent>(TEXT("StoryComponent"));
    CaptureComponent = CreateDefaultSubobject<UALSAEQACompanionCaptureComponent>(TEXT("CaptureComponent"));
    RidingComponent = CreateDefaultSubobject<UALSAEQARidingComponent>(TEXT("RidingComponent"));
    VisualAssetComponent = CreateDefaultSubobject<UALSAEQAVisualAssetComponent>(TEXT("VisualAssetComponent"));
    VisualAssetComponent->bApplyOnBeginPlay = true;
    GetCharacterMovement()->MaxWalkSpeed = CompanionWalkSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
}

void AALSAEQACompanionCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateFollowMovement(DeltaSeconds);
}

void AALSAEQACompanionCharacter::UpdateFollowMovement(float DeltaSeconds)
{
    if (DeltaSeconds <= 0.f || bListening || bRolling || BehaviorState == EALSAEQACompanionBehaviorState::Captured || BehaviorState == EALSAEQACompanionBehaviorState::Separated || !GetWorld()) return;
    const AALSAEQACharacter* Hero = Cast<AALSAEQACharacter>(GetWorld()->GetFirstPlayerController() ? GetWorld()->GetFirstPlayerController()->GetPawn() : nullptr);
    if (!Hero || Hero == this || Hero->IsRiding()) return;

    const FVector Offset = Hero->GetActorLocation() - GetActorLocation();
    const float Distance = Offset.Size2D();
    if (Distance <= FollowStopDistance) return;

    FVector Direction = Offset;
    Direction.Z = 0.f;
    Direction.Normalize();
    const bool bFar = Distance > FollowStartDistance;
    GetCharacterMovement()->MaxWalkSpeed = bFar ? CompanionSprintSpeed : CompanionWalkSpeed;
    AddMovementInput(Direction, FMath::Clamp((Distance - FollowStopDistance) / FMath::Max(1.f, FollowDistance), 0.25f, 1.f));
    PlayMovementPresentation(bFar ? TEXT("FollowSprint") : TEXT("FollowWalk"));
}

void AALSAEQACompanionCharacter::SetBehaviorState(EALSAEQACompanionBehaviorState NewState)
{
    BehaviorState = NewState;
    if (NewState == EALSAEQACompanionBehaviorState::Captured || NewState == EALSAEQACompanionBehaviorState::Separated)
    {
        GetCharacterMovement()->StopMovementImmediately();
    }
}

void AALSAEQACompanionCharacter::SetCompanionId(FName NewCompanionId)
{
    if (!NewCompanionId.IsNone()) { CompanionId = NewCompanionId; if (StoryComponent) StoryComponent->SetCompanionId(CompanionId); }
}

void AALSAEQACompanionCharacter::ToggleCrouch()
{
    if (BehaviorState == EALSAEQACompanionBehaviorState::Captured || bRolling) return;
    if (GetCharacterMovement()->IsCrouching()) { StopCrouching(); GetCharacterMovement()->MaxWalkSpeed = CompanionWalkSpeed; PlayMovementPresentation(TEXT("Stand")); }
    else { Crouch(); GetCharacterMovement()->MaxWalkSpeed = 170.f; PlayMovementPresentation(TEXT("Crouch")); }
}

void AALSAEQACompanionCharacter::StartListen()
{
    if (bRolling || BehaviorState == EALSAEQACompanionBehaviorState::Captured) return;
    bListening = true; GetCharacterMovement()->StopMovementImmediately(); GetCharacterMovement()->MaxWalkSpeed = 0.f;
    if (!GetCharacterMovement()->IsCrouching()) Crouch();
    PlayListenPresentation(true);
}

void AALSAEQACompanionCharacter::StopListen()
{
    if (!bListening) return;
    bListening = false; GetCharacterMovement()->MaxWalkSpeed = CompanionWalkSpeed; PlayListenPresentation(false);
}

void AALSAEQACompanionCharacter::PerformRoll()
{
    if (bRolling || !bRollReady || bListening || BehaviorState == EALSAEQACompanionBehaviorState::Captured || !GetCharacterMovement()->IsMovingOnGround()) return;
    FVector Direction = GetLastMovementInputVector();
    if (Direction.IsNearlyZero()) Direction = GetActorForwardVector();
    Direction.Z = 0.f; Direction.Normalize();
    bRolling = true; bRollReady = false;
    GetCharacterMovement()->BrakingFrictionFactor = 0.f;
    LaunchCharacter(Direction * 650.f, false, false);
    PlayRollPresentation();
    GetWorldTimerManager().SetTimer(RollTimerHandle, this, &AALSAEQACompanionCharacter::FinishRoll, .42f, false);
    GetWorldTimerManager().SetTimer(RollCooldownTimerHandle, [this]() { bRollReady = true; }, .65f, false);
}

void AALSAEQACompanionCharacter::FinishRoll()
{
    bRolling = false;
    GetCharacterMovement()->BrakingFrictionFactor = 2.f;
    GetCharacterMovement()->MaxWalkSpeed = CompanionWalkSpeed;
}

bool AALSAEQACompanionCharacter::CaptureCompanion()
{
    if (RidingComponent && RidingComponent->IsRiding()) RidingComponent->Dismount();
    if (!CaptureComponent || !CaptureComponent->Capture()) return false;
    bHelpingHeroFamilySearch = false; SetBehaviorState(EALSAEQACompanionBehaviorState::Captured);
    if (StoryComponent) StoryComponent->SetState(EALSAEQACompanionStoryState::Captured); return true;
}

bool AALSAEQACompanionCharacter::MarkRescueKnown()
{
    if (!CaptureComponent || !CaptureComponent->MarkRescueKnown()) return false;
    SetBehaviorState(EALSAEQACompanionBehaviorState::Captured); if (StoryComponent) StoryComponent->SetState(EALSAEQACompanionStoryState::RescueKnown); return true;
}

bool AALSAEQACompanionCharacter::RescueCompanion()
{
    if (!CaptureComponent || !CaptureComponent->Rescue()) return false;
    SetBehaviorState(EALSAEQACompanionBehaviorState::Rescued); if (StoryComponent) StoryComponent->RegisterRescue(); return true;
}

bool AALSAEQACompanionCharacter::BeginFamilySearch()
{
    UWorld* World = GetWorld(); UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr; UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    if (!SaveManager || !SaveManager->CanBeginFamilySearch()) return false; return StoryComponent && StoryComponent->SetState(EALSAEQACompanionStoryState::FamilyRevelation);
}

bool AALSAEQACompanionCharacter::BeginHelpingHeroFamilySearch()
{
    UWorld* World = GetWorld(); UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr; UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    if (!SaveManager || SaveManager->GetStage() < 40 || !CaptureComponent || !CaptureComponent->IsRescued()) return false;
    bHelpingHeroFamilySearch = true; SetBehaviorState(EALSAEQACompanionBehaviorState::Assisting); return true;
}

bool AALSAEQACompanionCharacter::Mount(AALSAEQAMountActor* MountActor)
{
    if (!MountActor || !RidingComponent || BehaviorState == EALSAEQACompanionBehaviorState::Captured || BehaviorState == EALSAEQACompanionBehaviorState::Separated) return false;
    return RidingComponent->TryMount(MountActor);
}

bool AALSAEQACompanionCharacter::Dismount() { return RidingComponent && RidingComponent->Dismount(); }
