#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Companions/ALSAEQACompanionStoryTypes.h"
#include "ALSAEQACompanionStoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQACompanionStoryStateChanged, EALSAEQACompanionStoryState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQACompanionFamilySearchUnlocked, int32, Stage);

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQACompanionStoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQACompanionStoryComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion Story")
    bool SetState(EALSAEQACompanionStoryState NewState);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion Story")
    bool DiscoverFamilyClue(FName ClueId);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion Story")
    bool DiscoverMemory(FName MemoryId);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion Story")
    bool RegisterRescue();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companion Story")
    void SetCompanionId(FName NewCompanionId);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion Story")
    bool IsFamilySearchUnlocked() const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companion Story")
    FALSAEQACompanionStoryProgress GetProgress() const { return Progress; }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Companion Story")
    FALSAEQACompanionStoryStateChanged OnStateChanged;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Companion Story")
    FALSAEQACompanionFamilySearchUnlocked OnFamilySearchUnlocked;

protected:
    virtual void BeginPlay() override;

private:
    void SynchronizeFromSaveManager(bool bBroadcastUnlock);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="ALSAEQA|Companion Story", meta=(AllowPrivateAccess="true"))
    FALSAEQACompanionStoryProgress Progress;
};
