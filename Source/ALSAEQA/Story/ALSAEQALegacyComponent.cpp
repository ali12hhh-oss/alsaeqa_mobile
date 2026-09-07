#include "Story/ALSAEQALegacyComponent.h"

UALSAEQALegacyComponent::UALSAEQALegacyComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UALSAEQALegacyComponent::DiscoverMemory(FName MemoryId)
{
    if (MemoryId.IsNone() || DiscoveredMemoryIds.Contains(MemoryId))
    {
        return false;
    }

    DiscoveredMemoryIds.Add(MemoryId);
    OnMemoryDiscovered.Broadcast(MemoryId);
    return true;
}

bool UALSAEQALegacyComponent::AddFamilyClue(FName ClueId)
{
    if (ClueId.IsNone() || FamilyClueIds.Contains(ClueId))
    {
        return false;
    }

    FamilyClueIds.Add(ClueId);
    return true;
}

void UALSAEQALegacyComponent::SetLegacyFlag(FName FlagId, bool bEnabled)
{
    if (FlagId.IsNone())
    {
        return;
    }

    if (bEnabled)
    {
        if (!LegacyFlags.Contains(FlagId))
        {
            LegacyFlags.Add(FlagId);
            OnLegacyFlagChanged.Broadcast(FlagId);
        }
    }
    else if (LegacyFlags.Remove(FlagId) > 0)
    {
        OnLegacyFlagChanged.Broadcast(FlagId);
    }
}

bool UALSAEQALegacyComponent::HasMemory(FName MemoryId) const
{
    return !MemoryId.IsNone() && DiscoveredMemoryIds.Contains(MemoryId);
}

bool UALSAEQALegacyComponent::HasFamilyClue(FName ClueId) const
{
    return !ClueId.IsNone() && FamilyClueIds.Contains(ClueId);
}

bool UALSAEQALegacyComponent::HasLegacyFlag(FName FlagId) const
{
    return !FlagId.IsNone() && LegacyFlags.Contains(FlagId);
}

bool UALSAEQALegacyComponent::CanBeginCompanionFamilySearch() const
{
    // The companion's family search must never become an active quest before stage 25.
    return CurrentStage >= 25;
}

void UALSAEQALegacyComponent::SetCurrentStage(int32 Stage)
{
    CurrentStage = FMath::Max(Stage, 1);
}
