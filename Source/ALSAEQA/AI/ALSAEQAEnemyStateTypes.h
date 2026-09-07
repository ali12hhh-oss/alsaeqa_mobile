#pragma once

#include "CoreMinimal.h"
#include "ALSAEQAEnemyStateTypes.generated.h"

UENUM(BlueprintType)
enum class EALSAEQAEnemyState : uint8
{
    Idle,
    Patrol,
    Alert,
    Chase,
    Attack,
    Stagger,
    Flee,
    Defeated
};

UENUM(BlueprintType)
enum class EALSAEQABossPhase : uint8
{
    PhaseOne,
    PhaseTwo,
    PhaseThree,
    Defeated
};
