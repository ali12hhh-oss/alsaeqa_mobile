#include "Combat/ALSAEQAMeleeCombatComponent.h"
#include "Systems/ALSAEQAHealthComponent.h"
#include "Systems/ALSAEQAInjuryComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UALSAEQAMeleeCombatComponent::UALSAEQAMeleeCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQAMeleeCombatComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool UALSAEQAMeleeCombatComponent::CanAttack() const
{
    const UWorld* World = GetWorld();
    return World && (World->GetTimeSeconds() - LastAttackTime) >= AttackCooldown;
}

bool UALSAEQAMeleeCombatComponent::LightAttack()
{
    if (!CanAttack()) return false;
    if (const AActor* Owner = GetOwner())
    {
        if (const UALSAEQAInjuryComponent* Injury = Owner->FindComponentByClass<UALSAEQAInjuryComponent>())
        {
            if (Injury->GetCombatEffectivenessMultiplier() <= 0.0f) return false;
        }
    }
    LastAttackTime = GetWorld()->GetTimeSeconds();
    OnAttackStarted.Broadcast(false);
    return true;
}

bool UALSAEQAMeleeCombatComponent::HeavyAttack()
{
    if (!CanAttack()) return false;
    if (const AActor* Owner = GetOwner())
    {
        if (const UALSAEQAInjuryComponent* Injury = Owner->FindComponentByClass<UALSAEQAInjuryComponent>())
        {
            if (Injury->GetCombatEffectivenessMultiplier() <= 0.0f) return false;
        }
    }
    LastAttackTime = GetWorld()->GetTimeSeconds();
    OnAttackStarted.Broadcast(true);
    return true;
}

bool UALSAEQAMeleeCombatComponent::TryHitActor(AActor* Target, float Damage, EALSAEQADamageType DamageType)
{
    if (!IsValid(Target) || Target == GetOwner() || Damage <= 0.0f) return false;

    UALSAEQAHealthComponent* Health = Target->FindComponentByClass<UALSAEQAHealthComponent>();
    if (!Health || Health->IsDead()) return false;

    float AttackerMultiplier = 1.0f;
    if (const AActor* Owner = GetOwner())
    {
        if (const UALSAEQAInjuryComponent* Injury = Owner->FindComponentByClass<UALSAEQAInjuryComponent>())
        {
            AttackerMultiplier = Injury->GetCombatEffectivenessMultiplier();
        }
    }
    if (AttackerMultiplier <= 0.0f) return false;

    FALSAEQADamageInfo Info;
    Info.Amount = Damage * AttackerMultiplier;
    Info.Type = DamageType;
    Info.Instigator = GetOwner();
    Info.HitLocation = Target->GetActorLocation();
    Info.HitRegion = EALSAEQADamageHitRegion::Auto;
    Info.bCritical = Damage >= HeavyDamage;
    Info.bKnockout = Damage >= HeavyDamage * 1.5f;
    Health->ApplyDamageInfo(Info);
    return true;
}
