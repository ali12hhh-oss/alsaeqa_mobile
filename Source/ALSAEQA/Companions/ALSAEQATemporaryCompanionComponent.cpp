#include "Companions/ALSAEQATemporaryCompanionComponent.h"

UALSAEQATemporaryCompanionComponent::UALSAEQATemporaryCompanionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UALSAEQATemporaryCompanionComponent::RecruitTemporaryCompanion(const FALSAEQATemporaryCompanionProfile& Profile)
{
    if (Profile.CompanionId.IsNone() || HasCompanion(Profile.CompanionId))
    {
        return false;
    }
    ActiveCompanions.Add(Profile);
    OnCompanionRecruited.Broadcast(Profile.CompanionId);
    return true;
}

bool UALSAEQATemporaryCompanionComponent::ReleaseTemporaryCompanion(FName CompanionId)
{
    const int32 Index = ActiveCompanions.IndexOfByPredicate([CompanionId](const FALSAEQATemporaryCompanionProfile& Entry)
    {
        return Entry.CompanionId == CompanionId;
    });
    if (Index == INDEX_NONE)
    {
        return false;
    }
    ActiveCompanions.RemoveAt(Index);
    OnCompanionReleased.Broadcast(CompanionId);
    return true;
}

void UALSAEQATemporaryCompanionComponent::ClearTemporaryCompanions()
{
    TArray<FName> ReleasedIds;
    for (const FALSAEQATemporaryCompanionProfile& Entry : ActiveCompanions)
    {
        ReleasedIds.Add(Entry.CompanionId);
    }
    ActiveCompanions.Reset();
    for (const FName Id : ReleasedIds)
    {
        OnCompanionReleased.Broadcast(Id);
    }
}

bool UALSAEQATemporaryCompanionComponent::HasCompanion(FName CompanionId) const
{
    return ActiveCompanions.ContainsByPredicate([CompanionId](const FALSAEQATemporaryCompanionProfile& Entry)
    {
        return Entry.CompanionId == CompanionId;
    });
}
