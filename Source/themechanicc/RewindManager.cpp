#include "RewindManager.h"
#include "IRewindable.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h" // For logging

ARewindManager::ARewindManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ARewindManager::BeginPlay()
{
    Super::BeginPlay();

    RegisterRewindableBoxesInScene();
}

void ARewindManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // No need to call Rewind on actors here; they handle it in their Tick
}

void ARewindManager::RegisterRewindableBoxesInScene()
{
    TArray<AActor*> RewindableBoxes;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Rewindable"), RewindableBoxes);

    RegisteredRewindables.Empty();

    UE_LOG(LogTemp, Log, TEXT("Found %d rewindable actors with tag 'Rewindable'"), RewindableBoxes.Num());

    for (AActor* Actor : RewindableBoxes)
    {
        if (Actor && Actor->GetClass()->ImplementsInterface(URewindable::StaticClass()))
        {
            RegisteredRewindables.Add(TScriptInterface<IRewindable>(Actor));
            UE_LOG(LogTemp, Log, TEXT("Registered rewindable actor: %s"), *Actor->GetName());
        }
    }
}

void ARewindManager::StartRewind()
{
    bIsRewinding = true;
    UE_LOG(LogTemp, Log, TEXT("Rewind started"));

    for (auto& Rewindable : RegisteredRewindables)
    {
        if (Rewindable)
        {
            IRewindable::Execute_StartRewind(Rewindable.GetObject());
        }
    }
}

void ARewindManager::StopRewind()
{
    bIsRewinding = false;
    UE_LOG(LogTemp, Log, TEXT("Rewind stopped"));

    for (auto& Rewindable : RegisteredRewindables)
    {
        if (Rewindable)
        {
            IRewindable::Execute_StopRewind(Rewindable.GetObject());
        }
    }
}

void ARewindManager::RegisterRewindableActor(AActor* Actor)
{
    if (Actor && Actor->GetClass()->ImplementsInterface(URewindable::StaticClass()))
    {
        RegisteredRewindables.AddUnique(TScriptInterface<IRewindable>(Actor));
        UE_LOG(LogTemp, Log, TEXT("Dynamically registered rewindable actor: %s"), *Actor->GetName());
    }
}
