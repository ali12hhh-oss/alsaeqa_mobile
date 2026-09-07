#include "Progression/ALSAEQAProgressionDiscoveryActor.h"
#include "Progression/ALSAEQAProgressionComponent.h"

AALSAEQAProgressionDiscoveryActor::AALSAEQAProgressionDiscoveryActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool AALSAEQAProgressionDiscoveryActor::GrantTo(AActor* PlayerActor)
{
    if (!PlayerActor || !Discovery.IsValid())
    {
        return false;
    }

    UALSAEQAProgressionComponent* Progression = PlayerActor->FindComponentByClass<UALSAEQAProgressionComponent>();
    if (!Progression || Progression->GetCurrentStage() < Discovery.RequiredStage)
    {
        return false;
    }

    if (!Discovery.PrerequisiteDiscoveryId.IsNone() && !Progression->HasCompletedDiscovery(Discovery.PrerequisiteDiscoveryId))
    {
        return false;
    }

    bool bGranted = false;
    if (Discovery.RewardType == EALSAEQAProgressionRewardType::Power)
    {
        bGranted = Progression->UnlockPower(Discovery.Power, Discovery.DiscoveryId);
    }
    else
    {
        bGranted = Progression->AcquireWeapon(Discovery.Weapon, Discovery.DiscoveryId);
    }

    if (bGranted)
    {
        OnDiscoveryGranted.Broadcast(Discovery.DiscoveryId);
        if (bConsumeAfterGrant)
        {
            SetActorEnableCollision(false);
            SetActorHiddenInGame(true);
            SetActorTickEnabled(false);
        }
    }

    return bGranted;
}

bool AALSAEQAProgressionDiscoveryActor::IsCompletedFor(AActor* PlayerActor) const
{
    if (!PlayerActor || Discovery.DiscoveryId.IsNone())
    {
        return false;
    }

    const UALSAEQAProgressionComponent* Progression = PlayerActor->FindComponentByClass<UALSAEQAProgressionComponent>();
    return Progression && Progression->HasCompletedDiscovery(Discovery.DiscoveryId);
}
