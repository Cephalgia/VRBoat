// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PaddleActor.h"

#include "BoatPawn.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

APaddleActor::APaddleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
	RootComp->SetupAttachment(RootComponent);

	PaddleMesh = CreateDefaultSubobject<UGrabbableMeshComponent>(TEXT("Paddle Mesh"));
	PaddleMesh->SetupAttachment(RootComp);
}

void APaddleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bHeld)
	{
		ToPaddle = FVector::ZeroVector;
		return;
	}

	if (!RightController || !LeftController)
	{
		return;
	}
	if (PaddleMesh->GetAttachChildren().Num() == 2)
	{
		FVector HandLocation = FVector::ZeroVector;
		if (RightController->IsHolding())
		{
			HandLocation = RightController->GetControllerLocation();
		}
		else if (LeftController->IsHolding())
		{
			HandLocation = LeftController->GetControllerLocation();
		}
		
		if (ToPaddle == FVector::ZeroVector)
		{
			if (HandLocation != FVector::ZeroVector)
			{
				ToPaddle = GetActorLocation() - HandLocation;
			}
		}

		FVector NewLocation = HandLocation + ToPaddle;
		SetActorLocation(NewLocation);
		return;
	}
	else if (PaddleMesh->GetAttachChildren().Num() == 1)
	{
		bHeld = false;
		AttachToDefaultComp();
		PaddleMesh->SetRelativeRotation(FRotator(30.f, 0.f, 0.f));
		return;
	}
	if (PaddleMesh->GetRelativeRotation() == FRotator::ZeroRotator)
	{
		PaddleMesh->SetRelativeRotation(FRotator(30.f, -90.f, 0.f));
		
	}
	ToPaddle = FVector::ZeroVector;

	FVector RightLocation = RightController->GetControllerLocation();
	FVector LeftLocation = LeftController->GetControllerLocation();

	FVector HandsMidPoint = PaddleMesh->GetHandsMidPoint();
	FVector ToHands = HandsMidPoint - GetActorLocation();
	//hand rotations apply only to Y axis
	// find middle point between hands
	FVector MiddlePoint = (RightLocation + LeftLocation) / 2.f /*- ToHands*/;
	// find rotation
	FVector HandsVector = (RightLocation - LeftLocation).GetSafeNormal();
	FRotator FinalRotator = HandsVector.ToOrientationRotator();
	
	FinalRotator.Roll = FMath::UnwindDegrees(-RightController->GetControllerYRotation());

	/*if (FinalRotator.Roll < 1.f)
	{
		DrawDebugSphere(GetWorld(), MiddlePoint, 5.f, 8, FColor::Red);
	}*/

	// The rotation should matter too
	FVector RightPaddleLocation = PaddleMesh->GetSocketLocation("PaddleWater_R");
	FVector LeftPaddleLocation = PaddleMesh->GetSocketLocation("PaddleWater_L");

	if (RightPaddleLocation.Z < 0.f || LeftPaddleLocation.Z < 0.f)
	{
		CurrentRotationLerpAlpha = 0.05f;
	}
	else
	{
		CurrentRotationLerpAlpha = FMath::Lerp(CurrentRotationLerpAlpha, 1.f, 0.05f);
	}

	FRotator PrevRotation = GetActorRotation();
	FinalRotator = FMath::Lerp(PrevRotation, FinalRotator, CurrentRotationLerpAlpha);

	SetActorLocationAndRotation(MiddlePoint, FinalRotator.Quaternion());

	DeltaMove = FVector::ZeroVector;

	if (RightPaddleLocation.Z < 0.f || LeftPaddleLocation.Z < 0.f) // what if they both are
	{
		bool bPaddleRight = RightPaddleLocation.Z < 0.f;
		if (bPaddleRight != bPrevPaddleRight)
		{
			bPrevPaddleRight = bPaddleRight;
			PaddleLocationPrev = FVector::ZeroVector;
			return;
		}

		if (PaddleLocationPrev == FVector::ZeroVector)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PaddleSounds[FMath::RandRange(0, PaddleSounds.Num() - 1)], bPaddleRight ? RightPaddleLocation : LeftPaddleLocation);
		}

		FVector RelevantPaddleLocation = RightPaddleLocation.Z < 0.f ? RightPaddleLocation : LeftPaddleLocation;
		if (PaddleLocationPrev != FVector::ZeroVector)
		{
			DeltaMove = RelevantPaddleLocation - PaddleLocationPrev;
			DeltaMove.Z = 0;
			DeltaMove = -(DeltaMove/**0.7f*/); //0.7 is sort of water resistance
		}

		bPrevPaddleRight = bPaddleRight;
		PaddleLocationPrev = RelevantPaddleLocation;
	}
	else
	{
		PaddleLocationPrev = FVector::ZeroVector;
	}
}

void APaddleActor::AttachPaddleMesh(USceneComponent * InComponent, FName InSocketName)
{
	PaddleMesh->AttachToComponent(InComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false), InSocketName);
}

void APaddleActor::DetachPaddleMesh()
{
	FTransform PaddleTransform = PaddleMesh->GetComponentTransform();
	PaddleMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	SetActorLocation(PaddleTransform.GetLocation());
	PaddleMesh->AttachToComponent(RootComp, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
}

void APaddleActor::AttachToDefaultComp()
{
	PaddleMesh->AttachToComponent(DefaultParentComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false), "Paddle");
	SetActorRotation(FRotator::ZeroRotator);
}