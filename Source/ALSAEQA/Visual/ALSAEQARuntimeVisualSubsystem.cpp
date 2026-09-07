#include "Visual/ALSAEQARuntimeVisualSubsystem.h"
#include "Visual/ALSAEQARuntimeVisualComponent.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
void UALSAEQARuntimeVisualSubsystem::Tick(float DeltaTime){(void)DeltaTime; UWorld* W=GetWorld(); if(!W)return; for(TActorIterator<ACharacter> It(W);It;++It) EnsureVisual(*It);}
void UALSAEQARuntimeVisualSubsystem::EnsureVisual(AActor* Actor){if(!IsValid(Actor)||Actor->FindComponentByClass<UALSAEQARuntimeVisualComponent>())return; FString N=Actor->GetClass()->GetName(); EALSAEQAVisualRole R=EALSAEQAVisualRole::Hero; if(N.Contains(TEXT("Boss")))R=EALSAEQAVisualRole::Boss; else if(N.Contains(TEXT("Snake")))R=EALSAEQAVisualRole::Snake; else if(N.Contains(TEXT("Enemy")))R=EALSAEQAVisualRole::Enemy; else if(N.Contains(TEXT("Companion")))R=EALSAEQAVisualRole::Companion; else if(N.Contains(TEXT("Mount")))R=EALSAEQAVisualRole::Mount; UALSAEQARuntimeVisualComponent* V=NewObject<UALSAEQARuntimeVisualComponent>(Actor,TEXT("ALSAEQARuntimeVisual")); if(!V)return; V->RegisterComponent(); V->SetVisualRole(R);}
