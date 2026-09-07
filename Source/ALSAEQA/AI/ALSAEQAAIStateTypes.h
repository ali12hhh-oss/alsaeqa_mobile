#pragma once

#include "CoreMinimal.h"
#include "ALSAEQAAIStateTypes.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAAIState : uint8
{
    Idle UMETA(DisplayName="Idle"),
    Patrol UMETA(DisplayName="Patrol"),
    Investigate UMETA(DisplayName="Investigate"),
    Chase UMETA(DisplayName="Chase"),
    Attack UMETA(DisplayName="Attack"),
    Stunned UMETA(DisplayName="Stunned"),
    Flee UMETA(DisplayName="Flee"),
    Dead UMETA(DisplayName="Dead")
};
