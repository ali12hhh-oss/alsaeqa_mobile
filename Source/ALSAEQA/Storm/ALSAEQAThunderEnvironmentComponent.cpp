#include "Storm/ALSAEQAThunderEnvironmentComponent.h"

UALSAEQAThunderEnvironmentComponent::UALSAEQAThunderEnvironmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UALSAEQAThunderEnvironmentComponent::ReceiveThunder(const FALSAEQADamageInfo& ThunderInfo)
{
    return ReceiveThunderInteraction_Implementation(ThunderInfo);
}

bool UALSAEQAThunderEnvironmentComponent::ReceiveThunderInteraction_Implementation(const FALSAEQADamageInfo& ThunderInfo)
{
    if (ThunderInfo.Type != EALSAEQADamageType::Thunder || ThunderInfo.Amount <= 0.0f)
    {
        return false;
    }

    if (bActivated && !bAllowRepeatedCharge)
    {
        return false;
    }

    const float SafeRequiredCharge = FMath::Max(RequiredCharge, 0.1f);
    const float AddedCharge = ThunderInfo.Amount * FMath::Max(ChargePerDamage, 0.01f);
    ChargePercent = FMath::Clamp(ChargePercent + AddedCharge / (SafeRequiredCharge * 100.0f), 0.0f, 1.0f);

    OnThunderEnvironmentChanged.Broadcast(Reaction, ChargePercent);

    if (ChargePercent >= 1.0f)
    {
        bActivated = true;
        OnThunderEnvironmentChanged.Broadcast(Reaction, 1.0f);
    }

    return true;
}

void UALSAEQAThunderEnvironmentComponent::ResetReaction()
{
    ChargePercent = 0.0f;
    bActivated = false;
    OnThunderEnvironmentChanged.Broadcast(Reaction, ChargePercent);
}
