#include "Systems/ALSAEQAInjuryComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

UALSAEQAInjuryComponent::UALSAEQAInjuryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQAInjuryComponent::BeginPlay()
{
    Super::BeginPlay();
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
        {
            BaseWalkSpeed = Movement->MaxWalkSpeed;
        }
    }
}

void UALSAEQAInjuryComponent::ApplyBodyPartInjury(EALSAEQAInjuryBodyPart BodyPart, float Severity)
{
    if (State == EALSAEQAInjuryState::Dead || Severity <= 0.0f) return;
    BodyPartSeverity.FindOrAdd(BodyPart) = FMath::Clamp(GetBodyPartSeverity(BodyPart) + Severity, 0.0f, 1.0f);
    PlayInjuryPresentation(BodyPart, GetBodyPartSeverity(BodyPart));
    OnHitReaction.Broadcast(BodyPart);
    RecalculateState();
    ApplyOwnerStatePresentation();
}

void UALSAEQAInjuryComponent::ApplyOrganInjury(EALSAEQAInjuryOrgan Organ, float Severity)
{
    if (Organ == EALSAEQAInjuryOrgan::None || State == EALSAEQAInjuryState::Dead || Severity <= 0.0f) return;
    OrganSeverity.FindOrAdd(Organ) = FMath::Clamp(GetOrganSeverity(Organ) + Severity, 0.0f, 1.0f);
    PlayOrganInjuryPresentation(Organ, GetOrganSeverity(Organ));
    OnOrganInjured.Broadcast(Organ);
    if ((Organ == EALSAEQAInjuryOrgan::Heart || Organ == EALSAEQAInjuryOrgan::Lungs) && GetOrganSeverity(Organ) >= 1.0f)
    {
        MarkDead();
        return;
    }
    RecalculateState();
    ApplyOwnerStatePresentation();
}

void UALSAEQAInjuryComponent::SetLimbSevered(EALSAEQAInjuryBodyPart BodyPart, bool bSevered)
{
    const bool bLimb = BodyPart == EALSAEQAInjuryBodyPart::LeftArm || BodyPart == EALSAEQAInjuryBodyPart::RightArm || BodyPart == EALSAEQAInjuryBodyPart::LeftLeg || BodyPart == EALSAEQAInjuryBodyPart::RightLeg;
    if (!bLimb || State == EALSAEQAInjuryState::Dead) return;
    if (bSevered && !SeveredLimbs.Contains(BodyPart))
    {
        SeveredLimbs.Add(BodyPart);
        BodyPartSeverity.FindOrAdd(BodyPart) = 1.0f;
        ApplySeveredLimbVisual(BodyPart);
        PlayDismembermentPresentation(BodyPart);
        OnLimbChanged.Broadcast(BodyPart);
        RecalculateState();
        ApplyOwnerStatePresentation();
    }
    else if (!bSevered && SeveredLimbs.Remove(BodyPart) > 0)
    {
        RestoreLimbVisual(BodyPart);
        OnLimbChanged.Broadcast(BodyPart);
        RecalculateState();
        ApplyOwnerStatePresentation();
    }
}

void UALSAEQAInjuryComponent::SetKnockedOut(bool bValue)
{
    if (State == EALSAEQAInjuryState::Dead) return;
    if (bValue)
    {
        if (State != EALSAEQAInjuryState::KnockedOut)
        {
            State = EALSAEQAInjuryState::KnockedOut;
            ApplyOwnerStatePresentation();
            PlayKnockoutPresentation(true);
            OnInjuryStateChanged.Broadcast(State, 1.0f);
        }
    }
    else RecoverFromKnockout();
}

void UALSAEQAInjuryComponent::RecoverFromKnockout()
{
    if (State != EALSAEQAInjuryState::KnockedOut) return;
    PlayKnockoutPresentation(false);
    RecalculateState();
    ApplyOwnerStatePresentation();
}

