#pragma once

#include "CoreMinimal.h"
#include "ALSAEQAProgressionStageTypes.generated.h"

USTRUCT(BlueprintType)
struct ALSAEQA_API FALSAEQAProgressionStage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 StageNumber = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StageId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 NextStage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCompanionFamilySearchStage = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bMountStage = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bThunderBeastStage = false;

    bool IsValid() const
    {
        return StageNumber >= 1 && StageNumber <= 70 && !StageId.IsNone() && !Title.IsEmpty();
    }
};
