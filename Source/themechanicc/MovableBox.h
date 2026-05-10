#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IRewindable.h"
#include "MovableBox.generated.h"

UCLASS()
class AMovableBox : public AActor, public IRewindable
{
    GENERATED_BODY()

public:
    AMovableBox();

    virtual void Tick(float DeltaTime) override;

    // Interface implementations
    virtual FMovableObjectSnapshot TakeSnapshot_Implementation() override;
    virtual void RestoreSnapshot_Implementation(const FMovableObjectSnapshot& Snapshot) override;

    virtual void StartRewind_Implementation() override;
    virtual void StopRewind_Implementation() override;

protected:
    virtual void BeginPlay() override;

private:
    // Snapshot buffer for rewind states
    TArray<FMovableObjectSnapshot> SnapshotBuffer;

    // Time interval between recordings
    float RecordInterval = 0.02f;
    float TimeSinceLastRecord = 0.0f;

    // Maximum rewind time in seconds
    float MaxRewindTime = 10.0f;
    int32 MaxBufferSize;

    // Flag to indicate if currently rewinding
    bool bIsRewinding = false;

    // Mesh component representing the box (SM_ChamferCube9)
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComponent;

    // Internal functions
    void RecordState(float DeltaTime);
    void Rewind(float DeltaTime);
};
