// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BoatExitPoint.h"

#include "VRBoatGameModeBase.h"

#include "DrawDebugHelpers.h"
ABoatExitPoint::ABoatExitPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Exit Point"));
}

void ABoatExitPoint::BeginPlay()
{
	Super::BeginPlay();

	if (AVRBoatGameModeBase * GameMode = Cast<AVRBoatGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->ExitPoints.AddUnique(this);
	}
}