#include "Companions/ALSAEQACompanionFamilyGate.h"

UALSAEQACompanionFamilyGate::UALSAEQACompanionFamilyGate()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UALSAEQACompanionFamilyGate::UpdateForStage(int32 CurrentStage)
{
    const bool bNewState = CurrentStage >= RequiredStage;
    if (bNewState == bFamilyArcUnlocked)
    {
        return bFamilyArcUnlocked;
    }

    bFamilyArcUnlocked = bNewState;
    OnFamilyArcGateChanged.Broadcast(bFamilyArcUnlocked);
    return bFamilyArcUnlocked;
}
