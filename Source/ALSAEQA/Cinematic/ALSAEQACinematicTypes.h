#pragma once

#include "CoreMinimal.h"
#include "ALSAEQACinematicTypes.generated.h"

// Action-moment / story-beat events. These map directly to
// Docs/MASTER_COMPLETION_SPEC.md §5 (Cinematic action system).
UENUM(BlueprintType)
enum class EALSAEQACinematicEvent : uint8
{
    Opening UMETA(DisplayName="Opening Cinematic"),
    OpeningCompleted UMETA(DisplayName="Opening Cinematic Completed"),
    CombatFinisher UMETA(DisplayName="Combat Finisher"),
    PerfectDodge UMETA(DisplayName="Perfect Dodge"),
    ParryCounter UMETA(DisplayName="Parry Counter"),
    AbilityImpact UMETA(DisplayName="Ability Impact"),
    NearDeath UMETA(DisplayName="Near Death"),
    Fall UMETA(DisplayName="Fall"),
    LedgeCatch UMETA(DisplayName="Ledge Catch"),
    SlidingSave UMETA(DisplayName="Sliding Save"),
    ClimbFailureRescue UMETA(DisplayName="Climb Failure Rescue"),
    CompanionRescue UMETA(DisplayName="Companion Rescue"),
    Rescue UMETA(DisplayName="Rescue"),
    StageTransition UMETA(DisplayName="Stage Transition"),
    TraversalStunt UMETA(DisplayName="Traversal Stunt"),
    GiantSnakeEncounter UMETA(DisplayName="Giant Snake Encounter"),
    BossReveal UMETA(DisplayName="Boss Reveal"),
    BossImpact UMETA(DisplayName="Boss Impact")
};

// The six authored beats of the opening cinematic, exactly as timed in
// Docs/OPENING_CINEMATIC.md (the canonical 30-45s mobile-safe cut).
UENUM(BlueprintType)
enum class EALSAEQAOpeningSequenceStage : uint8
{
    StormBeforeMemory        UMETA(DisplayName="01 - The Storm Before Memory (0:00-0:06)"),
    FallOfTheThunderLegacy   UMETA(DisplayName="02 - The Fall of the Thunder Legacy (0:06-0:13)"),
    SecondCaptorsAndTheHeir  UMETA(DisplayName="03 - The Second Captors and the Heir (0:13-0:20)"),
    TheCollapse              UMETA(DisplayName="04 - The Collapse (0:20-0:27)"),
    TheFirstAwakening        UMETA(DisplayName="05 - The First Awakening (0:27-0:37)"),
    HandoffToStageOne        UMETA(DisplayName="06 - Hand-off to Stage 1 (0:37-0:45)")
};

USTRUCT(BlueprintType)
struct ALSAEQA_API FALSAEQAOpeningSequenceBeat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|Cinematics")
    EALSAEQAOpeningSequenceStage Stage = EALSAEQAOpeningSequenceStage::StormBeforeMemory;

    // Seconds, matching the canonical mobile-safe 30-45s cut in OPENING_CINEMATIC.md.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|Cinematics")
    float Duration = 6.0f;

    // The final hand-off beat (title reveal + camera hand-off to Stage 1) must not be
    // skippable once entered, per OPENING_CINEMATIC.md's "Gameplay hand-off rules".
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|Cinematics")
    bool bCriticalBoundary = false;
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
