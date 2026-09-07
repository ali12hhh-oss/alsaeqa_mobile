#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "ALSAEQARuntimeVisualSubsystem.generated.h"
UCLASS()
class ALSAEQA_API UALSAEQARuntimeVisualSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UALSAEQARuntimeVisualSubsystem, STATGROUP_Tickables); }
    virtual bool IsTickable() const override { return true; }
private:
    void EnsureVisual(class AActor* Actor);
};
