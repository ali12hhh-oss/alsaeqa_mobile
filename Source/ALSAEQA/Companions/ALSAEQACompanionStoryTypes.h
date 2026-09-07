#pragma once

#include "CoreMinimal.h"
#include "ALSAEQACompanionStoryTypes.generated.h"

UENUM(BlueprintType)
enum class EALSAEQACompanionStoryState : uint8
{
    Introduced,
    Trusted,
    AbilityAwakening,
    Separated,
    Captured,
    RescueKnown,
    Rescued,
    FamilyClue,
    FamilyRevelation
};

USTRUCT(BlueprintType)
struct FALSAEQACompanionStoryProgress
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FName CompanionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    EALSAEQACompanionStoryState State = EALSAEQACompanionStoryState::Introduced;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    int32 AbilityDiscoveryCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    int32 FamilyClueCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    int32 RescueCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    TArray<FName> CompletedSceneIds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    TArray<FName> FamilyClueIds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    TArray<FName> PersistentFlags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Family Search")
    int32 FamilySearchStage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Family Search")
    int32 FamilyEvidenceCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Family Search")
    TArray<FName> FamilyEvidenceIds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Family Search")
    bool bFamilySearchCompleted = false;
};
