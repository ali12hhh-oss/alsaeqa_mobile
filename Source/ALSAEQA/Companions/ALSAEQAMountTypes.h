#pragma once

#include "CoreMinimal.h"
#include "ALSAEQAMountTypes.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAMountType : uint8
{
    Horse,
    Beast,
    ThunderBeast,
    FlyingBeast
};

UENUM(BlueprintType)
enum class EALSAEQAMountState : uint8
{
    Wild,
    Taming,
    Tamed,
    Mounted,
    Injured,
    Released
};

USTRUCT(BlueprintType)
struct FALSAEQAMountProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FName MountId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    EALSAEQAMountType Type = EALSAEQAMountType::Horse;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    int32 MinimumStage = 45;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float MovementSpeedMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float Stamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float TamingProgress = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float TamingRequired = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    bool bCanCombat = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    bool bCanTraverseRoughTerrain = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    bool bCanUseThunderInteractions = false;
};
