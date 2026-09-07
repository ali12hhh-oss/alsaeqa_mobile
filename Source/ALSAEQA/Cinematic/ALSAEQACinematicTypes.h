#pragma once

#include "CoreMinimal.h"
#include "ALSAEQACinematicTypes.generated.h"

UENUM(BlueprintType)
enum class EALSAEQACinematicEvent : uint8
{
    Opening UMETA(DisplayName="Opening Cinematic"),
    CombatFinisher UMETA(DisplayName="Combat Finisher"),
    PerfectDodge UMETA(DisplayName="Perfect Dodge"),
    ParryCounter UMETA(DisplayName="Parry Counter"),
    AbilityImpact UMETA(DisplayName="Ability Impact"),
    NearDeath UMETA(DisplayName="Near Death"),
    Fall UMETA(DisplayName="Fall"),
    Rescue UMETA(DisplayName="Rescue"),
    StageTransition UMETA(DisplayName="Stage Transition"),
    TraversalStunt UMETA(DisplayName="Traversal Stunt"),
    GiantSnakeEncounter UMETA(DisplayName="Giant Snake Encounter"),
    BossReveal UMETA(DisplayName="Boss Reveal")
};

USTRUCT(BlueprintType)
struct ALSAEQA_API FALSAEQACinematicRequest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EALSAEQACinematicEvent Event = EALSAEQACinematicEvent::CombatFinisher;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SlowMotionScale = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUseCameraShake = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUseDynamicCamera = true;
};