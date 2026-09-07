#include "MainMenu/ALSAEQAMainMenuWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Save/ALSAEQASaveManager.h"
#include "MainMenu/ALSAEQAMainMenuSubsystem.h"

void UALSAEQAMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BuildFallbackControls();

    if (GetGameInstance())
    {
        if (UALSAEQAMainMenuSubsystem* Menu = GetGameInstance()->GetSubsystem<UALSAEQAMainMenuSubsystem>())
        {
            Menu->OnStateChanged.AddDynamic(this, &UALSAEQAMainMenuWidget::HandleStateChanged);
            Menu->OnActionRequested.AddDynamic(this, &UALSAEQAMainMenuWidget::HandleAction);
            Menu->RefreshState();
        }
    }
}

void UALSAEQAMainMenuWidget::NativeDestruct()
{
    if (GetGameInstance())
    {
        if (UALSAEQAMainMenuSubsystem* Menu = GetGameInstance()->GetSubsystem<UALSAEQAMainMenuSubsystem>())
        {
            Menu->OnStateChanged.RemoveDynamic(this, &UALSAEQAMainMenuWidget::HandleStateChanged);
            Menu->OnActionRequested.RemoveDynamic(this, &UALSAEQAMainMenuWidget::HandleAction);
        }
    }
    Super::NativeDestruct();
}

void UALSAEQAMainMenuWidget::BuildFallbackControls()
{
    if (!WidgetTree || WidgetTree->RootWidget) return;

    RuntimeMenuBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainMenuActions"));
    WidgetTree->RootWidget = RuntimeMenuBox;

    auto AddAction = [this](const TCHAR* Name, const FText& Label, void (UALSAEQAMainMenuWidget::*Handler)(), UButton*& OutButton)
    {
        OutButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
        UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Text->SetText(Label);
        OutButton->AddChild(Text);
        RuntimeMenuBox->AddChildToVerticalBox(OutButton);
        OutButton->OnClicked.AddDynamic(this, Handler);
    };

    AddAction(TEXT("ContinueJourney"), FText::FromString(TEXT("متابعة الرحلة")), &UALSAEQAMainMenuWidget::OnContinueClicked, ContinueButton);
    AddAction(TEXT("NewJourney"), FText::FromString(TEXT("رحلة جديدة")), &UALSAEQAMainMenuWidget::OnNewJourneyClicked, NewJourneyButton);
    AddAction(TEXT("World"), FText::FromString(TEXT("العالم")), &UALSAEQAMainMenuWidget::OnWorldClicked, WorldButton);
    AddAction(TEXT("Achievements"), FText::FromString(TEXT("الإنجازات")), &UALSAEQAMainMenuWidget::OnAchievementsClicked, AchievementsButton);
    AddAction(TEXT("Settings"), FText::FromString(TEXT("الإعدادات")), &UALSAEQAMainMenuWidget::OnSettingsClicked, SettingsButton);
}

void UALSAEQAMainMenuWidget::RefreshFromGameState()
{
    if (!GetGameInstance()) return;
    if (UALSAEQAMainMenuSubsystem* Menu = GetGameInstance()->GetSubsystem<UALSAEQAMainMenuSubsystem>())
        Menu->RefreshState();
}

void UALSAEQAMainMenuWidget::HandleStateChanged(const FALSAEQAMainMenuState& State)
{
    MenuState = State;
    SetContinueEnabled(State.bHasSave);
    ApplyCinematicMenuState(State);
}

void UALSAEQAMainMenuWidget::HandleAction(EALSAEQAMainMenuAction Action)
{
    PlayMenuTransition(Action);
}

void UALSAEQAMainMenuWidget::SetContinueEnabled(bool bEnabled)
{
    if (ContinueButton) ContinueButton->SetIsEnabled(bEnabled);
}

void UALSAEQAMainMenuWidget::OnContinueClicked()
{
    if (!GetGameInstance()) return;
    if (UALSAEQAMainMenuSubsystem* Menu = GetGameInstance()->GetSubsystem<UALSAEQAMainMenuSubsystem>())
        Menu->ContinueJourney();
}

void UALSAEQAMainMenuWidget::OnNewJourneyClicked()
{
    if (!GetGameInstance()) return;
    if (UALSAEQAMainMenuSubsystem* Menu = GetGameInstance()->GetSubsystem<UALSAEQAMainMenuSubsystem>())
    {
        // Never overwrite an existing journey without explicit confirmation.
        if (Menu->RequiresNewJourneyConfirmation())
        {
            // Blueprint presentation owns the confirmation dialog and may call ConfirmNewJourney().
            return;
        }
        Menu->BeginNewJourney(false);
    }
}

void UALSAEQAMainMenuWidget::OnWorldClicked()
{
    if (!GetGameInstance()) return;
    if (UALSAEQAMainMenuSubsystem* Menu = GetGameInstance()->GetSubsystem<UALSAEQAMainMenuSubsystem>())
        Menu->NotifyAction(EALSAEQAMainMenuAction::World);
}

void UALSAEQAMainMenuWidget::OnAchievementsClicked()
{
    if (!GetGameInstance()) return;
    if (UALSAEQAMainMenuSubsystem* Menu = GetGameInstance()->GetSubsystem<UALSAEQAMainMenuSubsystem>())
        Menu->NotifyAction(EALSAEQAMainMenuAction::Achievements);
}

void UALSAEQAMainMenuWidget::OnSettingsClicked()
{
    if (!GetGameInstance()) return;
    if (UALSAEQAMainMenuSubsystem* Menu = GetGameInstance()->GetSubsystem<UALSAEQAMainMenuSubsystem>())
        Menu->NotifyAction(EALSAEQAMainMenuAction::Settings);
}
