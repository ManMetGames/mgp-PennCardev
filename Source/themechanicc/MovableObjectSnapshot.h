#pragma once

#include "CoreMinimal.h"
#include "MovableObjectSnapshot.generated.h"

USTRUCT(BlueprintType)
struct FMovableObjectSnapshot
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FVector Location;

    UPROPERTY()
    FRotator Rotation;

    FMovableObjectSnapshot() {}

    // Construct snapshot from an actor
    FMovableObjectSnapshot(const AActor* Actor)
    {
        if (Actor)
        {
            Location = Actor->GetActorLocation();
            Rotation = Actor->GetActorRotation();
            // Capture other states if needed
        }
    }

    // Restore actor state from snapshot
    void Restore(AActor* Actor) const
    {
        if (Actor)
        {
            Actor->SetActorLocation(Location);
            Actor->SetActorRotation(Rotation);
            // Restore other states if needed
        }
    }
};
