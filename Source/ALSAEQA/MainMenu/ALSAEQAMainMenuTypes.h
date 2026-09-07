#pragma once

#include "CoreMinimal.h"
#include "ALSAEQAMainMenuTypes.generated.h"

USTRUCT(BlueprintType)
struct FALSAEQAMainMenuState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Main Menu")
    bool bHasSave = false;

    UPROPERTY(BlueprintReadOnly, Category="Main Menu")
    int32 CurrentStage = 1;

    UPROPERTY(BlueprintReadOnly, Category="Main Menu")
    FName CurrentRegion = NAME_None;

    UPROPERTY(BlueprintReadOnly, Category="Main Menu")
    FName CurrentCheckpoint = NAME_None;

    UPROPERTY(BlueprintReadOnly, Category="Main Menu")
    bool bCompanionRescued = false;

    UPROPERTY(BlueprintReadOnly, Category="Main Menu")
    bool bThunderBeastOwned = false;

    UPROPERTY(BlueprintReadOnly, Category="Main Menu")
    int32 CompanionFamilySearchStage = 0;
};

UENUM(BlueprintType)
enum class EALSAEQAMainMenuAction : uint8
{
    ContinueJourney,
    NewJourney,
    World,
    Achievements,
    Settings
};
