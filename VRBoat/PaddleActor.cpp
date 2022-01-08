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

	PaddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Paddle Mesh"));
	PaddleMesh->SetupAttachment(RootComp);
}

void APaddleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!RightController || !LeftController)
	{
		return;
	}

	FVector RightLocation = RightController->GetControllerLocation();
	FVector LeftLocation = LeftController->GetControllerLocation();

	//hand rotations apply only to Y axis

	// find middle point between hands
	FVector MiddlePoint = (RightLocation + LeftLocation) / 2;
	// find rotation
	FVector HandsVector = (RightLocation - LeftLocation).GetSafeNormal();
	FRotator FinalRotator = HandsVector.ToOrientationRotator();
	
	FinalRotator.Roll = FMath::UnwindDegrees(-RightController->GetControllerYRotation());

	if (FinalRotator.Roll < 1.f)
	{
		DrawDebugSphere(GetWorld(), MiddlePoint, 5.f, 8, FColor::Red);
	}

	SetActorLocationAndRotation(MiddlePoint, FinalRotator.Quaternion());

	DeltaMove = FVector::ZeroVector;
	// The rotation should matter too
	FVector RightPaddleLocation = PaddleMesh->GetSocketLocation("PaddleWater_R");
	FVector LeftPaddleLocation = PaddleMesh->GetSocketLocation("PaddleWater_L");

	if (RightPaddleLocation.Z < 0.f || LeftPaddleLocation.Z < 0.f)
	{
		bool bPaddleRight = RightPaddleLocation.Z < 0.f;
		if (bPaddleRight != bPrevPaddleRight)
		{
			bPrevPaddleRight = bPaddleRight;
			PaddleLocationPrev = FVector::ZeroVector;
			return;
		}
		FVector RelevantPaddleLocation = RightPaddleLocation.Z < 0.f ? RightPaddleLocation : LeftPaddleLocation;
		if (PaddleLocationPrev != FVector::ZeroVector)
		{
			DeltaMove = RelevantPaddleLocation - PaddleLocationPrev;
			DeltaMove.Z = 0;
			DeltaMove = -(DeltaMove*0.7f)/* / DeltaTime*/; //0.7 is sort of water resistance
		}

		bPrevPaddleRight = bPaddleRight;
		PaddleLocationPrev = RelevantPaddleLocation;
	}
	else
	{
		PaddleLocationPrev = FVector::ZeroVector;
	}
}

UStaticMeshComponent* APaddleActor::GetPaddleMesh() const
{
	return PaddleMesh;
}