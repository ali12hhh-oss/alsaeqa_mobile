#pragma once

#include "CoreMinimal.h"
#include "ALSAEQAPowerTypes.h"
#include "ALSAEQAProgressionDiscoveryTypes.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAProgressionRewardType : uint8
{
    Power,
    Weapon
};

USTRUCT(BlueprintType)
struct ALSAEQA_API FALSAEQAProgressionDiscovery
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName DiscoveryId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PrerequisiteDiscoveryId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EALSAEQAProgressionRewardType RewardType = EALSAEQAProgressionRewardType::Power;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EALSAEQAPower Power = EALSAEQAPower::ThunderSpark;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EALSAEQAWeapon Weapon = EALSAEQAWeapon::RustedBlade;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredStage = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName LocationTag;

    bool IsValid() const
    {
        return !DiscoveryId.IsNone() && RequiredStage > 0;
    }
};
