#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSAEQATemporaryCompanionComponent.generated.h"

UENUM(BlueprintType)
enum class EALSAEQATemporaryCompanionRole : uint8 { Scout, Fighter, Archer, Medic, Climber, Survivor };

USTRUCT(BlueprintType)
struct FALSAEQATemporaryCompanionProfile
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName CompanionId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) EALSAEQATemporaryCompanionRole Role = EALSAEQATemporaryCompanionRole::Survivor;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float FollowDistance = 240.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AssistRange = 1100.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSAEQATemporaryCompanionEvent, FName, CompanionId);

UCLASS(ClassGroup=(ALSAEQA), meta=(BlueprintSpawnableComponent))
class ALSAEQA_API UALSAEQATemporaryCompanionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALSAEQATemporaryCompanionComponent();
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companions") bool RecruitTemporaryCompanion(const FALSAEQATemporaryCompanionProfile& Profile);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companions") bool ReleaseTemporaryCompanion(FName CompanionId);
    UFUNCTION(BlueprintCallable, Category="ALSAEQA|Companions") void ClearTemporaryCompanions();
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companions") bool HasCompanion(FName CompanionId) const;
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companions") int32 GetCompanionCount() const { return ActiveCompanions.Num(); }
    UFUNCTION(BlueprintPure, Category="ALSAEQA|Companions") const TArray<FALSAEQATemporaryCompanionProfile>& GetCompanions() const { return ActiveCompanions; }
    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Companions") FALSAEQATemporaryCompanionEvent OnCompanionRecruited;
    UPROPERTY(BlueprintAssignable, Category="ALSAEQA|Companions") FALSAEQATemporaryCompanionEvent OnCompanionReleased;
private:
    UPROPERTY(VisibleAnywhere, Category="ALSAEQA|Companions") TArray<FALSAEQATemporaryCompanionProfile> ActiveCompanions;
};
