#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALSAEQAInteractable.generated.h"

UCLASS(Abstract, Blueprintable)
class ALSAEQA_API AALSAEQAInteractable : public AActor
{
    GENERATED_BODY()

public:
    AALSAEQAInteractable();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ALSAEQA|Interaction")
    void Interact(AActor* Interactor);
    virtual void Interact_Implementation(AActor* Interactor);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Interaction")
    FText GetInteractionPrompt() const { return InteractionPrompt; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ALSAEQA|Interaction")
    FText InteractionPrompt = NSLOCTEXT("ALSAEQA", "DefaultInteractPrompt", "تفاعل");
};
