#include "MovableBox.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h" // For logging

AMovableBox::AMovableBox()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and attach the mesh component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_ChamferCube9"));
    RootComponent = MeshComponent;

    // Enable physics simulation on the mesh
    MeshComponent->SetSimulatePhysics(true);

    // Add rewindable tag for registration by RewindManager
    Tags.Add(FName("Rewindable"));

    // Calculate max buffer size based on max rewind time and record interval
    MaxBufferSize = FMath::CeilToInt(MaxRewindTime / RecordInterval);
}

void AMovableBox::BeginPlay()
{
    Super::BeginPlay();
}

void AMovableBox::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsRewinding)
    {
        Rewind(DeltaTime);
    }
    else
    {
        RecordState(DeltaTime);
    }
}

void AMovableBox::RecordState(float DeltaTime)
{
    TimeSinceLastRecord += DeltaTime;
    if (TimeSinceLastRecord < RecordInterval)
        return;

    TimeSinceLastRecord = 0.0f;

    // Insert snapshot at front (stack behavior)
    SnapshotBuffer.Insert(TakeSnapshot_Implementation(), 0);

    if (SnapshotBuffer.Num() > MaxBufferSize)
    {
        SnapshotBuffer.Pop(); // Remove oldest snapshot
    }

    UE_LOG(LogTemp, Verbose, TEXT("%s recorded snapshot. Buffer size: %d"), *GetName(), SnapshotBuffer.Num());
}

void AMovableBox::Rewind(float DeltaTime)
{
    if (SnapshotBuffer.Num() > 0)
    {
        RestoreSnapshot_Implementation(SnapshotBuffer[0]);
        SnapshotBuffer.RemoveAt(0);

        UE_LOG(LogTemp, Verbose, TEXT("%s rewinding. Remaining snapshots: %d"), *GetName(), SnapshotBuffer.Num());
    }
    else
    {
        StopRewind_Implementation();
    }
}

FMovableObjectSnapshot AMovableBox::TakeSnapshot_Implementation()
{
    return FMovableObjectSnapshot(this);
}

void AMovableBox::RestoreSnapshot_Implementation(const FMovableObjectSnapshot& Snapshot)
{
    Snapshot.Restore(this);
}

void AMovableBox::StartRewind_Implementation()
{
    bIsRewinding = true;

    if (MeshComponent)
    {
        MeshComponent->SetSimulatePhysics(false);
    }

    UE_LOG(LogTemp, Log, TEXT("%s started rewinding"), *GetName());
}

void AMovableBox::StopRewind_Implementation()
{
    if (!bIsRewinding) return;

    bIsRewinding = false;

    if (MeshComponent)
    {
        MeshComponent->SetSimulatePhysics(true);
    }

    SnapshotBuffer.Empty();

    UE_LOG(LogTemp, Log, TEXT("%s stopped rewinding and cleared snapshots"), *GetName());
}
