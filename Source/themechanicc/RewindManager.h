#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IRewindable.h"
#include "RewindManager.generated.h"

UCLASS()
class ARewindManager : public AActor
{
    GENERATED_BODY()

public:
    ARewindManager();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    void StartRewind();
    void StopRewind();

    // Optional: dynamic registration for spawned actors
    void RegisterRewindableActor(AActor* Actor);

private:
    // Store all rewindable actors found by tag
    TArray<TScriptInterface<IRewindable>> RegisteredRewindables;

    bool bIsRewinding = false;

    void RegisterRewindableBoxesInScene();
};
