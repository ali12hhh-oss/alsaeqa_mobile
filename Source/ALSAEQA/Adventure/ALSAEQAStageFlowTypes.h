#pragma once

#include "CoreMinimal.h"
#include "ALSAEQAStageFlowTypes.generated.h"

USTRUCT(BlueprintType)
struct FALSAEQAStageBeat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName BeatId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName StageId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName NextStageId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Title;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Description;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bPlayerControl = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEndsStage = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bCanRepeatAfterProgress = true;
};

USTRUCT(BlueprintType)
struct FALSAEQAStageDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName StageId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 StageNumber = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName NextStageId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName RequiredPower = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName RequiredWeapon = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FALSAEQAStageBeat> Interludes;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> PersistentWorldFlags;

    // Difficulty is deliberately monotonic: later stages cannot be configured easier than earlier stages.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="1.0", ClampMax="10.0")) float DifficultyRating = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.1", ClampMax="5.0")) float EnemyHealthMultiplier = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.1", ClampMax="5.0")) float EnemyDamageMultiplier = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.1", ClampMax="5.0")) float EnemyAggressionMultiplier = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.1", ClampMax="5.0")) float HazardMultiplier = 1.0f;
};
