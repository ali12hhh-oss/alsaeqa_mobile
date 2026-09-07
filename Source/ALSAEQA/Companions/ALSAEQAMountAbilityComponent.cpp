#include "Companions/ALSAEQAMountAbilityComponent.h"

#include "Companions/ALSAEQAMountActor.h"
#include "Companions/ALSAEQAMountComponent.h"
#include "Save/ALSAEQASaveManager.h"
#include "Combat/ALSAEQADamageTypes.h"
#include "Combat/ALSAEQADamageReceiver.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "GameFramework/CharacterMovementComponent.h"

UALSAEQAMountAbilityComponent::UALSAEQAMountAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UALSAEQAMountAbilityComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UALSAEQAMountAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    for (auto& Pair : Cooldowns)
    {
        Pair.Value = FMath::Max(0.0f, Pair.Value - DeltaTime);
    }

    if (LightningShieldRemaining > 0.0f)
    {
        LightningShieldRemaining = FMath::Max(0.0f, LightningShieldRemaining - DeltaTime);
        bLightningShieldActive = LightningShieldRemaining > 0.0f;
    }

    if (StormModeRemaining > 0.0f)
    {
        StormModeRemaining = FMath::Max(0.0f, StormModeRemaining - DeltaTime);
        bStormModeActive = StormModeRemaining > 0.0f;
    }
}

bool UALSAEQAMountAbilityComponent::IsThunderBeast() const
{
    const AALSAEQAMountActor* Mount = Cast<AALSAEQAMountActor>(GetOwner());
    const UALSAEQAMountComponent* Component = Mount ? Mount->GetMountComponent() : nullptr;
    return Component && Component->GetMountProfile().Type == EALSAEQAMountType::ThunderBeast;
}

int32 UALSAEQAMountAbilityComponent::GetCurrentStage() const
{
    const UWorld* World = GetWorld();
    const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    const UALSAEQASaveManager* SaveManager = GameInstance ? GameInstance->GetSubsystem<UALSAEQASaveManager>() : nullptr;
    return SaveManager ? SaveManager->GetStage() : 0;
}

bool UALSAEQAMountAbilityComponent::IsUnlocked(EALSAEQAMountAbility Ability) const
{
    if (!IsThunderBeast())
    {
        return false;
    }

    static const TMap<EALSAEQAMountAbility, int32> UnlockStages = {
        {EALSAEQAMountAbility::LightningDash, 60},
        {EALSAEQAMountAbility::ThunderRoar, 61},
        {EALSAEQAMountAbility::LightningKick, 62},
        {EALSAEQAMountAbility::StormCharge, 63},
        {EALSAEQAMountAbility::LightningCrossing, 64},
        {EALSAEQAMountAbility::StormLeap, 65},
        {EALSAEQAMountAbility::LightningShield, 66},
        {EALSAEQAMountAbility::StormSummon, 67},
        {EALSAEQAMountAbility::ThunderSense, 68},
        {EALSAEQAMountAbility::StormMode, 70}
    };

    const int32* RequiredStage = UnlockStages.Find(Ability);
    return RequiredStage && GetCurrentStage() >= *RequiredStage;
}

float UALSAEQAMountAbilityComponent::GetCooldownRemaining(EALSAEQAMountAbility Ability) const
{
    const float* Value = Cooldowns.Find(Ability);
    return Value ? *Value : 0.0f;
}

bool UALSAEQAMountAbilityComponent::CanActivate(EALSAEQAMountAbility Ability) const
{
    AALSAEQAMountActor* Mount = Cast<AALSAEQAMountActor>(GetOwner());
    UALSAEQAMountComponent* Component = Mount ? Mount->GetMountComponent() : nullptr;
    if (!Mount || !Component || !Mount->HasRider() || !Component->IsMounted() || !IsUnlocked(Ability))
    {
        return false;
    }

    return GetCooldownRemaining(Ability) <= 0.0f;
}

void UALSAEQAMountAbilityComponent::SetCooldown(EALSAEQAMountAbility Ability, float Seconds)
{
    Cooldowns.FindOrAdd(Ability) = FMath::Max(0.0f, Seconds);
}

bool UALSAEQAMountAbilityComponent::ExecuteThunderRoar(float Power)
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World)
    {
        return false;
    }

    TArray<FOverlapResult> Hits;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(ALSAEQQAThunderRoar), false, Owner);
    const FCollisionShape Shape = FCollisionShape::MakeSphere(ThunderRoarRadius);
    FCollisionObjectQueryParams ObjectTypes;
    ObjectTypes.AddObjectTypesToQuery(ECC_Pawn);
    if (!World->OverlapMultiByObjectType(Hits, Owner->GetActorLocation(), FQuat::Identity, ObjectTypes, Shape, Params))
    {
        return true;
    }

    TSet<AActor*> Damaged;
    for (const FOverlapResult& Hit : Hits)
    {
        AActor* Target = Hit.GetActor();
        if (!IsValid(Target) || Target == Owner || Damaged.Contains(Target))
        {
            continue;
        }

        if (Target->GetClass()->ImplementsInterface(UALSAEQADamageReceiver::StaticClass()))
        {
            FALSAEQADamageInfo Info;
            Info.Amount = ThunderRoarDamage * Power;
            Info.Type = EALSAEQADamageType::Thunder;
            Info.Instigator = Owner;
            Info.HitLocation = Target->GetActorLocation();
            IALSAEQADamageReceiver::Execute_ReceiveALSAEQADamage(Target, Info);
            Damaged.Add(Target);
        }
    }
    return true;
}

