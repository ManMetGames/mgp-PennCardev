#include "themechaniccCharacter.h"
#include "themechaniccProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AthemechaniccCharacter

AthemechaniccCharacter::AthemechaniccCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // Create a CameraComponent    
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    PrimaryActorTick.bCanEverTick = true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AthemechaniccCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AthemechaniccCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AthemechaniccCharacter::Move);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AthemechaniccCharacter::Look);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system."), *GetNameSafe(this));
    }

    // Bind rewind input action (single press)
    PlayerInputComponent->BindAction("Rewind", IE_Pressed, this, &AthemechaniccCharacter::StartRewind);
}

void AthemechaniccCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add movement 
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
        AddMovementInput(GetActorRightVector(), MovementVector.X);
    }
}

void AthemechaniccCharacter::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AthemechaniccCharacter::RecordState(float DeltaTime)
{
    TimeSinceLastRecord += DeltaTime;

    if (TimeSinceLastRecord >= RecordInterval)
    {
        TimeSinceLastRecord = 0.0f;

        FVector CurrentLocation = GetActorLocation();
        FRotator CurrentRotation = GetActorRotation();
        FVector CurrentVelocity = GetVelocity();

        FPlayerStateSnapshot NewSnapshot(CurrentLocation, CurrentRotation, CurrentVelocity);

        // Insert at front to keep newest at index 0
        StateBuffer.Insert(NewSnapshot, 0);

        int32 MaxBufferSize = FMath::CeilToInt(MaxRewindTime / RecordInterval);
        if (StateBuffer.Num() > MaxBufferSize)
        {
            StateBuffer.RemoveAt(StateBuffer.Num() - 1);
        }
    }
}

void AthemechaniccCharacter::Rewind(float DeltaTime)
{
    if (StateBuffer.Num() == 0 || RewindIndex >= StateBuffer.Num())
    {
        StopRewind();
        return;
    }

    FPlayerStateSnapshot Snapshot = StateBuffer[RewindIndex];
    SetActorLocation(Snapshot.Location);
    SetActorRotation(Snapshot.Rotation);

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->Velocity = Snapshot.Velocity;
    }

    RewindIndex++;
}

void AthemechaniccCharacter::StartRewind()
{
    if (bIsRewinding || bIsRewindOnCooldown)
    {
        // Optionally notify player rewind is on cooldown
        return;
    }

    bIsRewinding = true;
    RewindIndex = 0;
    RewindDuration = 0.0f;

    // Start cooldown
    bIsRewindOnCooldown = true;
    RewindCooldownTimer = RewindCooldownTime;

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
    }
}

void AthemechaniccCharacter::StopRewind()
{
    bIsRewinding = false;

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
    }

    StateBuffer.Empty();
}

void AthemechaniccCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Handle cooldown timer
    if (bIsRewindOnCooldown)
    {
        RewindCooldownTimer -= DeltaTime;
        if (RewindCooldownTimer <= 0.0f)
        {
            bIsRewindOnCooldown = false;
            RewindCooldownTimer = 0.0f;
        }
    }

    if (bIsRewinding)
    {
        RewindDuration += DeltaTime;

        if (RewindDuration >= MaxRewindTime || RewindIndex >= StateBuffer.Num())
        {
            StopRewind();
        }
        else
        {
            Rewind(DeltaTime);
        }
    }
    else
    {
        RecordState(DeltaTime);
    }
}

float AthemechaniccCharacter::GetRewindCooldownPercent() const
{
    if (bIsRewindOnCooldown && RewindCooldownTime > 0.f)
    {
        return FMath::Clamp(RewindCooldownTimer / RewindCooldownTime, 0.f, 1.f);
    }
    return 0.f;
}

bool AthemechaniccCharacter::IsRewindOnCooldown() const
{
    return bIsRewindOnCooldown;
}

void AthemechaniccCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (RewindCooldownWidgetClass)
    {
        RewindCooldownWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), RewindCooldownWidgetClass);
        if (RewindCooldownWidgetInstance)
        {
            RewindCooldownWidgetInstance->AddToViewport();
        }
    }
}
