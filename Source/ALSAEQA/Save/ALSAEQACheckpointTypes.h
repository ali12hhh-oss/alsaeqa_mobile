#pragma once

#include "CoreMinimal.h"
#include "ALSAEQACheckpointTypes.generated.h"

USTRUCT(BlueprintType)
struct ALSAEQA_API FALSAEQACheckpointData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FName CheckpointId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FName RegionId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    int32 Stage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FVector PlayerLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FRotator PlayerRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    int32 RespawnCount = 0;
};
