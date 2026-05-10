// Copyright Epic Games, Inc. All Rights Reserved.

#include "themechaniccPickUpComponent.h"

UthemechaniccPickUpComponent::UthemechaniccPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UthemechaniccPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UthemechaniccPickUpComponent::OnSphereBeginOverlap);
}

void UthemechaniccPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AthemechaniccCharacter* Character = Cast<AthemechaniccCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
