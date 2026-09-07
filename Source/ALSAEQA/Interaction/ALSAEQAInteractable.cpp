#include "Interaction/ALSAEQAInteractable.h"

AALSAEQAInteractable::AALSAEQAInteractable()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AALSAEQAInteractable::Interact_Implementation(AActor* Interactor)
{
    // Base interaction intentionally performs no action.
    // Concrete actors implement their own gameplay behavior in C++ or Blueprints.
}
