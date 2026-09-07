#include "Systems/ALSAEQAAbilityComponent.h"

UALSAEQAAbilityComponent::UALSAEQAAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    AbilityCosts.Add(EALSAEQAAbility::ThunderShock, 12.0f);
    AbilityCosts.Add(EALSAEQAAbility::LightningDash, 18.0f);
    AbilityCosts.Add(EALSAEQAAbility::ThunderJump, 15.0f);
    AbilityCosts.Add(EALSAEQAAbility::StormFist, 10.0f);
    AbilityCosts.Add(EALSAEQAAbility::LightningShield, 20.0f);
    AbilityCosts.Add(EALSAEQAAbility::StormSummon, 45.0f);
    AbilityCosts.Add(EALSAEQAAbility::ThunderEye, 8.0f);
    AbilityCosts.Add(EALSAEQAAbility::ThunderEcho, 6.0f);
}

void UALSAEQAAbilityComponent::BeginPlay()
{
    Super::BeginPlay();
    Energy = FMath::Clamp(Energy, 0.0f, MaxEnergy);
}

bool UALSAEQAAbilityComponent::UnlockAbility(EALSAEQAAbility Ability)
{
    return UnlockedAbilities.Add(Ability) > 0;
}

bool UALSAEQAAbilityComponent::HasAbility(EALSAEQAAbility Ability) const
{
    return UnlockedAbilities.Contains(Ability);
}

bool UALSAEQAAbilityComponent::ConsumeEnergy(float Amount)
{
    if (Amount <= 0.0f || Energy < Amount)
    {
        return false;
    }
    Energy -= Amount;
    return true;
}

bool UALSAEQAAbilityComponent::TryActivateAbility(EALSAEQAAbility Ability)
{
    if (!HasAbility(Ability))
    {
        return false;
    }
    const float* Cost = AbilityCosts.Find(Ability);
    return ConsumeEnergy(Cost ? *Cost : 0.0f);
}

void UALSAEQAAbilityComponent::RestoreEnergy(float Amount)
{
    Energy = FMath::Clamp(Energy + FMath::Max(0.0f, Amount), 0.0f, MaxEnergy);
}
