#pragma once

#include "CoreMinimal.h"
#include "ALSAEQAPowerTypes.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAPower : uint8
{
    ThunderSpark,
    LightningDash,
    ThunderJump,
    StormFist,
    LightningShield,
    ThunderEye,
    ThunderEcho,
    StormSummon
};

UENUM(BlueprintType)
enum class EALSAEQAWeapon : uint8
{
    RustedBlade,
    HunterDagger,
    IronSpear,
    ThunderHammer,
    StormSpear,
    ThunderBlade
};

USTRUCT(BlueprintType)
struct ALSAEQA_API FALSAEQAPowerUnlock
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EALSAEQAPower Power = EALSAEQAPower::ThunderSpark;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredStage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName DiscoveryId;
};

USTRUCT(BlueprintType)
struct ALSAEQA_API FALSAEQAWeaponUnlock
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EALSAEQAWeapon Weapon = EALSAEQAWeapon::RustedBlade;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredStage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName DiscoveryId;
};
