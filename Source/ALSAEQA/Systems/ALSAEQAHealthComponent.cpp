#include "Systems/ALSAEQAHealthComponent.h"
#include "Systems/ALSAEQAInjuryComponent.h"
#include "GameFramework/Actor.h"

UALSAEQAHealthComponent::UALSAEQAHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQAHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner) return;

    InjuryComponent = Owner->FindComponentByClass<UALSAEQAInjuryComponent>();
    if (!InjuryComponent)
    {
        InjuryComponent = NewObject<UALSAEQAInjuryComponent>(Owner, TEXT("UniversalInjuryComponent"));
        if (InjuryComponent) InjuryComponent->RegisterComponent();
    }

    if (InjuryComponent)
    {
        InjuryComponent->OnFatalState.AddDynamic(this, &UALSAEQAHealthComponent::HandleInjuryDeath);
    }
}

void UALSAEQAHealthComponent::ApplyDamage(float Damage)
{
    if (bDead || Damage <= 0.0f) return;

    FALSAEQADamageInfo Info;
    Info.Amount = Damage;
    Info.Type = EALSAEQADamageType::Physical;
    Info.Instigator = nullptr;
    Info.HitLocation = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
    ApplyDamageInfo(Info);
}

void UALSAEQAHealthComponent::ApplyDamageInfo(const FALSAEQADamageInfo& DamageInfo)
{
    if (bDead || DamageInfo.Amount <= 0.0f) return;

    ApplyInjuryFromDamage(DamageInfo);
    Health = FMath::Clamp(Health - DamageInfo.Amount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health, MaxHealth);

    if (Health <= 0.0f)
    {
        if (InjuryComponent) InjuryComponent->MarkDead();
        if (!bDead)
        {
            bDead = true;
            OnDeath.Broadcast();
        }
    }
}

void UALSAEQAHealthComponent::ApplyInjuryFromDamage(const FALSAEQADamageInfo& DamageInfo)
{
    if (!InjuryComponent || !GetOwner()) return;

    const float Severity = FMath::Clamp(DamageInfo.Amount / FMath::Max(MaxHealth, 1.0f) * 2.0f, 0.05f, 1.0f);
    EALSAEQAInjuryBodyPart Part = EALSAEQAInjuryBodyPart::Torso;

    switch (DamageInfo.HitRegion)
    {
        case EALSAEQADamageHitRegion::Head: Part = EALSAEQAInjuryBodyPart::Head; break;
        case EALSAEQADamageHitRegion::Torso: Part = EALSAEQAInjuryBodyPart::Torso; break;
        case EALSAEQADamageHitRegion::LeftArm: Part = EALSAEQAInjuryBodyPart::LeftArm; break;
        case EALSAEQADamageHitRegion::RightArm: Part = EALSAEQAInjuryBodyPart::RightArm; break;
        case EALSAEQADamageHitRegion::LeftLeg: Part = EALSAEQAInjuryBodyPart::LeftLeg; break;
        case EALSAEQADamageHitRegion::RightLeg: Part = EALSAEQAInjuryBodyPart::RightLeg; break;
        case EALSAEQADamageHitRegion::Auto:
        default:
        {
            const FVector LocalHit = GetOwner()->GetActorTransform().InverseTransformPosition(DamageInfo.HitLocation);
            const float Height = FMath::Max(GetOwner()->GetComponentsBoundingBox(true).Extent.Z, 1.0f);
            const float NormalizedHeight = LocalHit.Z / Height;
            const float Lateral = FMath::Abs(LocalHit.Y);
            if (NormalizedHeight > 0.68f)
            {
                Part = EALSAEQAInjuryBodyPart::Head;
            }
            else if (NormalizedHeight < -0.35f)
            {
                Part = LocalHit.Y >= 0.0f ? EALSAEQAInjuryBodyPart::RightLeg : EALSAEQAInjuryBodyPart::LeftLeg;
            }
            else if (Lateral > Height * 0.55f)
            {
                Part = LocalHit.Y >= 0.0f ? EALSAEQAInjuryBodyPart::RightArm : EALSAEQAInjuryBodyPart::LeftArm;
            }
            break;
        }
    }

    InjuryComponent->ApplyBodyPartInjury(Part, Severity);

    if (DamageInfo.bDismemberment &&
        (Part == EALSAEQAInjuryBodyPart::LeftArm || Part == EALSAEQAInjuryBodyPart::RightArm ||
         Part == EALSAEQAInjuryBodyPart::LeftLeg || Part == EALSAEQAInjuryBodyPart::RightLeg))
    {
        InjuryComponent->SetLimbSevered(Part);
    }

    if (DamageInfo.bKnockout || DamageInfo.bCritical || Severity >= 0.90f)
    {
        if (DamageInfo.bKnockout || Severity >= 0.90f)
        {
            InjuryComponent->SetKnockedOut();
        }
    }

    if (Part == EALSAEQAInjuryBodyPart::Torso && DamageInfo.Amount >= 35.0f)
    {
        InjuryComponent->ApplyOrganInjury(EALSAEQAInjuryOrgan::Lungs, FMath::Clamp(Severity * 0.75f, 0.1f, 0.9f));
    }
}

void UALSAEQAHealthComponent::Heal(float Amount)
{
    if (bDead || Amount <= 0.0f) return;
    Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UALSAEQAHealthComponent::ResetHealth()
{
    bDead = false;
    Health = MaxHealth;
    if (InjuryComponent) InjuryComponent->ResetAfterDeath();
    OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UALSAEQAHealthComponent::HandleInjuryDeath()
{
    if (bDead) return;
    bDead = true;
    Health = 0.0f;
    OnHealthChanged.Broadcast(Health, MaxHealth);
    OnDeath.Broadcast();
}

float UALSAEQAHealthComponent::GetHealthPercent() const
{
    return MaxHealth > KINDA_SMALL_NUMBER ? Health / MaxHealth : 0.0f;
}
