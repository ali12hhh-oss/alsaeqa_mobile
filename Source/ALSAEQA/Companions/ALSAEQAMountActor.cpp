#include "Companions/ALSAEQAMountActor.h"

#include "Companions/ALSAEQAMountComponent.h"
#include "Companions/ALSAEQAMountAbilityComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Save/ALSAEQASaveManager.h"

AALSAEQAMountActor::AALSAEQAMountActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    MountComponent = CreateDefaultSubobject<UALSAEQAMountComponent>(TEXT("MountComponent"));
    MountAbilityComponent = CreateDefaultSubobject<UALSAEQAMountAbilityComponent>(TEXT("MountAbilityComponent"));
    if (GetCharacterMovement())
    {
        BaseMovementSpeed = 650.0f;
        GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
        GetCharacterMovement()->JumpZVelocity = 650.0f;
    }
}

void AALSAEQAMountActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!Rider.IsValid() || !MountComponent || !MountComponent->IsMounted() || !bRiderSprinting)
    {
        SetActorTickEnabled(false);
        return;
    }
    if (!MountComponent->ConsumeStamina(StaminaPerSecondWhileSprinting * DeltaSeconds) || MountComponent->GetStamina() <= 0.0f)
    {
        DismountRider();
    }
}

bool AALSAEQAMountActor::InitializeWildMount(const FALSAEQAMountProfile& Profile)
{
    return MountComponent && MountComponent->InitializeWildMount(Profile);
}

bool AALSAEQAMountActor::BeginTaming()
{
    if (!MountComponent) return false;
    const FALSAEQAMountProfile Profile = MountComponent->GetMountProfile();
    UWorld* World = GetWorld();
    UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    if (SaveManager && SaveManager->GetStage() < FMath::Max(Profile.MinimumStage, 1)) return false;
    return MountComponent->BeginTaming();
}

bool AALSAEQAMountActor::AddTamingProgress(float Amount)
{
    return MountComponent && MountComponent->AddTamingProgress(Amount);
}

bool AALSAEQAMountActor::Tame(const FALSAEQAMountProfile& Profile)
{
    if (!MountComponent) return false;
    UWorld* World = GetWorld();
    UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    if (SaveManager && SaveManager->GetStage() < FMath::Max(Profile.MinimumStage, 1)) return false;
    if (!MountComponent->TameMount(Profile)) return false;
    if (SaveManager) SaveManager->SaveTamedMount(MountComponent->GetMountProfile());
    return true;
}

bool AALSAEQAMountActor::MountRider(AActor* NewRider)
{
    if (!NewRider || Rider.IsValid() || !MountComponent || !MountComponent->Mount()) return false;
    Rider = NewRider;
    bRiderSprinting = false;
    SetActorTickEnabled(false);
    NewRider->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    NewRider->SetActorRelativeLocation(RiderOffset);
    return true;
}

bool AALSAEQAMountActor::DismountRider()
{
    if (!Rider.IsValid())
    {
        bRiderSprinting = false;
        SetActorTickEnabled(false);
        return false;
    }
    AActor* CurrentRider = Rider.Get();
    if (MountComponent && MountComponent->IsMounted()) MountComponent->Dismount();
    Rider.Reset();
    bRiderSprinting = false;
    SetActorTickEnabled(false);
    if (GetCharacterMovement() && MountComponent)
    {
        const FALSAEQAMountProfile Profile = MountComponent->GetMountProfile();
        GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed * FMath::Max(Profile.MovementSpeedMultiplier, 0.1f);
    }
    CurrentRider->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    return true;
}

bool AALSAEQAMountActor::InjureMount()
{
    if (!MountComponent) return false;
    const bool bWasMounted = MountComponent->IsMounted();
    const bool bInjured = MountComponent->InjureMount();
    if (bWasMounted && Rider.IsValid()) DismountRider();
    return bInjured;
}

bool AALSAEQAMountActor::ReleaseMount()
{
    if (!MountComponent) return false;
    const bool bReleased = MountComponent->ReleaseMount();
    if (Rider.IsValid())
    {
        AActor* CurrentRider = Rider.Get();
        Rider.Reset();
        bRiderSprinting = false;
        SetActorTickEnabled(false);
        CurrentRider->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    }
    return bReleased;
}

bool AALSAEQAMountActor::MoveRiderForward(float Value)
{
    if (!Rider.IsValid() || !MountComponent || !MountComponent->IsMounted() || FMath::IsNearlyZero(Value)) return false;
    AddMovementInput(GetActorForwardVector(), FMath::Clamp(Value, -1.0f, 1.0f));
    return true;
}

bool AALSAEQAMountActor::MoveRiderRight(float Value)
{
    if (!Rider.IsValid() || !MountComponent || !MountComponent->IsMounted() || FMath::IsNearlyZero(Value)) return false;
    AddMovementInput(GetActorRightVector(), FMath::Clamp(Value, -1.0f, 1.0f));
    return true;
}

bool AALSAEQAMountActor::SetRiderSprint(bool bEnabled)
{
    if (!Rider.IsValid() || !MountComponent || !MountComponent->IsMounted())
    {
        bRiderSprinting = false;
        SetActorTickEnabled(false);
        if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
        return false;
    }
    bRiderSprinting = bEnabled;
    const FALSAEQAMountProfile Profile = MountComponent->GetMountProfile();
    const float ProfileMultiplier = FMath::Max(Profile.MovementSpeedMultiplier, 0.1f);
    const float SprintMultiplier = bRiderSprinting ? FMath::Max(SprintSpeedMultiplier, 1.0f) : 1.0f;
    if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed * ProfileMultiplier * SprintMultiplier;
    SetActorTickEnabled(bRiderSprinting);
    return true;
}
