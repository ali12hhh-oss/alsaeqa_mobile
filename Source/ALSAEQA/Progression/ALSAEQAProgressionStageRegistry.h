#pragma once

#include "CoreMinimal.h"
#include "ALSAEQAProgressionStageTypes.h"

namespace ALSAEQAProgressionStageRegistry
{
    ALSAEQA_API const TArray<FALSAEQAProgressionStage>& GetStages();

    ALSAEQA_API const FALSAEQAProgressionStage* FindStage(int32 StageNumber);

    ALSAEQA_API bool IsValidStage(int32 StageNumber);
}
