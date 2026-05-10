#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorStateSnapshot.generated.h"

USTRUCT()
struct FActorStateSnapshot
{
    GENERATED_BODY()

    FVector Location;
    FRotator Rotation;
    FVector Velocity;

    // Add other properties as needed, e.g., health, animation state, etc.

    FActorStateSnapshot() {}

    FActorStateSnapshot(AActor* Actor)
    {
        Location = Actor->GetActorLocation();
        Rotation = Actor->GetActorRotation();

        if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
        {
            Velocity = PrimComp->GetComponentVelocity();
        }
        else
        {
            Velocity = FVector::ZeroVector;
        }

        // Capture other states here
    }

    void Restore(AActor* Actor) const
    {
        Actor->SetActorLocation(Location);
        Actor->SetActorRotation(Rotation);

        if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
        {
            PrimComp->SetPhysicsLinearVelocity(Velocity);
        }

        // Restore other states here
    }
};
