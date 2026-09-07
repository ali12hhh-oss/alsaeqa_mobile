#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu/ALSAEQAMainMenuTypes.h"
#include "ALSAEQAMainMenuWidget.generated.h"

class UButton;
class UTextBlock;
class UVerticalBox;

UCLASS(Abstract, Blueprintable)
class ALSAEQA_API UALSAEQAMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Main Menu")
    void RefreshFromGameState();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Main Menu")
    FALSAEQAMainMenuState GetMenuState() const { return MenuState; }

    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Main Menu|Presentation")
    void ApplyCinematicMenuState(const FALSAEQAMainMenuState& State);

    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Main Menu|Presentation")
    void PlayMenuTransition(EALSAEQAMainMenuAction Action);\n\n    UFUNCTION(BlueprintImplementableEvent, Category="ALSAEQA|Main Menu|Presentation")\n    void RequestNewJourneyConfirmation();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void HandleStateChanged(const FALSAEQAMainMenuState& State);

    UFUNCTION()
    void HandleAction(EALSAEQAMainMenuAction Action);

    UFUNCTION()
    void OnContinueClicked();

    UFUNCTION()
    void OnNewJourneyClicked();

    UFUNCTION()
    void OnWorldClicked();

    UFUNCTION()
    void OnAchievementsClicked();

    UFUNCTION()
    void OnSettingsClicked();

private:
    void BuildFallbackControls();
    void SetContinueEnabled(bool bEnabled);

    UPROPERTY()
    FALSAEQAMainMenuState MenuState;

    UPROPERTY()
    UVerticalBox* RuntimeMenuBox = nullptr;

    UPROPERTY()
    UButton* ContinueButton = nullptr;

    UPROPERTY()
    UButton* NewJourneyButton = nullptr;

    UPROPERTY()
    UButton* WorldButton = nullptr;

    UPROPERTY()
    UButton* AchievementsButton = nullptr;

    UPROPERTY()
    UButton* SettingsButton = nullptr;
};