void UALSAEQAInjuryComponent::ClearTemporaryInjuries()
{
    if (State == EALSAEQAInjuryState::Dead) return;
    BodyPartSeverity.Reset();
    OrganSeverity.Reset();
    State = SeveredLimbs.Num() > 0 ? EALSAEQAInjuryState::Injured : EALSAEQAInjuryState::Healthy;
    ApplyOwnerStatePresentation();
    OnInjuryStateChanged.Broadcast(State, 0.0f);
}

void UALSAEQAInjuryComponent::MarkDead()
{
    if (State == EALSAEQAInjuryState::Dead) return;
    State = EALSAEQAInjuryState::Dead;
    ApplyOwnerStatePresentation();
    OnInjuryStateChanged.Broadcast(State, 1.0f);
    PlayDeathPresentation();
    OnFatalState.Broadcast();
}

void UALSAEQAInjuryComponent::ResetAfterDeath()
{
    BodyPartSeverity.Reset();
    OrganSeverity.Reset();
    SeveredLimbs.Reset();
    State = EALSAEQAInjuryState::Healthy;
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
        {
            Movement->SetMovementMode(MOVE_Walking);
            if (BaseWalkSpeed > 0.0f) Movement->MaxWalkSpeed = BaseWalkSpeed;
        }
        if (USkeletalMeshComponent* Mesh = Character->GetMesh())
        {
            Mesh->SetSimulatePhysics(false);
            Mesh->SetCollisionProfileName(TEXT("CharacterMesh"));
            Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            Mesh->UnHideBoneByName(LeftArmBone);
            Mesh->UnHideBoneByName(RightArmBone);
            Mesh->UnHideBoneByName(LeftLegBone);
            Mesh->UnHideBoneByName(RightLegBone);
        }
        if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent()) Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
    OnInjuryStateChanged.Broadcast(State, 0.0f);
}

float UALSAEQAInjuryComponent::GetBodyPartSeverity(EALSAEQAInjuryBodyPart BodyPart) const
{
    if (const float* Value = BodyPartSeverity.Find(BodyPart)) return *Value;
    return 0.0f;
}

float UALSAEQAInjuryComponent::GetOrganSeverity(EALSAEQAInjuryOrgan Organ) const
{
    if (const float* Value = OrganSeverity.Find(Organ)) return *Value;
    return 0.0f;
}

bool UALSAEQAInjuryComponent::IsLimbSevered(EALSAEQAInjuryBodyPart BodyPart) const { return SeveredLimbs.Contains(BodyPart); }

float UALSAEQAInjuryComponent::GetMovementSpeedMultiplier() const
{
    if (State == EALSAEQAInjuryState::Dead || State == EALSAEQAInjuryState::KnockedOut) return 0.0f;
    const float LegSeverity = FMath::Max(GetBodyPartSeverity(EALSAEQAInjuryBodyPart::LeftLeg), GetBodyPartSeverity(EALSAEQAInjuryBodyPart::RightLeg));
    float Multiplier = 1.0f - 0.65f * LegSeverity;
    if (IsLimbSevered(EALSAEQAInjuryBodyPart::LeftLeg) || IsLimbSevered(EALSAEQAInjuryBodyPart::RightLeg)) Multiplier = FMath::Min(Multiplier, 0.35f);
    if (State == EALSAEQAInjuryState::Critical) Multiplier = FMath::Min(Multiplier, 0.55f);
    return FMath::Clamp(Multiplier, 0.0f, 1.0f);
}

float UALSAEQAInjuryComponent::GetCombatEffectivenessMultiplier() const
{
    if (State == EALSAEQAInjuryState::Dead || State == EALSAEQAInjuryState::KnockedOut) return 0.0f;
    const float ArmSeverity = FMath::Max(GetBodyPartSeverity(EALSAEQAInjuryBodyPart::LeftArm), GetBodyPartSeverity(EALSAEQAInjuryBodyPart::RightArm));
    float Multiplier = 1.0f - 0.55f * ArmSeverity;
    if (IsLimbSevered(EALSAEQAInjuryBodyPart::LeftArm) || IsLimbSevered(EALSAEQAInjuryBodyPart::RightArm)) Multiplier = FMath::Min(Multiplier, 0.45f);
    if (GetBodyPartSeverity(EALSAEQAInjuryBodyPart::Head) >= CriticalThreshold) Multiplier = FMath::Min(Multiplier, 0.5f);
    return FMath::Clamp(Multiplier, 0.0f, 1.0f);
}

