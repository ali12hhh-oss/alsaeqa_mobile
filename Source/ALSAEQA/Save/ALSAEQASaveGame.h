#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Save/ALSAEQACheckpointTypes.h"
#include "Companions/ALSAEQACompanionStoryTypes.h"
#include "Companions/ALSAEQAMountTypes.h"
#include "ALSAEQASaveGame.generated.h"

UCLASS()
class ALSAEQA_API UALSAEQASaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Save")
    FALSAEQACheckpointData LastCheckpoint;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Save")
    TArray<FName> CompletedDiscoveryIds;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Save")
    TArray<uint8> UnlockedPowers;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Save")
    TArray<uint8> AcquiredWeapons;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Legacy")
    TArray<FName> DiscoveredMemoryIds;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Legacy")
    TArray<FName> PersistentWorldFlags;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Stage1")
    TArray<FName> RescuedStageOneWorkerIds;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Stage1")
    TArray<FName> DefeatedStageOneSlaverIds;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Companion")
    FALSAEQACompanionStoryProgress CompanionStory;

    UPROPERTY(BlueprintReadWrite, SaveGame, Category="ALSAEQA|Mounts")
    TArray<FALSAEQAMountProfile> TamedMountProfiles;
};
