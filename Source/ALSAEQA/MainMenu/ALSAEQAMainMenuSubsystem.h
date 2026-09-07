#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MainMenu/ALSAEQAMainMenuTypes.h"
#include "ALSAEQAMainMenuSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAMainMenuStateChanged, const FALSAEQAMainMenuState&, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQAMainMenuActionEvent, EALSAEQAMainMenuAction, Action);

UCLASS()
class ALSAEQA_API UALSAEQAMainMenuSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Main Menu")
    void RefreshState();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Main Menu")
    FALSAEQAMainMenuState GetState() const { return State; }

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Main Menu")
    bool HasContinueJourney() const { return State.bHasSave; }

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Main Menu")
    bool ContinueJourney();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Main Menu")
    bool BeginNewJourney(bool bConfirmExistingSave);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Main Menu")
    bool ConfirmNewJourney();

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Main Menu")
    bool RequiresNewJourneyConfirmation() const { return State.bHasSave; }

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Main Menu")
    void NotifyAction(EALSAEQAMainMenuAction Action);

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Main Menu")
    FALSAEQAMainMenuStateChanged OnStateChanged;

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Main Menu")
    FALSAEQAMainMenuActionEvent OnActionRequested;

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
    UPROPERTY()
    FALSAEQAMainMenuState State;
};