void UALSAEQAInjuryComponent::RecalculateState()
{
    if (State == EALSAEQAInjuryState::Dead || State == EALSAEQAInjuryState::KnockedOut) return;
    float Highest = 0.0f;
    for (const TPair<EALSAEQAInjuryBodyPart, float>& Pair : BodyPartSeverity) Highest = FMath::Max(Highest, Pair.Value);
    for (const TPair<EALSAEQAInjuryOrgan, float>& Pair : OrganSeverity) Highest = FMath::Max(Highest, Pair.Value);
    EALSAEQAInjuryState NewState = EALSAEQAInjuryState::Healthy;
    if (Highest >= CriticalThreshold) NewState = EALSAEQAInjuryState::Critical;
    else if (Highest > 0.0f) NewState = EALSAEQAInjuryState::Injured;
    if (Highest >= KnockoutThreshold) NewState = EALSAEQAInjuryState::KnockedOut;
    if (NewState != State)
    {
        State = NewState;
        ApplyOwnerStatePresentation();
        OnInjuryStateChanged.Broadcast(State, Highest);
        if (State == EALSAEQAInjuryState::KnockedOut) PlayKnockoutPresentation(true);
    }
}

void UALSAEQAInjuryComponent::ApplyOwnerStatePresentation()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;
    UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
    if (!Movement) return;

    if (State == EALSAEQAInjuryState::KnockedOut || State == EALSAEQAInjuryState::Dead)
    {
        Movement->StopMovementImmediately();
        if (bAutoDisableMovementOnKnockout || State == EALSAEQAInjuryState::Dead) Movement->DisableMovement();
        if (State == EALSAEQAInjuryState::Dead && bAutoRagdollOnDeath)
        {
            if (USkeletalMeshComponent* Mesh = Character->GetMesh())
            {
                Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
                Mesh->SetSimulatePhysics(true);
                if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent()) Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
        }
        return;
    }

    if (BaseWalkSpeed > 0.0f) Movement->MaxWalkSpeed = BaseWalkSpeed * GetMovementSpeedMultiplier();
}

void UALSAEQAInjuryComponent::ApplySeveredLimbVisual(EALSAEQAInjuryBodyPart BodyPart)
{
    if (!bAutoHideSeveredLimbBones) return;
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    USkeletalMeshComponent* Mesh = Character ? Character->GetMesh() : nullptr;
    if (!Mesh) return;
    FName Bone = NAME_None;
    switch (BodyPart)
    {
        case EALSAEQAInjuryBodyPart::LeftArm: Bone = LeftArmBone; break;
        case EALSAEQAInjuryBodyPart::RightArm: Bone = RightArmBone; break;
        case EALSAEQAInjuryBodyPart::LeftLeg: Bone = LeftLegBone; break;
        case EALSAEQAInjuryBodyPart::RightLeg: Bone = RightLegBone; break;
        default: break;
    }
    if (!Bone.IsNone() && Mesh->GetBoneIndex(Bone) != INDEX_NONE) Mesh->HideBoneByName(Bone, EPhysBodyOp::PBO_Term);
}

void UALSAEQAInjuryComponent::RestoreLimbVisual(EALSAEQAInjuryBodyPart BodyPart)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    USkeletalMeshComponent* Mesh = Character ? Character->GetMesh() : nullptr;
    if (!Mesh) return;
    FName Bone = NAME_None;
    switch (BodyPart)
    {
        case EALSAEQAInjuryBodyPart::LeftArm: Bone = LeftArmBone; break;
        case EALSAEQAInjuryBodyPart::RightArm: Bone = RightArmBone; break;
        case EALSAEQAInjuryBodyPart::LeftLeg: Bone = LeftLegBone; break;
        case EALSAEQAInjuryBodyPart::RightLeg: Bone = RightLegBone; break;
        default: break;
    }
    if (!Bone.IsNone() && Mesh->GetBoneIndex(Bone) != INDEX_NONE) Mesh->UnHideBoneByName(Bone);
}
