// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PlayerMotionController.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "PaddleActor.h"
#include "GrabbableMeshComponent.h"

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
	if (PlayerHandActor->GetAttachParentActor() == nullptr)
	{
		PlayerHandActor->SetActorTransform(MotionControllerComp->GetComponentTransform());
	}
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

FVector APlayerMotionController::GetControllerForwardVector() const
{
	return MotionControllerComp->GetComponentRotation().Vector();
}

FRotator APlayerMotionController::GetControllerRotation() const
{
	return MotionControllerComp->GetComponentRotation();
}

float APlayerMotionController::GetControllerYRotation() const
{
	return MotionControllerComp->GetComponentRotation().Roll;
}

void APlayerMotionController::AttachHandTo(USceneComponent * InComponent, FName InSocketName)
{
	PlayerHandActor->AttachToComponent(InComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false), InSocketName);
}

void APlayerMotionController::DetachHand()
{
	PlayerHandActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
}

void APlayerMotionController::Grab(APaddleActor * InPaddle)
{
	UCapsuleComponent * Capsule = Cast<UCapsuleComponent>(InPaddle->GetComponentByClass(UCapsuleComponent::StaticClass()));
	float DistSq = 0.f;
	FVector ClosestPoint;
	bool bFoundDistance = Capsule->GetSquaredDistanceToCollision(PlayerHandActor->GetActorLocation(), DistSq, ClosestPoint);
	if (Capsule && !bHolding && bFoundDistance && DistSq < FMath::Square(10.f))
	{
		//DrawDebugSphere(GetWorld(), ClosestPoint, 5.f, 8, FColor::Green, true);
		//DrawDebugLine(GetWorld(), ClosestPoint, PlayerHandActor->GetActorLocation(), FColor::Green, true);
		bHolding = true;
		if (!InPaddle->bHeld)
		{
			InPaddle->DetachPaddleMesh();
		}
		FName SocketName = MotionControllerComp->MotionSource == TEXT("Left") ? "Handle_L" : "Handle_R";
		//update socket location first, then attach
		InPaddle->GetPaddleMesh()->SetSocketTransform(SocketName, MotionControllerComp->GetComponentTransform());
		FTransform HandTransform = MotionControllerComp->GetComponentTransform();

		AttachHandTo(InPaddle->GetPaddleMesh(), SocketName);
		InPaddle->bHeld = true;
	}
}

void APlayerMotionController::Release(APaddleActor * InPaddle)
{
	if (bHolding)
	{
		bHolding = false;
	}
	else
	{
		return;
	}
	DetachHand();
}
