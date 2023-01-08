// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "WalkingMovementComponent.h"

#include "BoatPawn.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UWalkingMovementComponent::UWalkingMovementComponent()
{
	
}

void UWalkingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}