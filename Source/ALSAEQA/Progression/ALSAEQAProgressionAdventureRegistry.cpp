#include "Progression/ALSAEQAProgressionAdventureRegistry.h"

namespace
{
    FALSAEQAProgressionDiscovery MakePower(FName Id, int32 Stage, EALSAEQAPower Power, FName Location, FName Prerequisite = NAME_None)
    {
        FALSAEQAProgressionDiscovery D;
        D.DiscoveryId = Id;
        D.RequiredStage = Stage;
        D.RewardType = EALSAEQAProgressionRewardType::Power;
        D.Power = Power;
        D.LocationTag = Location;
        D.PrerequisiteDiscoveryId = Prerequisite;
        return D;
    }

    FALSAEQAProgressionDiscovery MakeWeapon(FName Id, int32 Stage, EALSAEQAWeapon Weapon, FName Location, FName Prerequisite = NAME_None)
    {
        FALSAEQAProgressionDiscovery D;
        D.DiscoveryId = Id;
        D.RequiredStage = Stage;
        D.RewardType = EALSAEQAProgressionRewardType::Weapon;
        D.Weapon = Weapon;
        D.LocationTag = Location;
        D.PrerequisiteDiscoveryId = Prerequisite;
        return D;
    }
}

namespace ALSAEQAProgressionAdventureRegistry
{
    const TArray<FALSAEQAProgressionDiscovery>& GetDiscoveries()
    {
        static const TArray<FALSAEQAProgressionDiscovery> Discoveries = {
            MakePower(TEXT("Power_ThunderSpark_HeirAwakening"), 1, EALSAEQAPower::ThunderSpark, TEXT("SlaveMines")),
            MakeWeapon(TEXT("Weapon_RustedBlade_MineEscape"), 1, EALSAEQAWeapon::RustedBlade, TEXT("SlaveMines"), TEXT("Power_ThunderSpark_HeirAwakening")),
            MakePower(TEXT("Power_LightningDash_CliffTrial"), 2, EALSAEQAPower::LightningDash, TEXT("ShadowForest"), TEXT("Weapon_RustedBlade_MineEscape")),
            MakeWeapon(TEXT("Weapon_HunterDagger_ShadowForest"), 2, EALSAEQAWeapon::HunterDagger, TEXT("ShadowForest"), TEXT("Power_LightningDash_CliffTrial")),
            MakePower(TEXT("Power_ThunderJump_MountainRift"), 3, EALSAEQAPower::ThunderJump, TEXT("ThunderMountains"), TEXT("Weapon_HunterDagger_ShadowForest")),
            MakeWeapon(TEXT("Weapon_IronSpear_ThunderCave"), 3, EALSAEQAWeapon::IronSpear, TEXT("ThunderCaves"), TEXT("Power_ThunderJump_MountainRift")),
            MakePower(TEXT("Power_StormFist_GuardianTrial"), 4, EALSAEQAPower::StormFist, TEXT("MonsterValley"), TEXT("Weapon_IronSpear_ThunderCave")),
            MakePower(TEXT("Power_LightningShield_AncientGate"), 5, EALSAEQAPower::LightningShield, TEXT("CursedCaves"), TEXT("Power_StormFist_GuardianTrial")),
            MakeWeapon(TEXT("Weapon_ThunderHammer_CursedForge"), 5, EALSAEQAWeapon::ThunderHammer, TEXT("CursedForge"), TEXT("Power_LightningShield_AncientGate")),
            MakePower(TEXT("Power_ThunderEye_SecretTemple"), 6, EALSAEQAPower::ThunderEye, TEXT("AbandonedCity"), TEXT("Weapon_ThunderHammer_CursedForge")),
            MakePower(TEXT("Power_ThunderEcho_FamilyMemory"), 7, EALSAEQAPower::ThunderEcho, TEXT("AncientRuins"), TEXT("Power_ThunderEye_SecretTemple")),
            MakeWeapon(TEXT("Weapon_StormSpear_AncientTrial"), 7, EALSAEQAWeapon::StormSpear, TEXT("AncientTrial"), TEXT("Power_ThunderEcho_FamilyMemory")),
            MakePower(TEXT("Power_StormSummon_StormTrial"), 8, EALSAEQAPower::StormSummon, TEXT("BlackDesert"), TEXT("Weapon_StormSpear_AncientTrial")),
            MakeWeapon(TEXT("Weapon_ThunderBlade_GiantLegacy"), 9, EALSAEQAWeapon::ThunderBlade, TEXT("LandOfGiants"), TEXT("Power_StormSummon_StormTrial"))
        };
        return Discoveries;
    }
}
