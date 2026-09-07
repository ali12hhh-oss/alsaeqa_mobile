#include "Boss/ALSAEQABossCharacter.h"

AALSAEQABossCharacter::AALSAEQABossCharacter()
{
    BossComponent = CreateDefaultSubobject<UALSAEQABossComponent>(TEXT("BossComponent"));
    BossMaxHealth = 1200.0f;
    BossHealth = BossMaxHealth;
    AttackDamage = 24.0f;
    AttackCooldown = 1.10f;
    AttackRange = 220.0f;
    DetectionRange = 2200.0f;
    ChaseSpeed = 460.0f;
}

void AALSAEQABossCharacter::BeginPlay()
{
    Super::BeginPlay();
    BossHealth = FMath::Max(BossMaxHealth, 1.0f);
    if (BossComponent)
    {
        BossComponent->SetHealthPercent(1.0f);
        BossComponent->OnPhaseChanged.AddDynamic(this, &AALSAEQABossCharacter::HandleBossPhaseChanged);
    }
}

float AALSAEQABossCharacter::ReceiveALSAEQADamage_Implementation(const FALSAEQADamageInfo& DamageInfo)
{
    return ReceiveBossDamage(DamageInfo) ? DamageInfo.Amount : 0.0f;
}

bool AALSAEQABossCharacter::ReceiveBossDamage(const FALSAEQADamageInfo& DamageInfo)
{
    if (!BossComponent || BossComponent->IsDefeated() || DamageInfo.Amount <= 0.0f)
    {
        return false;
    }

    BossHealth = FMath::Max(0.0f, BossHealth - DamageInfo.Amount);
    const float Percent = BossHealth / FMath::Max(BossMaxHealth, 1.0f);
    BossComponent->SetHealthPercent(Percent);
    if (BossComponent->IsDefeated())
    {
        SetEnemyState(EALSAEQAEnemyState::Dead);
    }
    else if (DamageInfo.Instigator)
    {
        SetTargetActor(DamageInfo.Instigator);
        SetEnemyState(EALSAEQAEnemyState::Alert);
    }
    return true;
}

void AALSAEQABossCharacter::HandleBossPhaseChanged(EALSAEQABossPhase NewPhase)
{
    // Each phase increases combat pressure instead of only changing a UI state.
    switch (NewPhase)
    {
    case EALSAEQABossPhase::PhaseOne:
        AttackDamage = 24.0f;
        AttackCooldown = 1.10f;
        ChaseSpeed = 460.0f;
        break;
    case EALSAEQABossPhase::PhaseTwo:
        AttackDamage = 34.0f;
        AttackCooldown = 0.90f;
        ChaseSpeed = 485.0f;
        break;
    case EALSAEQABossPhase::PhaseThree:
        AttackDamage = 48.0f;
        AttackCooldown = 0.72f;
        ChaseSpeed = 515.0f;
        break;
    case EALSAEQABossPhase::Defeated:
        SetEnemyState(EALSAEQAEnemyState::Dead);
        return;
    }

    SetEnemyState(EALSAEQAEnemyState::Alert);
}
