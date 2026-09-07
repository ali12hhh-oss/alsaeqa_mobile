#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQAVisualAssetComponent.generated.h"

class USkeletalMesh;
class UAnimInstance;

/**
 * Visual-only bridge between ALSAEQA gameplay actors and licensed art assets.
 * Gameplay code remains independent from a specific marketplace character.
 */
UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQAVisualAssetComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQAVisualAssetComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Visual") bool ApplyVisualAsset();
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Visual") bool HasConfiguredVisualAsset() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|Visual")
    TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|Visual")
    TSoftClassPtr<UAnimInstance> AnimationClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|Visual")
    bool bApplyOnBeginPlay = true;

protected:
    virtual void BeginPlay() override;
};
