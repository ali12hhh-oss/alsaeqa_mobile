#include "Traversal/ALSAEQAFallRescueComponent.h"

#include "GameFramework/Actor.h"

UALSAEQAFallRescueComponent::UALSAEQAFallRescueComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQAFallRescueComponent::BeginFall()
{
    if (bFalling)
    {
        return;
    }

    bFalling = true;
    OnFallStarted.Broadcast();
}

bool UALSAEQAFallRescueComponent::TryRescue(const FVector& RescueLocation)
{
    if (!bFalling || !GetOwner())
    {
        return false;
    }

    GetOwner()->SetActorLocation(RescueLocation);
    bFalling = false;
    OnRescued.Broadcast();
    return true;
}

void UALSAEQAFallRescueComponent::CancelFall()
{
    if (!bFalling)
    {
        return;
    }
    bFalling = false;
}
