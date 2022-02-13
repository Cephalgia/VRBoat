// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PlayerHandActor.h"

APlayerHandActor::APlayerHandActor()
{
	//PrimaryActorTick.bCanEverTick = true;
	Origin = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
	Origin->SetupAttachment(RootComponent);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hand Mesh"));
	HandMesh->SetupAttachment(Origin);
}