bool UALSAEQAMountAbilityComponent::ExecuteLightningKick(float Power)
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World)
    {
        return false;
    }

    const FVector Start = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 140.0f;
    const FVector End = Start + Owner->GetActorForwardVector() * 280.0f;
    TArray<FHitResult> Hits;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(ALSAEQALightningKick), false, Owner);
    if (!World->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(100.0f), Params))
    {
        return true;
    }

    TSet<AActor*> Damaged;
    for (const FHitResult& Hit : Hits)
    {
        AActor* Target = Hit.GetActor();
        if (!IsValid(Target) || Target == Owner || Damaged.Contains(Target) || !Target->GetClass()->ImplementsInterface(UALSAEQADamageReceiver::StaticClass()))
        {
            continue;
        }

        FALSAEQADamageInfo Info;
        Info.Amount = LightningKickDamage * Power;
        Info.Type = EALSAEQADamageType::Thunder;
        Info.Instigator = Owner;
        Info.HitLocation = Hit.ImpactPoint;
        IALSAEQADamageReceiver::Execute_ReceiveALSAEQADamage(Target, Info);
        Damaged.Add(Target);
    }
    return true;
}

bool UALSAEQAMountAbilityComponent::ExecuteStormSummon(float Power)
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World)
    {
        return false;
    }

    const FVector Start = Owner->GetActorLocation() + FVector(0.0f, 0.0f, 1000.0f);
    const FVector End = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 850.0f;
    TArray<FHitResult> Hits;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(ALSAEQAStormSummon), false, Owner);
    if (!World->LineTraceMultiByChannel(Hits, Start, End, ECC_Pawn, Params))
    {
        return true;
    }

    for (const FHitResult& Hit : Hits)
    {
        AActor* Target = Hit.GetActor();
        if (!IsValid(Target) || !Target->GetClass()->ImplementsInterface(UALSAEQADamageReceiver::StaticClass()))
        {
            continue;
        }
        FALSAEQADamageInfo Info;
        Info.Amount = StormSummonDamage * Power;
        Info.Type = EALSAEQADamageType::Thunder;
        Info.Instigator = Owner;
        Info.HitLocation = Hit.ImpactPoint;
        IALSAEQADamageReceiver::Execute_ReceiveALSAEQADamage(Target, Info);
        return true;
    }
    return true;
}

bool UALSAEQAMountAbilityComponent::ExecuteAbility(EALSAEQAMountAbility Ability, float& OutPower)
{
    OutPower = bStormModeActive ? StormModeMultiplier : 1.0f;
    AALSAEQAMountActor* Mount = Cast<AALSAEQAMountActor>(GetOwner());
    UALSAEQAMountComponent* Component = Mount ? Mount->GetMountComponent() : nullptr;
    if (!Mount || !Component)
    {
        return false;
    }

    switch (Ability)
    {
        case EALSAEQAMountAbility::LightningDash:
            Mount->LaunchCharacter(Mount->GetActorForwardVector() * LightningDashDistance + FVector(0, 0, 100), true, true);
            SetCooldown(Ability, 3.0f);
            return true;
        case EALSAEQAMountAbility::ThunderRoar:
            SetCooldown(Ability, 6.0f);
            return ExecuteThunderRoar(OutPower);
        case EALSAEQAMountAbility::LightningKick:
            SetCooldown(Ability, 2.5f);
            return ExecuteLightningKick(OutPower);
        case EALSAEQAMountAbility::StormCharge:
            Component->RestoreStamina(35.0f);
            SetCooldown(Ability, 12.0f);
            return true;
        case EALSAEQAMountAbility::LightningCrossing:
            SetCooldown(Ability, 8.0f);
            return true;
        case EALSAEQAMountAbility::StormLeap:
            Mount->LaunchCharacter(Mount->GetActorForwardVector() * 450.0f + FVector(0, 0, 950.0f), true, true);
            SetCooldown(Ability, 5.0f);
            return true;
        case EALSAEQAMountAbility::LightningShield:
            LightningShieldRemaining = ShieldDuration;
            bLightningShieldActive = true;
            SetCooldown(Ability, 15.0f);
            return true;
        case EALSAEQAMountAbility::StormSummon:
            SetCooldown(Ability, 10.0f);
            return ExecuteStormSummon(OutPower);
        case EALSAEQAMountAbility::ThunderSense:
            SetCooldown(Ability, 20.0f);
            return true;
        case EALSAEQAMountAbility::StormMode:
            StormModeRemaining = StormModeDuration;
            bStormModeActive = true;
            SetCooldown(Ability, 30.0f);
            return true;
        default:
            return false;
    }
}

bool UALSAEQAMountAbilityComponent::TryActivate(EALSAEQAMountAbility Ability)
{
    if (!CanActivate(Ability))
    {
        return false;
    }

    float Power = 1.0f;
    if (!ExecuteAbility(Ability, Power))
    {
        return false;
    }

    OnAbilityActivated.Broadcast(Ability, Power);
    return true;
}
