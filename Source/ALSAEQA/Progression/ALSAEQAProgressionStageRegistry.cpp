#include "Progression/ALSAEQAProgressionStageRegistry.h"

namespace
{
    FALSAEQAProgressionStage MakeStage(int32 Number, const TCHAR* Id, const TCHAR* Title, bool bFamilySearch = false, bool bMount = false, bool bThunderBeast = false)
    {
        FALSAEQAProgressionStage Stage;
        Stage.StageNumber = Number;
        Stage.StageId = FName(Id);
        Stage.Title = FText::FromString(Title);
        Stage.NextStage = Number < 70 ? Number + 1 : 0;
        Stage.bCompanionFamilySearchStage = bFamilySearch;
        Stage.bMountStage = bMount;
        Stage.bThunderBeastStage = bThunderBeast;
        return Stage;
    }
}

namespace ALSAEQAProgressionStageRegistry
{
    const TArray<FALSAEQAProgressionStage>& GetStages()
    {
        static const TArray<FALSAEQAProgressionStage> Stages = {
            MakeStage(1,  TEXT("Stage_01_ChainsInTheMine"), TEXT("Chains in the Mine")),
            MakeStage(2,  TEXT("Stage_02_TheHiddenMark"), TEXT("The Hidden Mark")),
            MakeStage(3,  TEXT("Stage_03_EchoUnderStone"), TEXT("Echo Under Stone")),
            MakeStage(4,  TEXT("Stage_04_BrokenLift"), TEXT("Broken Lift")),
            MakeStage(5,  TEXT("Stage_05_TheCaptivesTrail"), TEXT("The Captive’s Trail")),
            MakeStage(6,  TEXT("Stage_06_ForestOfWhispers"), TEXT("Forest of Whispers")),
            MakeStage(7,  TEXT("Stage_07_TheOldShrine"), TEXT("The Old Shrine")),
            MakeStage(8,  TEXT("Stage_08_BeastAtThePass"), TEXT("Beast at the Pass")),
            MakeStage(9,  TEXT("Stage_09_StormOnTheRidge"), TEXT("Storm on the Ridge")),
            MakeStage(10, TEXT("Stage_10_TheStormThread"), TEXT("The Storm Thread")),
            MakeStage(11, TEXT("Stage_11_HunterOfTheMist"), TEXT("Hunter of the Mist")),
            MakeStage(12, TEXT("Stage_12_LivingMountain"), TEXT("Living Mountain")),
            MakeStage(13, TEXT("Stage_13_ChainOfTheColossus"), TEXT("Chain of the Colossus")),
            MakeStage(14, TEXT("Stage_14_StormWithin"), TEXT("Storm Within")),
            MakeStage(15, TEXT("Stage_15_FallenCity"), TEXT("Fallen City")),
            MakeStage(16, TEXT("Stage_16_CompanionsSecret"), TEXT("Companion’s Secret")),
            MakeStage(17, TEXT("Stage_17_BlackDesert"), TEXT("Black Desert")),
            MakeStage(18, TEXT("Stage_18_FrozenKingdom"), TEXT("Frozen Kingdom")),
            MakeStage(19, TEXT("Stage_19_EchoOfAncestors"), TEXT("Echo of Ancestors")),
            MakeStage(20, TEXT("Stage_20_LandOfGiants"), TEXT("Land of Giants")),
            MakeStage(21, TEXT("Stage_21_AshesOfTheOldRoad"), TEXT("Ashes of the Old Road")),
            MakeStage(22, TEXT("Stage_22_TheSilentPrison"), TEXT("The Silent Prison")),
            MakeStage(23, TEXT("Stage_23_TheBrokenSeal"), TEXT("The Broken Seal")),
            MakeStage(24, TEXT("Stage_24_BeforeTheTruth"), TEXT("Before the Truth")),
            MakeStage(25, TEXT("Stage_25_FirstConfirmedTrace"), TEXT("First Confirmed Trace"), true),
            MakeStage(26, TEXT("Stage_26_AbandonedCaravan"), TEXT("Abandoned Caravan"), true),
            MakeStage(27, TEXT("Stage_27_BrokenSymbol"), TEXT("Broken Symbol"), true),
            MakeStage(28, TEXT("Stage_28_HiddenWitness"), TEXT("Hidden Witness"), true),
            MakeStage(29, TEXT("Stage_29_PrisonRecord"), TEXT("Prison Record"), true),
            MakeStage(30, TEXT("Stage_30_TheSplitPaths"), TEXT("The Split Paths"), true),
            MakeStage(31, TEXT("Stage_31_HiddenRefuge"), TEXT("Hidden Refuge"), true),
            MakeStage(32, TEXT("Stage_32_SurvivorMessage"), TEXT("Survivor Message"), true),
            MakeStage(33, TEXT("Stage_33_EnemyNetwork"), TEXT("Enemy Network"), true),
            MakeStage(34, TEXT("Stage_34_MountainRoad"), TEXT("Mountain Road"), true),
            MakeStage(35, TEXT("Stage_35_FalseGrave"), TEXT("False Grave"), true),
            MakeStage(36, TEXT("Stage_36_SecondCaravan"), TEXT("Second Caravan"), true),
            MakeStage(37, TEXT("Stage_37_SurvivorRoad"), TEXT("Survivor Road"), true),
            MakeStage(38, TEXT("Stage_38_FamilyStronghold"), TEXT("Family Stronghold"), true),
            MakeStage(39, TEXT("Stage_39_TheSeparation"), TEXT("The Separation"), true),
            MakeStage(40, TEXT("Stage_40_TheFamily"), TEXT("The Family"), true),
            MakeStage(41, TEXT("Stage_41_TwoHistories"), TEXT("Two Histories")),
            MakeStage(42, TEXT("Stage_42_TheEmptyCrest"), TEXT("The Empty Crest")),
            MakeStage(43, TEXT("Stage_43_BloodInTheArchive"), TEXT("Blood in the Archive")),
            MakeStage(44, TEXT("Stage_44_RoadOfTheHeirs"), TEXT("Road of the Heirs")),
            MakeStage(45, TEXT("Stage_45_FirstCreatureTraces"), TEXT("First Creature Traces"), false, true),
            MakeStage(46, TEXT("Stage_46_Trust"), TEXT("Trust"), false, true),
            MakeStage(47, TEXT("Stage_47_FirstTame"), TEXT("First Tame"), false, true),
            MakeStage(48, TEXT("Stage_48_FirstRide"), TEXT("First Ride"), false, true),
            MakeStage(49, TEXT("Stage_49_RoughTerrain"), TEXT("Rough Terrain"), false, true),
            MakeStage(50, TEXT("Stage_50_TheChase"), TEXT("The Chase"), false, true),
            MakeStage(51, TEXT("Stage_51_RescueByMount"), TEXT("Rescue by Mount"), false, true),
            MakeStage(52, TEXT("Stage_52_SideRoute"), TEXT("Side Route"), false, true),
            MakeStage(53, TEXT("Stage_53_TrustTest"), TEXT("Trust Test"), false, true),
            MakeStage(54, TEXT("Stage_54_CompanionRides"), TEXT("Companion Rides"), false, true),
            MakeStage(55, TEXT("Stage_55_GreatRoute"), TEXT("Great Route"), false, true),
            MakeStage(56, TEXT("Stage_56_StormTracks"), TEXT("Storm Tracks"), false, false, true),
            MakeStage(57, TEXT("Stage_57_HeartOfTheStorm"), TEXT("Heart of the Storm"), false, false, true),
            MakeStage(58, TEXT("Stage_58_AncientHerd"), TEXT("Ancient Herd"), false, false, true),
            MakeStage(59, TEXT("Stage_59_TheBond"), TEXT("The Bond"), false, false, true),
            MakeStage(60, TEXT("Stage_60_LightningDash"), TEXT("Lightning Dash"), false, false, true),
            MakeStage(61, TEXT("Stage_61_ThunderRoar"), TEXT("Thunder Roar"), false, false, true),
            MakeStage(62, TEXT("Stage_62_LightningKick"), TEXT("Lightning Kick"), false, false, true),
            MakeStage(63, TEXT("Stage_63_StormCharge"), TEXT("Storm Charge"), false, false, true),
            MakeStage(64, TEXT("Stage_64_LightningCrossing"), TEXT("Lightning Crossing"), false, false, true),
            MakeStage(65, TEXT("Stage_65_StormLeap"), TEXT("Storm Leap"), false, false, true),
            MakeStage(66, TEXT("Stage_66_LightningShield"), TEXT("Lightning Shield"), false, false, true),
            MakeStage(67, TEXT("Stage_67_StormSummon"), TEXT("Storm Summon"), false, false, true),
            MakeStage(68, TEXT("Stage_68_ThunderSense"), TEXT("Thunder Sense"), false, false, true),
            MakeStage(69, TEXT("Stage_69_StormMode"), TEXT("Storm Mode"), false, false, true),
            MakeStage(70, TEXT("Stage_70_HeirsOfTheStorm"), TEXT("Heirs of the Storm"), false, false, true)
        };
        return Stages;
    }

    const FALSAEQAProgressionStage* FindStage(int32 StageNumber)
    {
        if (StageNumber < 1 || StageNumber > 70)
        {
            return nullptr;
        }
        const TArray<FALSAEQAProgressionStage>& Stages = GetStages();
        return Stages.IsValidIndex(StageNumber - 1) ? &Stages[StageNumber - 1] : nullptr;
    }

    bool IsValidStage(int32 StageNumber)
    {
        return FindStage(StageNumber) != nullptr;
    }
}
