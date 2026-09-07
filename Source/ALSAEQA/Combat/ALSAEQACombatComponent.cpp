#include "Combat/ALSAEQACombatComponent.h"
#include "Engine/World.h"

UALSAEQACombatComponent::UALSAEQACombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UALSAEQACombatComponent::StartAttack(EALSAEQAAttackType AttackType)
{
    if (bAttacking || !GetWorld())
    {
        return false;
    }

    const float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastAttackTime < AttackCooldown)
    {
        return false;
    }

    CurrentAttack = AttackType;
    bAttacking = true;
    LastAttackTime = Now;
    OnAttackStarted.Broadcast(CurrentAttack);
    return true;
}

void UALSAEQACombatComponent::EndAttack()
{
    bAttacking = false;
}
