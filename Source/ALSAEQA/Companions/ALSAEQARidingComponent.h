#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQARidingComponent.generated.h"

class AALSAEQAMountActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQARidingStateChanged, bool, bRiding);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQARidingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UALSAEQARidingComponent();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding")
    bool TryMount(AALSAEQAMountActor* Mount);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding")
    bool Dismount();

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding")
    bool MoveForward(float Value);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding")
    bool MoveRight(float Value);

    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Riding")
    bool SetSprint(bool bEnabled);

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Riding")
    bool IsRiding() const;

    UFUNCTION(BlueprintPure, Category="ALSAEQA|Riding")
    AALSAEQAMountActor* GetCurrentMount() const { return CurrentMount.Get(); }

    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Riding")
    FALSAEQARidingStateChanged OnRidingStateChanged;

private:
    UPROPERTY(BlueprintReadOnly, SaveGame, Category="ALSAEQA|Riding", meta=(AllowPrivateAccess="true"))
    TWeakObjectPtr<AALSAEQAMountActor> CurrentMount;

    UPROPERTY(BlueprintReadOnly, SaveGame, Category="ALSAEQA|Riding", meta=(AllowPrivateAccess="true"))
    FName CurrentMountId = NAME_None;
};
