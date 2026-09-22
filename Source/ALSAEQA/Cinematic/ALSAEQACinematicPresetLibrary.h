#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Cinematic/ALSAEQACinematicTypes.h"
#include "ALSAEQACinematicPresetLibrary.generated.h"

// Professionally-tuned slow-motion/duration defaults for each cinematic action event,
// per Docs/MASTER_COMPLETION_SPEC.md §5 ("Cinematic action system"). Gameplay code
// should request these presets instead of hand-tuning SlowMotionScale/Duration at
// every call site, so every action beat across the whole game reads with a
// consistent, deliberately-authored feel rather than an ad hoc one.
UCLASS()
class ALSAEQA_API UALSAEQACinematicPresetLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Cinematics")
    static FALSAEQACinematicRequest MakeRequestForEvent(EALSAEQACinematicEvent Event);
};
