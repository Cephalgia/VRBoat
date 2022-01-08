// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PlayerMotionController.h"

#include "Components/SkeletalMeshComponent.h"

#include "DrawDebugHelpers.h"

APlayerMotionController::APlayerMotionController()
{
	PrimaryActorTick.bCanEverTick = true;

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VR Origin"));
	VROrigin->SetupAttachment(RootComponent);

	MotionControllerComp = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Motion Controller"));
	MotionControllerComp->SetupAttachment(VROrigin);
}

void APlayerMotionController::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*DrawDebugSphere(GetWorld(), GetActorLocation(), 5.f, 8, FColor::Red);
	DrawDebugSphere(GetWorld(), MotionControllerComp->GetComponentLocation(), 5.f, 8, FColor::Green);*/
}

void APlayerMotionController::SetMotionSource(FName SourceName)
{
	MotionControllerComp->MotionSource = SourceName;
	UClass * HandClass = nullptr;
	if (MotionControllerComp->MotionSource == TEXT("Left"))
	{
		HandClass = LeftHandActorClass.LoadSynchronous();		
	}
	else
	{
		HandClass = RightHandActorClass.LoadSynchronous();
	}

	FTransform SpawnTransform(FVector(0.f, 0.f, 0.f));
	PlayerHandActor = Cast<APlayerHandActor>(GetWorld()->SpawnActor(HandClass, &SpawnTransform));
	PlayerHandActor->bIsLeft = true;
}

FVector APlayerMotionController::GetControllerLocation() const
{
	return MotionControllerComp->GetComponentLocation();
}

float APlayerMotionController::GetControllerYRotation() const
{
	return MotionControllerComp->GetComponentRotation().Roll;
}

void APlayerMotionController::AttachHandTo(USceneComponent * InComponent, FName InSocketName)
{
	PlayerHandActor->AttachToComponent(InComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false), InSocketName);
}
