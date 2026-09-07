#include "Player/ALSAEQACharacter.h"
#include "Systems/ALSAEQAHealthComponent.h"
#include "Systems/ALSAEQAInjuryComponent.h"
#include "Storm/ALSAEQAThunderChargeComponent.h"
#include "Storm/ALSAEQAThunderEnvironmentComponent.h"
#include "Storm/ALSAEQADynamicStormSubsystem.h"
#include "Story/ALSAEQALegacyComponent.h"
#include "Companions/ALSAEQARidingComponent.h"
#include "Companions/ALSAEQAMountActor.h"
#include "Companions/ALSAEQAMountComponent.h"
#include "Save/ALSAEQASaveManager.h"
#include "Visual/ALSAEQAVisualAssetComponent.h"
#include "Camera/CameraComponent.h"
#include "Cinematic/ALSAEQACinematicDirector.h"
#include "Progression/ALSAEQAProgressionComponent.h"
#include "Progression/ALSAEQAStageFlowComponent.h"
#include "Progression/ALSAEQAStageObjectiveComponent.h"
#include "Progression/ALSAEQAStage1MineResponseCoordinator.h"
#include "Interaction/ALSAEQAInteractable.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Combat/ALSAEQADamageTypes.h"
#include "Combat/ALSAEQADamageReceiver.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "EngineUtils.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "GameFramework/PlayerController.h"

AALSAEQACharacter::AALSAEQACharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 360.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 12.0f;
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    AbilityComponent = CreateDefaultSubobject<UALSAEQAAbilityComponent>(TEXT("AbilityComponent"));
    MeleeCombatComponent = CreateDefaultSubobject<UALSAEQAMeleeCombatComponent>(TEXT("MeleeCombatComponent"));
    HealthComponent = CreateDefaultSubobject<UALSAEQAHealthComponent>(TEXT("HealthComponent"));
    ThunderChargeComponent = CreateDefaultSubobject<UALSAEQAThunderChargeComponent>(TEXT("ThunderChargeComponent"));
    LegacyComponent = CreateDefaultSubobject<UALSAEQALegacyComponent>(TEXT("LegacyComponent"));
    RidingComponent = CreateDefaultSubobject<UALSAEQARidingComponent>(TEXT("RidingComponent"));
    VisualAssetComponent = CreateDefaultSubobject<UALSAEQAVisualAssetComponent>(TEXT("VisualAssetComponent"));
    CinematicDirector = CreateDefaultSubobject<UALSAEQACinematicDirector>(TEXT("CinematicDirector"));
    ProgressionComponent = CreateDefaultSubobject<UALSAEQAProgressionComponent>(TEXT("ProgressionComponent"));
    StageFlowComponent = CreateDefaultSubobject<UALSAEQAStageFlowComponent>(TEXT("StageFlowComponent"));
    StageObjectiveComponent = CreateDefaultSubobject<UALSAEQAStageObjectiveComponent>(TEXT("StageObjectiveComponent"));
    Stage1MineResponseCoordinator = CreateDefaultSubobject<UALSAEQAStage1MineResponseCoordinator>(TEXT("Stage1MineResponseCoordinator"));
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->BrakingDecelerationWalking = 1800.0f;
    GetCharacterMovement()->AirControl = 0.35f;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
}

void AALSAEQACharacter::BeginPlay()
{
    Super::BeginPlay();
    if (HealthComponent) HealthComponent->OnDeath.AddDynamic(this, &AALSAEQACharacter::HandlePlayerDeath);
}

void AALSAEQACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AALSAEQACharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AALSAEQACharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AALSAEQACharacter::LookUp);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AALSAEQACharacter::Turn);
    PlayerInputComponent->BindAction(TEXT("LightAttack"), IE_Pressed, this, &AALSAEQACharacter::PerformLightAttack);
    PlayerInputComponent->BindAction(TEXT("HeavyAttack"), IE_Pressed, this, &AALSAEQACharacter::PerformHeavyAttack);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AALSAEQACharacter::StartSprint);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AALSAEQACharacter::StopSprint);
    PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AALSAEQACharacter::ToggleCrouch);
    PlayerInputComponent->BindAction(TEXT("Listen"), IE_Pressed, this, &AALSAEQACharacter::StartListen);
    PlayerInputComponent->BindAction(TEXT("Listen"), IE_Released, this, &AALSAEQACharacter::StopListen);
    PlayerInputComponent->BindAction(TEXT("Roll"), IE_Pressed, this, &AALSAEQACharacter::PerformRoll);
    PlayerInputComponent->BindAction(TEXT("Mount"), IE_Pressed, this, &AALSAEQACharacter::HandleMountInput);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AALSAEQACharacter::InteractWithNearest);
    PlayerInputComponent->BindAction(TEXT("ThunderCharge"), IE_Pressed, this, &AALSAEQACharacter::BeginThunderCharge);
    PlayerInputComponent->BindAction(TEXT("ThunderCharge"), IE_Released, this, &AALSAEQACharacter::ReleaseThunderCharge);
    PlayerInputComponent->BindAction(TEXT("MountLightningDash"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountLightningDash);
    PlayerInputComponent->BindAction(TEXT("MountThunderRoar"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountThunderRoar);
    PlayerInputComponent->BindAction(TEXT("MountLightningKick"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountLightningKick);
    PlayerInputComponent->BindAction(TEXT("MountStormCharge"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountStormCharge);
    PlayerInputComponent->BindAction(TEXT("MountLightningCrossing"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountLightningCrossing);
    PlayerInputComponent->BindAction(TEXT("MountStormLeap"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountStormLeap);
    PlayerInputComponent->BindAction(TEXT("MountLightningShield"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountLightningShield);
    PlayerInputComponent->BindAction(TEXT("MountStormSummon"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountStormSummon);
    PlayerInputComponent->BindAction(TEXT("MountThunderSense"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountThunderSense);
    PlayerInputComponent->BindAction(TEXT("MountStormMode"), IE_Pressed, this, &AALSAEQACharacter::ActivateMountStormMode);
}

void AALSAEQACharacter::HandleMountInput() { MountOrDismount(); }

bool AALSAEQACharacter::InteractWithNearest()
{
    if (bDeathInProgress || !GetWorld()) return false;
    AALSAEQAInteractable* BestInteractable = nullptr;
    float BestDistanceSquared = FMath::Square(InteractionRange);
    const FVector HeroLocation = GetActorLocation();
    const FVector Forward = GetActorForwardVector();
    for (TActorIterator<AALSAEQAInteractable> It(GetWorld()); It; ++It)
    {
        AALSAEQAInteractable* Candidate = *It;
        if (!IsValid(Candidate)) continue;
        const FVector ToCandidate = Candidate->GetActorLocation() - HeroLocation;
        const float DistanceSquared = ToCandidate.SizeSquared();
        if (DistanceSquared > BestDistanceSquared || DistanceSquared <= KINDA_SMALL_NUMBER) continue;
        if (FVector::DotProduct(Forward, ToCandidate.GetSafeNormal()) < 0.25f) continue;
        BestDistanceSquared = DistanceSquared;
        BestInteractable = Candidate;
    }
    if (!BestInteractable) return false;
    BestInteractable->Interact(this);
    return true;
}

void AALSAEQACharacter::MoveForward(float Value)
{
    if (bDeathInProgress) return;
    if (RidingComponent && RidingComponent->IsRiding()) { RidingComponent->MoveForward(Value); return; }
    if (Controller && !FMath::IsNearlyZero(Value))
    {
        const FRotator R(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
        AddMovementInput(FRotationMatrix(R).GetUnitAxis(EAxis::X), Value);
    }
}

void AALSAEQACharacter::MoveRight(float Value)
{
    if (bDeathInProgress) return;
    if (RidingComponent && RidingComponent->IsRiding()) { RidingComponent->MoveRight(Value); return; }
    if (Controller && !FMath::IsNearlyZero(Value))
    {
        const FRotator R(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
        AddMovementInput(FRotationMatrix(R).GetUnitAxis(EAxis::Y), Value);
    }
}

void AALSAEQACharacter::LookUp(float Value) { if (!bDeathInProgress) AddControllerPitchInput(Value); }
void AALSAEQACharacter::Turn(float Value) { if (!bDeathInProgress) AddControllerYawInput(Value * (bListening ? ListenTurnRateScale : 1.0f)); }

void AALSAEQACharacter::StartSprint()
{
    if (bDeathInProgress) return;
    if (RidingComponent && RidingComponent->IsRiding()) { RidingComponent->SetSprint(true); return; }
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AALSAEQACharacter::StopSprint()
{
    if (RidingComponent && RidingComponent->IsRiding()) { RidingComponent->SetSprint(false); return; }
    GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->IsCrouching() ? CrouchSpeed : WalkSpeed;
}

void AALSAEQACharacter::ToggleCrouch()
{
    if (bDeathInProgress || bRolling || IsRiding() || bListening) return;
    if (GetCharacterMovement()->IsCrouching())
    {
        StopCrouching();
        GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        PlayMovementPresentation(TEXT("Stand"));
    }
    else
    {
        StopSprint();
        Crouch();
        GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
        PlayMovementPresentation(TEXT("Crouch"));
    }
}

void AALSAEQACharacter::StartListen()
{
    if (bDeathInProgress || bRolling || IsRiding()) return;
    bListening = true;
    StopSprint();
    if (!GetCharacterMovement()->IsCrouching()) Crouch();
    GetCharacterMovement()->MaxWalkSpeed = 0.0f;
    PlayListenPresentation(true);
    PlayMovementPresentation(TEXT("Listen"));
}

void AALSAEQACharacter::StopListen()
{
    if (!bListening) return;
    bListening = false;
    GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->IsCrouching() ? CrouchSpeed : WalkSpeed;
    PlayListenPresentation(false);
}

void AALSAEQACharacter::PerformRoll()
{
    if (bDeathInProgress || bRolling || !bRollReady || IsRiding() || bListening || !GetCharacterMovement()->IsMovingOnGround()) return;
    FVector Direction = GetLastMovementInputVector();
    if (Direction.IsNearlyZero()) Direction = GetActorForwardVector();
    Direction.Z = 0.0f;
    Direction.Normalize();
    if (Direction.IsNearlyZero()) return;
    StopSprint();
    if (GetCharacterMovement()->IsCrouching()) StopCrouching();
    bRolling = true;
    bRollReady = false;
    GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
    LaunchCharacter(Direction * RollStrength, false, false);
    PlayRollPresentation();
    GetWorldTimerManager().SetTimer(RollTimerHandle, this, &AALSAEQACharacter::FinishRoll, RollDuration, false);
    GetWorldTimerManager().SetTimer(RollCooldownTimerHandle, FTimerDelegate::CreateLambda([this]() { bRollReady = true; }), RollCooldown, false);
}

void AALSAEQACharacter::FinishRoll()
{
    bRolling = false;
    GetCharacterMovement()->BrakingFrictionFactor = 2.0f;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

bool AALSAEQACharacter::IsRiding() const { return RidingComponent && RidingComponent->IsRiding(); }
bool AALSAEQACharacter::DismountCurrentMount() { return RidingComponent && RidingComponent->Dismount(); }

bool AALSAEQACharacter::MountNearestTamedMount()
{
    if (bDeathInProgress || !RidingComponent || RidingComponent->IsRiding()) return false;
    UWorld* World = GetWorld();
    if (!World) return false;
    AALSAEQAMountActor* BestMount = nullptr;
    float BestDistanceSquared = FMath::Square(MountSearchRadius);
    for (TActorIterator<AALSAEQAMountActor> It(World); It; ++It)
    {
        AALSAEQAMountActor* Mount = *It;
        if (!IsValid(Mount) || !Mount->GetMountComponent() || !Mount->GetMountComponent()->HasTamedMount() || Mount->HasRider()) continue;
        const float DistanceSquared = FVector::DistSquared(GetActorLocation(), Mount->GetActorLocation());
        if (DistanceSquared <= BestDistanceSquared) { BestDistanceSquared = DistanceSquared; BestMount = Mount; }
    }
    return BestMount && RidingComponent->TryMount(BestMount);
}

bool AALSAEQACharacter::MountOrDismount() { return IsRiding() ? DismountCurrentMount() : MountNearestTamedMount(); }

bool AALSAEQACharacter::ActivateMountAbility(EALSAEQAMountAbility Ability)
{
    if (bDeathInProgress || !RidingComponent || !RidingComponent->IsRiding()) return false;
    AALSAEQAMountActor* Mount = RidingComponent->GetCurrentMount();
    UALSAEQAMountAbilityComponent* Abilities = Mount ? Mount->GetMountAbilityComponent() : nullptr;
    return Abilities && Abilities->TryActivate(Ability);
}

void AALSAEQACharacter::ActivateMountLightningDash() { ActivateMountAbility(EALSAEQAMountAbility::LightningDash); }
void AALSAEQACharacter::ActivateMountThunderRoar() { ActivateMountAbility(EALSAEQAMountAbility::ThunderRoar); }
void AALSAEQACharacter::ActivateMountLightningKick() { ActivateMountAbility(EALSAEQAMountAbility::LightningKick); }
void AALSAEQACharacter::ActivateMountStormCharge() { ActivateMountAbility(EALSAEQAMountAbility::StormCharge); }
void AALSAEQACharacter::ActivateMountLightningCrossing() { ActivateMountAbility(EALSAEQAMountAbility::LightningCrossing); }
void AALSAEQACharacter::ActivateMountStormLeap() { ActivateMountAbility(EALSAEQAMountAbility::StormLeap); }
void AALSAEQACharacter::ActivateMountLightningShield() { ActivateMountAbility(EALSAEQAMountAbility::LightningShield); }
void AALSAEQACharacter::ActivateMountStormSummon() { ActivateMountAbility(EALSAEQAMountAbility::StormSummon); }
void AALSAEQACharacter::ActivateMountThunderSense() { ActivateMountAbility(EALSAEQAMountAbility::ThunderSense); }
void AALSAEQACharacter::ActivateMountStormMode() { ActivateMountAbility(EALSAEQAMountAbility::StormMode); }

bool AALSAEQACharacter::PerformMeleeStrike(bool bHeavy)
{
    if (bDeathInProgress || !MeleeCombatComponent) return false;
    const bool bStarted = bHeavy ? MeleeCombatComponent->HeavyAttack() : MeleeCombatComponent->LightAttack();
    if (!bStarted) return false;
    if (CinematicDirector && bHeavy)
    {
        FALSAEQACinematicRequest Moment;
        Moment.Event = EALSAEQACinematicEvent::CombatFinisher;
        Moment.SlowMotionScale = 0.35f;
        Moment.Duration = 0.75f;
        CinematicDirector->PlayActionMoment(Moment);
    }
    UWorld* World = GetWorld();
    if (!World) return true;
    const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 45.0f);
    const FVector End = Start + GetActorForwardVector() * MeleeAttackRange;
    const FCollisionShape Shape = FCollisionShape::MakeSphere(MeleeAttackRadius);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(ALSAEQAMeleeStrike), false, this);
    Params.AddIgnoredActor(this);
    FCollisionObjectQueryParams ObjectTypes;
    ObjectTypes.AddObjectTypesToQuery(ECC_Pawn);
    TArray<FHitResult> Hits;
    World->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjectTypes, Shape, Params);
    TSet<AActor*> Damaged;
    const float Damage = bHeavy ? MeleeCombatComponent->HeavyDamage : MeleeCombatComponent->LightDamage;
    for (const FHitResult& Hit : Hits)
    {
        AActor* Target = Hit.GetActor();
        if (!IsValid(Target) || Damaged.Contains(Target)) continue;
        if (MeleeCombatComponent->TryHitActor(Target, Damage, EALSAEQADamageType::Physical)) Damaged.Add(Target);
    }
    return true;
}

void AALSAEQACharacter::PerformLightAttack() { PerformMeleeStrike(false); }
void AALSAEQACharacter::PerformHeavyAttack() { PerformMeleeStrike(true); }

void AALSAEQACharacter::BeginThunderCharge()
{
    if (!bDeathInProgress && ThunderChargeComponent && AbilityComponent && AbilityComponent->HasAbility(EALSAEQAAbility::ThunderShock)) ThunderChargeComponent->BeginCharge();
}

int32 AALSAEQACharacter::ApplyThunderReleaseToTargets(float Damage)
{
    UWorld* World = GetWorld();
    if (!World || Damage <= 0.0f) return 0;
    float StormMultiplier = 1.0f;
    if (UALSAEQADynamicStormSubsystem* Storm = World->GetSubsystem<UALSAEQADynamicStormSubsystem>()) StormMultiplier = Storm->GetThunderMultiplier();
    const float FinalDamage = Damage * StormMultiplier;
    const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 45.0f);
    const FVector End = Start + GetActorForwardVector() * ThunderAttackRange;
    const FCollisionShape Shape = FCollisionShape::MakeSphere(ThunderAttackRadius);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(ALSAEQAThunderRelease), false, this);
    Params.AddIgnoredActor(this);
    FCollisionObjectQueryParams ObjectTypes;
    ObjectTypes.AddObjectTypesToQuery(ECC_Pawn);
    ObjectTypes.AddObjectTypesToQuery(ECC_WorldDynamic);
    TArray<FHitResult> Hits;
    World->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjectTypes, Shape, Params);
    TSet<AActor*> DamagedActors;
    int32 HitCount = 0;
    for (const FHitResult& Hit : Hits)
    {
        AActor* Target = Hit.GetActor();
        if (!IsValid(Target) || DamagedActors.Contains(Target)) continue;
        const FVector HitLocation = Hit.ImpactPoint.IsNearlyZero() ? Target->GetActorLocation() : Hit.ImpactPoint;

        if (UALSAEQAHealthComponent* Health = Target->FindComponentByClass<UALSAEQAHealthComponent>())
        {
            if (!Health->IsDead())
            {
                FALSAEQADamageInfo DamageInfo;
                DamageInfo.Amount = FinalDamage;
                DamageInfo.Type = EALSAEQADamageType::Thunder;
                DamageInfo.Instigator = this;
                DamageInfo.HitLocation = HitLocation;
                Health->ApplyDamageInfo(DamageInfo);
                DamagedActors.Add(Target);
                ++HitCount;
                continue;
            }
        }

        if (Target->GetClass()->ImplementsInterface(UALSAEQADamageReceiver::StaticClass()))
        {
            FALSAEQADamageInfo DamageInfo;
            DamageInfo.Amount = FinalDamage;
            DamageInfo.Type = EALSAEQADamageType::Thunder;
            DamageInfo.Instigator = this;
            DamageInfo.HitLocation = HitLocation;
            IALSAEQADamageReceiver::Execute_ReceiveALSAEQADamage(Target, DamageInfo);
            DamagedActors.Add(Target);
            ++HitCount;
            continue;
        }

        TArray<UALSAEQAThunderEnvironmentComponent*> EnvironmentComponents;
        Target->GetComponents<UALSAEQAThunderEnvironmentComponent>(EnvironmentComponents);
        for (UALSAEQAThunderEnvironmentComponent* Environment : EnvironmentComponents)
        {
            if (!IsValid(Environment)) continue;
            FALSAEQADamageInfo ThunderInfo;
            ThunderInfo.Amount = FinalDamage;
            ThunderInfo.Type = EALSAEQADamageType::Thunder;
            ThunderInfo.Instigator = this;
            ThunderInfo.HitLocation = HitLocation;
            if (Environment->ReceiveThunder(ThunderInfo)) { DamagedActors.Add(Target); ++HitCount; break; }
        }
    }
    return HitCount;
}

void AALSAEQACharacter::ReleaseThunderCharge()
{
    if (bDeathInProgress || !ThunderChargeComponent || !AbilityComponent || !AbilityComponent->HasAbility(EALSAEQAAbility::ThunderShock)) return;
    const float ChargePercent = ThunderChargeComponent->GetChargePercent();
    const float Multiplier = ThunderChargeComponent->GetDamageMultiplier();
    constexpr float MinimumCharge = 0.15f;
    constexpr float ThunderShockCost = 12.0f;
    if (ChargePercent < MinimumCharge || Multiplier <= 0.0f || !AbilityComponent->ConsumeEnergy(ThunderShockCost))
    {
        ThunderChargeComponent->CancelCharge(); return;
    }
    const float ReleasedPercent = ThunderChargeComponent->ReleaseCharge();
    if (ReleasedPercent <= 0.0f) return;
    ApplyThunderReleaseToTargets(ThunderReleaseDamage * Multiplier);
    if (CinematicDirector && ReleasedPercent >= 0.85f)
    {
        FALSAEQACinematicRequest Moment;
        Moment.Event = EALSAEQACinematicEvent::AbilityImpact;
        Moment.SlowMotionScale = 0.22f;
        Moment.Duration = 1.15f;
        CinematicDirector->PlayActionMoment(Moment);
    }
}

void AALSAEQACharacter::CancelThunderCharge() { if (ThunderChargeComponent) ThunderChargeComponent->CancelCharge(); }
bool AALSAEQACharacter::ActivateAbility(EALSAEQAAbility Ability) { return !bDeathInProgress && AbilityComponent && AbilityComponent->TryActivateAbility(Ability); }

void AALSAEQACharacter::HandlePlayerDeath()
{
    if (bDeathInProgress || !IsValid(this)) return;
    bDeathInProgress = true;
    if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
    if (RidingComponent && RidingComponent->IsRiding()) RidingComponent->Dismount();
    if (ThunderChargeComponent) ThunderChargeComponent->CancelCharge();
    StopJumping();
    if (GetCharacterMovement()) { GetCharacterMovement()->StopMovementImmediately(); GetCharacterMovement()->DisableMovement(); }
    SetActorEnableCollision(false);
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) DisableInput(PlayerController);
    if (GetWorld()) GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AALSAEQACharacter::RespawnAtCheckpoint, FMath::Max(0.1f, DeathRespawnDelay), false);
}

void AALSAEQACharacter::RespawnAtCheckpoint()
{
    if (!IsValid(this)) return;
    UALSAEQASaveManager* SaveManager = nullptr;
    if (UGameInstance* GameInstance = GetGameInstance()) SaveManager = GameInstance->GetSubsystem<UALSAEQASaveManager>();
    FALSAEQACheckpointData Checkpoint;
    bool bHasCheckpoint = false;
    if (SaveManager)
    {
        Checkpoint = SaveManager->GetLastCheckpoint();
        bHasCheckpoint = !Checkpoint.CheckpointId.IsNone();
        if (bHasCheckpoint) SaveManager->RespawnAtLastCheckpoint();
    }
    if (bHasCheckpoint) SetActorLocationAndRotation(Checkpoint.PlayerLocation, Checkpoint.PlayerRotation, false, nullptr, ETeleportType::TeleportPhysics);
    if (GetCharacterMovement()) { GetCharacterMovement()->StopMovementImmediately(); GetCharacterMovement()->SetMovementMode(MOVE_Walking); GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; }
    SetActorEnableCollision(true);
    if (HealthComponent) HealthComponent->ResetHealth();
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        EnableInput(PlayerController);
        if (bHasCheckpoint) PlayerController->SetControlRotation(Checkpoint.PlayerRotation);
    }
    bDeathInProgress = false;
}
