#include "Progression/ALSAEQAStageFlowComponent.h"

#include "Progression/ALSAEQAProgressionComponent.h"
#include "Progression/ALSAEQAProgressionStageRegistry.h"
#include "Save/ALSAEQASaveManager.h"
#include "Story/ALSAEQALegacyComponent.h"
#include "Cinematic/ALSAEQACinematicDirector.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

UALSAEQAStageFlowComponent::UALSAEQAStageFlowComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALSAEQAStageFlowComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    Progression = Owner ? Owner->FindComponentByClass<UALSAEQAProgressionComponent>() : nullptr;

    if (!Progression && Owner)
    {
        Progression = NewObject<UALSAEQAProgressionComponent>(Owner, UALSAEQAProgressionComponent::StaticClass(), TEXT("ProgressionComponent"));
        if (Progression)
        {
            Progression->RegisterComponent();
        }
    }

    if (!Progression || !GetWorld() || !GetWorld()->GetGameInstance())
    {
        return;
    }

    if (UALSAEQASaveManager* SaveManager = GetWorld()->GetGameInstance()->GetSubsystem<UALSAEQASaveManager>())
    {
        SaveManager->LoadProgress();
        const int32 SavedStage = SaveManager->GetStage();
        if (Progression->IsValidStage(SavedStage) && SavedStage > Progression->GetCurrentStage())
        {
            Progression->AdvanceStage(SavedStage);
        }
    }

    if (UALSAEQALegacyComponent* Legacy = Owner ? Owner->FindComponentByClass<UALSAEQALegacyComponent>() : nullptr)
    {
        Legacy->SetCurrentStage(Progression->GetCurrentStage());
    }
}

bool UALSAEQAStageFlowComponent::CompleteCurrentStage()
{
    if (!Progression || bTransitionPending)
    {
        return false;
    }

    const int32 CurrentStage = Progression->GetCurrentStage();
    const FALSAEQAProgressionStage* Definition = ALSAEQAProgressionStageRegistry::FindStage(CurrentStage);
    if (!Definition || Definition->NextStage == INDEX_NONE)
    {
        return false;
    }

    const int32 NextStage = Definition->NextStage;
    if (!ALSAEQAProgressionStageRegistry::IsValidStage(NextStage))
    {
        return false;
    }

    PendingStage = NextStage;
    bTransitionPending = true;
    OnStageCompleted.Broadcast(CurrentStage);

    if (TransitionDelay <= 0.0f || !GetWorld())
    {
        ApplyPendingTransition();
        return true;
    }

    GetWorld()->GetTimerManager().SetTimer(
        TransitionTimerHandle,
        this,
        &UALSAEQAStageFlowComponent::ApplyPendingTransition,
        FMath::Max(0.05f, TransitionDelay),
        false);

    return true;
}

void UALSAEQAStageFlowComponent::CancelPendingTransition()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(TransitionTimerHandle);
    }

    PendingStage = INDEX_NONE;
    bTransitionPending = false;
}

void UALSAEQAStageFlowComponent::ApplyPendingTransition()
{
    const int32 NextStage = PendingStage;
    PendingStage = INDEX_NONE;
    bTransitionPending = false;

    if (!Progression || !Progression->IsValidStage(NextStage))
    {
        return;
    }

    const int32 PreviousStage = Progression->GetCurrentStage();
    if (Progression->AdvanceStage(NextStage))
    {
        if (GetWorld() && GetWorld()->GetGameInstance())
        {
            if (UALSAEQASaveManager* SaveManager = GetWorld()->GetGameInstance()->GetSubsystem<UALSAEQASaveManager>())
            {
                SaveManager->SetStage(NextStage);
            }
        }

        if (AActor* Owner = GetOwner())
        {
            if (UALSAEQALegacyComponent* Legacy = Owner->FindComponentByClass<UALSAEQALegacyComponent>())
            {
                Legacy->SetCurrentStage(NextStage);
            }

            if (UALSAEQACinematicDirector* Cinematic = Owner->FindComponentByClass<UALSAEQACinematicDirector>())
            {
                Cinematic->StartStoryBeat(EALSAEQACinematicEvent::StageTransition);
            }
        }

        OnAutomaticStageChanged.Broadcast(PreviousStage, NextStage);
    }
}