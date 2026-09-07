#pragma once

#include "CoreMinimal.h"
#include "ALSAEQADamageTypes.generated.h"

UENUM(BlueprintType)
enum class EALSAEQADamageType : uint8
{
    Physical UMETA(DisplayName="Physical"),
    Thunder UMETA(DisplayName="Thunder"),
    Fire UMETA(DisplayName="Fire"),
    Ice UMETA(DisplayName="Ice"),
    Poison UMETA(DisplayName="Poison")
};

UENUM(BlueprintType)
enum class EALSAEQADamageHitRegion : uint8
{
    Auto UMETA(DisplayName="Auto Detect"),
    Head UMETA(DisplayName="Head"),
    Torso UMETA(DisplayName="Torso"),
    LeftArm UMETA(DisplayName="Left Arm"),
    RightArm UMETA(DisplayName="Right Arm"),
    LeftLeg UMETA(DisplayName="Left Leg"),
    RightLeg UMETA(DisplayName="Right Leg")
};

USTRUCT(BlueprintType)
struct ALSAEQA_API FALSAEQADamageInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Amount = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EALSAEQADamageType Type = EALSAEQADamageType::Physical;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<AActor> Instigator = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector HitLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EALSAEQADamageHitRegion HitRegion = EALSAEQADamageHitRegion::Auto;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCritical = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bKnockout = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDismemberment = false;
};
