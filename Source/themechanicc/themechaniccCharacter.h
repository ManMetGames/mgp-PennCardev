#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FPlayerStateSnapshot.h"  // Make sure this header exists and is included
#include "Blueprint/UserWidget.h"  // For UUserWidget
#include "themechaniccCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AthemechaniccCharacter : public ACharacter
{
    GENERATED_BODY()

    /** Pawn mesh: 1st person view (arms; seen only by self) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* Mesh1P;

    /** First person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;

    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

public:
    AthemechaniccCharacter();

    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Called for looking input */
    void Look(const FInputActionValue& Value);

protected:
    // Maximum rewind time in seconds
    const float MaxRewindTime = 5.0f;

    // How often to record state
    const float RecordInterval = 0.02f; // 50 FPS

    // Buffer to store snapshots
    TArray<FPlayerStateSnapshot> StateBuffer;

    // Timer to track recording intervals
    float TimeSinceLastRecord = 0.0f;

    // Flag to check if rewinding
    bool bIsRewinding = false;

    // Current rewind index
    int32 RewindIndex = 0;

    // Duration rewind has been active
    float RewindDuration = 0.0f;

    // Cooldown duration in seconds
    const float RewindCooldownTime = 10.0f;  // Example: 10 seconds cooldown

    // Timer to track cooldown
    float RewindCooldownTimer = 0.0f;

    // Flag to indicate if rewind is on cooldown
    bool bIsRewindOnCooldown = false;

    // Functions to manage rewind mechanic
    void RecordState(float DeltaTime);
    void Rewind(float DeltaTime);
    void StartRewind();
    void StopRewind();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UUserWidget> RewindCooldownWidgetClass;

    UPROPERTY()
    UUserWidget* RewindCooldownWidgetInstance;

protected:
    // APawn interface
    virtual void NotifyControllerChanged() override;
    // End of APawn interface

public:
    /** Returns Mesh1P subobject **/
    USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
    /** Returns FirstPersonCameraComponent subobject **/
    UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

public:
    // Blueprint callable functions to expose cooldown info
    UFUNCTION(BlueprintCallable, Category = "Rewind")
    float GetRewindCooldownPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Rewind")
    bool IsRewindOnCooldown() const;

protected:
    virtual void BeginPlay() override;
};
