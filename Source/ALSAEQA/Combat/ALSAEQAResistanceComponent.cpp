#include "Combat/ALSAEQAResistanceComponent.h"

UALSAEQAResistanceComponent::UALSAEQAResistanceComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    DamageMultipliers.Add(EALSAEQADamageType::Physical, 1.0f);
    DamageMultipliers.Add(EALSAEQADamageType::Thunder, 1.0f);
    DamageMultipliers.Add(EALSAEQADamageType::Fire, 1.0f);
    DamageMultipliers.Add(EALSAEQADamageType::Ice, 1.0f);
    DamageMultipliers.Add(EALSAEQADamageType::Poison, 1.0f);
}

float UALSAEQAResistanceComponent::GetMultiplier(EALSAEQADamageType DamageType) const
{
    if (const float* Value = DamageMultipliers.Find(DamageType))
    {
        return FMath::Clamp(*Value, 0.0f, 2.0f);
    }

    return 1.0f;
}

float UALSAEQAResistanceComponent::ModifyDamage(const FALSAEQADamageInfo& DamageInfo) const
{
    if (DamageInfo.Amount <= 0.0f)
    {
        return 0.0f;
    }

    return DamageInfo.Amount * GetMultiplier(DamageInfo.Type);
}

void UALSAEQAResistanceComponent::SetMultiplier(EALSAEQADamageType DamageType, float Multiplier)
{
    DamageMultipliers.FindOrAdd(DamageType) = FMath::Clamp(Multiplier, 0.0f, 2.0f);
}
