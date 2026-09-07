#include "Progression/ALSAEQAProgressionComponent.h"
#include "Progression/ALSAEQAProgressionStageRegistry.h"

namespace
{
    int32 PowerStage(EALSAEQAPower Power)
    {
        switch (Power)
        {
        case EALSAEQAPower::ThunderSpark:    return 1;
        case EALSAEQAPower::LightningDash:   return 2;
        case EALSAEQAPower::ThunderJump:     return 3;
        case EALSAEQAPower::StormFist:       return 4;
        case EALSAEQAPower::LightningShield: return 5;
        case EALSAEQAPower::ThunderEye:      return 6;
        case EALSAEQAPower::ThunderEcho:     return 7;
        case EALSAEQAPower::StormSummon:     return 8;
        default:                             return MAX_int32;
        }
    }

    int32 WeaponStage(EALSAEQAWeapon Weapon)
    {
        switch (Weapon)
        {
        case EALSAEQAWeapon::RustedBlade:  return 1;
        case EALSAEQAWeapon::HunterDagger: return 2;
        case EALSAEQAWeapon::IronSpear:    return 3;
        case EALSAEQAWeapon::ThunderHammer:return 5;
        case EALSAEQAWeapon::StormSpear:   return 7;
        case EALSAEQAWeapon::ThunderBlade: return 9;
        default:                            return MAX_int32;
        }
    }
}

UALSAEQAProgressionComponent::UALSAEQAProgressionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UALSAEQAProgressionComponent::UnlockPower(EALSAEQAPower Power, FName DiscoveryId)
{
    if (DiscoveryId.IsNone() || HasCompletedDiscovery(DiscoveryId) || UnlockedPowers.Contains(Power) || !IsPowerAvailable(Power))
    {
        return false;
    }

    UnlockedPowers.Add(Power);
    CompletedDiscoveryIds.Add(DiscoveryId);
    OnProgressionChanged.ExecuteIfBound();
    return true;
}

bool UALSAEQAProgressionComponent::AcquireWeapon(EALSAEQAWeapon Weapon, FName DiscoveryId)
{
    if (DiscoveryId.IsNone() || HasCompletedDiscovery(DiscoveryId) || AcquiredWeapons.Contains(Weapon) || !IsWeaponAvailable(Weapon))
    {
        return false;
    }

    AcquiredWeapons.Add(Weapon);
    CompletedDiscoveryIds.Add(DiscoveryId);
    OnProgressionChanged.ExecuteIfBound();
    return true;
}

bool UALSAEQAProgressionComponent::AdvanceStage(int32 NewStage)
{
    if (!IsValidStage(NewStage) || NewStage <= CurrentStage)
    {
        return false;
    }

    CurrentStage = NewStage;
    OnProgressionChanged.ExecuteIfBound();
    return true;
}

bool UALSAEQAProgressionComponent::IsValidStage(int32 StageNumber) const
{
    return ALSAEQAProgressionStageRegistry::IsValidStage(StageNumber);
}

FALSAEQAProgressionStage UALSAEQAProgressionComponent::GetStageDefinition(int32 StageNumber) const
{
    if (const FALSAEQAProgressionStage* Stage = ALSAEQAProgressionStageRegistry::FindStage(StageNumber))
    {
        return *Stage;
    }
    return FALSAEQAProgressionStage();
}

bool UALSAEQAProgressionComponent::HasPower(EALSAEQAPower Power) const
{
    return UnlockedPowers.Contains(Power);
}

bool UALSAEQAProgressionComponent::HasWeapon(EALSAEQAWeapon Weapon) const
{
    return AcquiredWeapons.Contains(Weapon);
}

bool UALSAEQAProgressionComponent::IsPowerAvailable(EALSAEQAPower Power) const
{
    return CurrentStage >= PowerStage(Power);
}

bool UALSAEQAProgressionComponent::IsWeaponAvailable(EALSAEQAWeapon Weapon) const
{
    return CurrentStage >= WeaponStage(Weapon);
}

int32 UALSAEQAProgressionComponent::GetPowerRequiredStage(EALSAEQAPower Power) const
{
    return PowerStage(Power);
}

int32 UALSAEQAProgressionComponent::GetWeaponRequiredStage(EALSAEQAWeapon Weapon) const
{
    return WeaponStage(Weapon);
}

bool UALSAEQAProgressionComponent::HasCompletedDiscovery(FName DiscoveryId) const
{
    return !DiscoveryId.IsNone() && CompletedDiscoveryIds.Contains(DiscoveryId);
}

void UALSAEQAProgressionComponent::ResetProgression()
{
    CurrentStage = 1;
    UnlockedPowers.Reset();
    AcquiredWeapons.Reset();
    CompletedDiscoveryIds.Reset();
    OnProgressionChanged.ExecuteIfBound();
}
