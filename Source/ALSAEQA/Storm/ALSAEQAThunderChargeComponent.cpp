#include "Storm/ALSAEQAThunderChargeComponent.h"

UALSAEQAThunderChargeComponent::UALSAEQAThunderChargeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UALSAEQAThunderChargeComponent::BeginPlay()
{
    Super::BeginPlay();
    ChargePercent = 0.0f;
    bCharging = false;
}

void UALSAEQAThunderChargeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bCharging)
    {
        return;
    }

    const float SafeChargeTime = FMath::Max(FullChargeTime, KINDA_SMALL_NUMBER);
    const float OldCharge = ChargePercent;
    ChargePercent = FMath::Clamp(ChargePercent + (DeltaTime / SafeChargeTime), 0.0f, 1.0f);

    if (!FMath::IsNearlyEqual(OldCharge, ChargePercent))
    {
        OnChargeChanged.ExecuteIfBound(ChargePercent, IsFullyCharged());
    }
}

bool UALSAEQAThunderChargeComponent::BeginCharge()
{
    if (bCharging)
    {
        return false;
    }

    bCharging = true;
    ChargePercent = 0.0f;
    OnChargeChanged.ExecuteIfBound(ChargePercent, false);
    return true;
}

float UALSAEQAThunderChargeComponent::ReleaseCharge()
{
    if (!bCharging)
    {
        return 0.0f;
    }

    const float ReleasedPercent = ChargePercent;
    bCharging = false;
    ChargePercent = 0.0f;
    OnThunderReleased.ExecuteIfBound(ReleasedPercent);
    OnChargeChanged.ExecuteIfBound(0.0f, false);
    return ReleasedPercent;
}

void UALSAEQAThunderChargeComponent::CancelCharge()
{
    if (!bCharging && FMath::IsNearlyZero(ChargePercent))
    {
        return;
    }

    bCharging = false;
    ChargePercent = 0.0f;
    OnChargeChanged.ExecuteIfBound(0.0f, false);
}

float UALSAEQAThunderChargeComponent::GetDamageMultiplier() const
{
    if (ChargePercent < MinimumReleasePercent)
    {
        return 0.0f;
    }

    const float Normalized = FMath::GetRangeValue(FVector2D(MinimumReleasePercent, 1.0f), ChargePercent);
    return FMath::Lerp(1.0f, MaxDamageMultiplier, FMath::Clamp(Normalized, 0.0f, 1.0f));
}
