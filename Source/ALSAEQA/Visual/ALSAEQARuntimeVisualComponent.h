#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQARuntimeVisualComponent.generated.h"

class UALSAEQAArtProfile;
class UAnimInstance;
class UAnimMontage;
class USkeletalMesh;
class USkeletalMeshComponent;

UENUM(BlueprintType)
enum class EALSAEQAVisualRole : uint8 { Hero, Companion, Enemy, Boss, Snake, Mount, Weapon };

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQARuntimeVisualComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALSAEQARuntimeVisualComponent();

    // Real authored/imported art only. No procedural or primitive fallback is permitted.
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Visual") void RebuildVisual();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Visual") void SetVisualRole(EALSAEQAVisualRole NewRole);
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Visual") EALSAEQAVisualRole GetVisualRole() const { return VisualRole; }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Visual") bool IsUsingImportedArt() const { return bUsingImportedArt; }

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Animation") bool PlayLightAttackAnimation();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Animation") bool PlayHeavyAttackAnimation();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Animation") bool PlayHitReactionAnimation();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Animation") bool PlayDeathAnimation();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Animation") bool PlaySpecialAbilityAnimation();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Visual") EALSAEQAVisualRole VisualRole = EALSAEQAVisualRole::Hero;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Visual") bool bBuildOnBeginPlay = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Real Art", meta=(ToolTip="Required source of truth for final authored 3D art. Assign after the asset has been previewed and approved.")) TSoftObjectPtr<UALSAEQAArtProfile> ArtProfile;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Real Art", meta=(ToolTip="Direct imported skeletal mesh override. Used only when ArtProfile is empty.")) TSoftObjectPtr<USkeletalMesh> ImportedSkeletalMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Real Art", meta=(ToolTip="Optional animation blueprint class.")) TSubclassOf<UAnimInstance> ImportedAnimClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Real Art") FVector ImportedRelativeLocation = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Real Art") FRotator ImportedRelativeRotation = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALSAEQA|Real Art") FVector ImportedRelativeScale = FVector(1.f);
    UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="ALSAEQA|Real Art") bool bUsingImportedArt = false;

private:
    void ClearVisual();
    bool TryBuildImportedArt();
    bool PlayProfileMontage(TSoftObjectPtr<UAnimMontage> MontageAsset);
    UPROPERTY(Transient) TObjectPtr<USkeletalMeshComponent> ImportedMeshComponent;
};
