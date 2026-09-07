#include "Visual/ALSAEQARuntimeVisualComponent.h"
#include "Visual/ALSAEQAArtProfile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"

UALSAEQARuntimeVisualComponent::UALSAEQARuntimeVisualComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQARuntimeVisualComponent::BeginPlay()
{
    Super::BeginPlay();
    if (bBuildOnBeginPlay)
    {
        RebuildVisual();
    }
}

void UALSAEQARuntimeVisualComponent::ClearVisual()
{
    if (IsValid(ImportedMeshComponent))
    {
        ImportedMeshComponent->DestroyComponent();
        ImportedMeshComponent = nullptr;
    }
    bUsingImportedArt = false;
}

bool UALSAEQARuntimeVisualComponent::TryBuildImportedArt()
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->GetRootComponent())
    {
        return false;
    }

    USkeletalMesh* Mesh = nullptr;
    TSubclassOf<UAnimInstance> AnimClass = ImportedAnimClass;
    FVector Location = ImportedRelativeLocation;
    FRotator Rotation = ImportedRelativeRotation;
    FVector Scale = ImportedRelativeScale;
    TArray<TSoftObjectPtr<UMaterialInterface>> Materials;

    if (!ArtProfile.IsNull())
    {
        if (UALSAEQAArtProfile* Profile = ArtProfile.LoadSynchronous())
        {
            Mesh = Profile->Mesh.LoadSynchronous();
            if (Profile->AnimClass)
            {
                AnimClass = Profile->AnimClass;
            }
            Location = Profile->RelativeLocation;
            Rotation = Profile->RelativeRotation;
            Scale = Profile->RelativeScale;
            Materials = Profile->MaterialOverrides;
        }
    }

    if (!Mesh && !ImportedSkeletalMesh.IsNull())
    {
        Mesh = ImportedSkeletalMesh.LoadSynchronous();
    }

    if (!Mesh)
    {
        return false;
    }

    USkeletalMeshComponent* Component = NewObject<USkeletalMeshComponent>(Owner);
    if (!Component)
    {
        return false;
    }

    Component->SetSkeletalMesh(Mesh);
    Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Component->SetMobility(EComponentMobility::Movable);
    Component->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    Component->SetRelativeLocation(Location);
    Component->SetRelativeRotation(Rotation);
    Component->SetRelativeScale3D(Scale);

    if (AnimClass)
    {
        Component->SetAnimInstanceClass(AnimClass);
    }

    Component->RegisterComponent();

    for (int32 Index = 0; Index < Materials.Num() && Index < Component->GetNumMaterials(); ++Index)
    {
        if (UMaterialInterface* Material = Materials[Index].LoadSynchronous())
        {
            Component->SetMaterial(Index, Material);
        }
    }

    ImportedMeshComponent = Component;
    bUsingImportedArt = true;
    return true;
}

void UALSAEQARuntimeVisualComponent::RebuildVisual()
{
    ClearVisual();
    // Deliberately do nothing when authored art is not assigned.
    // ALSAEQA must never silently substitute primitives, procedural meshes, or placeholders.
    TryBuildImportedArt();
}

void UALSAEQARuntimeVisualComponent::SetVisualRole(EALSAEQAVisualRole NewRole)
{
    VisualRole = NewRole;
    if (IsRegistered())
    {
        RebuildVisual();
    }
}

bool UALSAEQARuntimeVisualComponent::PlayProfileMontage(TSoftObjectPtr<UAnimMontage> MontageAsset)
{
    if (!bUsingImportedArt || !IsValid(ImportedMeshComponent) || MontageAsset.IsNull())
    {
        return false;
    }

    UAnimMontage* Montage = MontageAsset.LoadSynchronous();
    if (!Montage)
    {
        return false;
    }

    UAnimInstance* AnimInstance = ImportedMeshComponent->GetAnimInstance();
    if (!AnimInstance)
    {
        return false;
    }

    return AnimInstance->Montage_Play(Montage) > 0.f;
}

bool UALSAEQARuntimeVisualComponent::PlayLightAttackAnimation()
{
    UALSAEQAArtProfile* Profile = ArtProfile.IsNull() ? nullptr : ArtProfile.LoadSynchronous();
    return Profile && PlayProfileMontage(Profile->LightAttack);
}

bool UALSAEQARuntimeVisualComponent::PlayHeavyAttackAnimation()
{
    UALSAEQAArtProfile* Profile = ArtProfile.IsNull() ? nullptr : ArtProfile.LoadSynchronous();
    return Profile && PlayProfileMontage(Profile->HeavyAttack);
}

bool UALSAEQARuntimeVisualComponent::PlayHitReactionAnimation()
{
    UALSAEQAArtProfile* Profile = ArtProfile.IsNull() ? nullptr : ArtProfile.LoadSynchronous();
    return Profile && PlayProfileMontage(Profile->HitReact);
}

bool UALSAEQARuntimeVisualComponent::PlayDeathAnimation()
{
    UALSAEQAArtProfile* Profile = ArtProfile.IsNull() ? nullptr : ArtProfile.LoadSynchronous();
    return Profile && PlayProfileMontage(Profile->Death);
}

bool UALSAEQARuntimeVisualComponent::PlaySpecialAbilityAnimation()
{
    UALSAEQAArtProfile* Profile = ArtProfile.IsNull() ? nullptr : ArtProfile.LoadSynchronous();
    return Profile && PlayProfileMontage(Profile->SpecialAbility);
}
