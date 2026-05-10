#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MovableObjectSnapshot.h" // Your snapshot struct
#include "IRewindable.generated.h"

UINTERFACE(MinimalAPI)
class URewindable : public UInterface
{
    GENERATED_BODY()
};

class IRewindable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Rewind")
    FMovableObjectSnapshot TakeSnapshot();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Rewind")
    void RestoreSnapshot(const FMovableObjectSnapshot& Snapshot);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Rewind")
    void StartRewind();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Rewind")
    void StopRewind();
};
