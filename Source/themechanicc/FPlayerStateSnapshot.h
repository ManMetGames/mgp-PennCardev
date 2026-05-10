#pragma once

#include "CoreMinimal.h"
#include "FPlayerStateSnapshot.generated.h"

USTRUCT(BlueprintType)
struct FPlayerStateSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Velocity;

    FPlayerStateSnapshot() {}

    FPlayerStateSnapshot(FVector InLocation, FRotator InRotation, FVector InVelocity)
        : Location(InLocation), Rotation(InRotation), Velocity(InVelocity) {
    }
};