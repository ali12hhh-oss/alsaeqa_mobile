#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ALSAEQAArtProfile.generated.h"

class UAnimMontage;
class UAnimSequence;
class UMaterialInterface;
class USkeletalMesh;

UENUM(BlueprintType)
enum class EALSAEQAArtRole : uint8
{
    Hero,
    Companion,
    Enemy,
    Boss,
    GiantSnake,
    Horse,
    Beast,
    ThunderBeast,
    FlyingBeast,
    Weapon
};

/** Real imported art profile. No procedural geometry is required when Mesh is assigned. */
UCLASS(BlueprintType)
class ALSAEQA_API UALSAEQAArtProfile : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Identity")
    FName ProfileId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Identity")
    EALSAEQAArtRole Role = EALSAEQAArtRole::Hero;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Identity")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Mesh")
    TSoftObjectPtr<USkeletalMesh> Mesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Animation")
    TSubclassOf<UAnimInstance> AnimClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Animation")
    TSoftObjectPtr<UAnimSequence> Idle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Animation")
    TSoftObjectPtr<UAnimSequence> Walk;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Animation")
    TSoftObjectPtr<UAnimSequence> Run;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Animation")
    TSoftObjectPtr<UAnimMontage> LightAttack;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Animation")
    TSoftObjectPtr<UAnimMontage> HeavyAttack;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Animation")
    TSoftObjectPtr<UAnimMontage> HitReact;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Animation")
    TSoftObjectPtr<UAnimMontage> Death;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Animation")
    TSoftObjectPtr<UAnimMontage> SpecialAbility;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Visual")
    TArray<TSoftObjectPtr<UMaterialInterface>> MaterialOverrides;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Visual")
    FVector RelativeLocation = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Visual")
    FRotator RelativeRotation = FRotator::ZeroRotator;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALSAEQA|Visual")
    FVector RelativeScale = FVector(1.0f);
};
