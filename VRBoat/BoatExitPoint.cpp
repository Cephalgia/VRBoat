// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BoatExitPoint.h"

#include "Components/BillboardComponent.h"

#include "VRBoatGameModeBase.h"

#include "DrawDebugHelpers.h"
ABoatExitPoint::ABoatExitPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Exit Point"));

	BoatStayPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Boat Point"));
	BoatStayPoint->SetupAttachment(GetRootComponent()); 

	BoatPole = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Boat Pole"));
	BoatPole->SetupAttachment(GetRootComponent());

#if WITH_EDITOR
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(GetRootComponent());
	BoatBillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Boat Billboard"));
	BoatBillboardComponent->SetupAttachment(BoatStayPoint);
#endif
}

void ABoatExitPoint::BeginPlay()
{
	Super::BeginPlay();

	if (AVRBoatGameModeBase * GameMode = Cast<AVRBoatGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->ExitPoints.AddUnique(this);
	}
}