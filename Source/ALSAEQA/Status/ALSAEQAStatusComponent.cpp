#include "Status/ALSAEQAStatusComponent.h"

#include "Systems/ALSAEQAHealthComponent.h"

UALSAEQAStatusComponent::UALSAEQAStatusComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UALSAEQAStatusComponent::BeginPlay()
{
    Super::BeginPlay();
    HealthComponent = GetOwner() ? GetOwner()->FindComponentByClass<UALSAEQAHealthComponent>() : nullptr;
    PoisonTickAccumulator = 0.0f;
}

void UALSAEQAStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const float SafeDeltaTime = FMath::Max(DeltaTime, 0.0f);
    PoisonTickAccumulator += SafeDeltaTime;

    float PoisonDamagePerSecond = 0.0f;
    for (int32 Index = ActiveStatuses.Num() - 1; Index >= 0; --Index)
    {
        FALSAEQAStatusEffect& Effect = ActiveStatuses[Index];
        Effect.RemainingTime -= SafeDeltaTime;

        if (Effect.StatusId == FName(TEXT("Poison")) && Effect.RemainingTime > 0.0f)
        {
            PoisonDamagePerSecond = FMath::Max(PoisonDamagePerSecond, Effect.DamagePerSecond);
        }

        if (Effect.RemainingTime <= 0.0f)
        {
            const FName Id = Effect.StatusId;
            ActiveStatuses.RemoveAtSwap(Index);
            OnStatusChanged.Broadcast(Id, false);
        }
    }

    if (PoisonDamagePerSecond > 0.0f && HealthComponent && !HealthComponent->IsDead())
    {
        while (PoisonTickAccumulator >= PoisonTickInterval)
        {
            PoisonTickAccumulator -= PoisonTickInterval;
            HealthComponent->ApplyDamage(PoisonDamagePerSecond * PoisonTickInterval);

            if (HealthComponent->IsDead())
            {
                PoisonTickAccumulator = 0.0f;
                break;
            }
        }
    }
    else if (PoisonDamagePerSecond <= 0.0f)
    {
        PoisonTickAccumulator = 0.0f;
    }
}

void UALSAEQAStatusComponent::ApplyPoison(float DamagePerSecond, float Duration)
{
    if (Duration <= 0.0f || DamagePerSecond <= 0.0f)
    {
        return;
    }

    static const FName PoisonId(TEXT("Poison"));
    for (FALSAEQAStatusEffect& Effect : ActiveStatuses)
    {
        if (Effect.StatusId == PoisonId)
        {
            Effect.DamagePerSecond = FMath::Max(Effect.DamagePerSecond, DamagePerSecond);
            Effect.RemainingTime = FMath::Max(Effect.RemainingTime, Duration);
            OnStatusChanged.Broadcast(PoisonId, true);
            return;
        }
    }

    FALSAEQAStatusEffect NewEffect;
    NewEffect.StatusId = PoisonId;
    NewEffect.DamagePerSecond = DamagePerSecond;
    NewEffect.RemainingTime = Duration;
    ActiveStatuses.Add(NewEffect);
    PoisonTickAccumulator = 0.0f;
    OnStatusChanged.Broadcast(PoisonId, true);
}

void UALSAEQAStatusComponent::ClearStatus(FName StatusId)
{
    for (int32 Index = ActiveStatuses.Num() - 1; Index >= 0; --Index)
    {
        if (ActiveStatuses[Index].StatusId == StatusId)
        {
            ActiveStatuses.RemoveAtSwap(Index);
            OnStatusChanged.Broadcast(StatusId, false);
        }
    }

    if (StatusId == FName(TEXT("Poison")))
    {
        PoisonTickAccumulator = 0.0f;
    }
}

bool UALSAEQAStatusComponent::HasStatus(FName StatusId) const
{
    return ActiveStatuses.ContainsByPredicate([StatusId](const FALSAEQAStatusEffect& Effect)
    {
        return Effect.StatusId == StatusId && Effect.RemainingTime > 0.0f;
    });
}

float UALSAEQAStatusComponent::GetRemainingTime(FName StatusId) const
{
    for (const FALSAEQAStatusEffect& Effect : ActiveStatuses)
    {
        if (Effect.StatusId == StatusId)
        {
            return FMath::Max(0.0f, Effect.RemainingTime);
        }
    }
    return 0.0f;
}
