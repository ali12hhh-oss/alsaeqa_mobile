#include "Visual/ALSAEQAVisualAssetComponent.h"

#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

UALSAEQAVisualAssetComponent::UALSAEQAVisualAssetComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQAVisualAssetComponent::BeginPlay()
{
    Super::BeginPlay();
    if (bApplyOnBeginPlay)
    {
        ApplyVisualAsset();
    }
}

bool UALSAEQAVisualAssetComponent::HasConfiguredVisualAsset() const
{
    return !SkeletalMesh.IsNull();
}

bool UALSAEQAVisualAssetComponent::ApplyVisualAsset()
{
    ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
    if (!CharacterOwner || SkeletalMesh.IsNull())
    {
        return false;
    }

    USkeletalMesh* Mesh = SkeletalMesh.LoadSynchronous();
    if (!Mesh)
    {
        return false;
    }

    USkeletalMeshComponent* MeshComponent = CharacterOwner->GetMesh();
    if (!MeshComponent)
    {
        return false;
    }

    MeshComponent->SetSkeletalMesh(Mesh);

    if (!AnimationClass.IsNull())
    {
        if (UClass* LoadedAnimationClass = AnimationClass.LoadSynchronous())
        {
            MeshComponent->SetAnimInstanceClass(LoadedAnimationClass);
        }
    }

    return true;
}
