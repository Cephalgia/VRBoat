// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BoatPawn.h"

#include "Engine/Engine.h"
#include "Engine/CollisionProfile.h"
#include "Components/StaticMeshComponent.h"
#include "AudioDevice.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "../../../HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "XRMotionControllerBase.h"

#include "VRBoatGameModeBase.h"
#include "BoatMovementComponent.h"
#include "RiverSplineActor.h"
#include "BoatExitPoint.h"
#include "WalkingPawn.h"

#include "DrawDebugHelpers.h"

ABoatPawn::ABoatPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.TickInterval = 0.7f;

	BoatCollision = CreateDefaultSubobject<UBoatCollisionComponent>(TEXT("Boat Collision"));
	BoatCollision->SetupAttachment(GetRootComponent());

	BoatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Boat Mesh"));
	BoatMesh->SetupAttachment(BoatCollision);

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VR Origin"));
	VROrigin->SetupAttachment(BoatMesh, "Camera");

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	VRCamera->SetupAttachment(VROrigin);

	BoatMovementComp = CreateDefaultSubobject<UBoatMovementComponent>(TEXT("BoatMovementComp"));

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Post Process Component"));
	PostProcessComponent->SetupAttachment(GetRootComponent());
}

void ABoatPawn::BeginPlay()
{
	Super::BeginPlay();

	if (MaterialInterface)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(MaterialInterface, this);
		DynamicMaterial->SetScalarParameterValue(TEXT("BlinkerRadius"), 1.f);
		PostProcessComponent->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.f, DynamicMaterial));
	}

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);

	FTransform SpawnTransform(FVector(0.f, 0.f, 0.f));

	UClass* PaddleLoaded = PaddleClass.LoadSynchronous();
	Paddle = Cast<APaddleActor>(GetWorld()->SpawnActor(PaddleLoaded, &SpawnTransform));	

	if (Paddle)
	{
		UClass * MCClass = MotionControllerClass.LoadSynchronous();
		LeftController = Cast<APlayerMotionController>(GetWorld()->SpawnActor(MCClass, &SpawnTransform)); // find or spawn
		if (LeftController)
		{
			LeftController->SetMotionSource(FXRMotionControllerBase::LeftHandSourceId);
			LeftController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false));
		}

		RightController = Cast<APlayerMotionController>(GetWorld()->SpawnActor(MCClass, &SpawnTransform));
		if (RightController)
		{
			RightController->SetMotionSource(FXRMotionControllerBase::RightHandSourceId);
			RightController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false));
		}

		Paddle->LeftController = LeftController;
		Paddle->RightController = RightController;

		BoatMovementComp->Paddle = Paddle;
		Paddle->DefaultParentComponent = BoatMesh;
		Paddle->AttachToDefaultComp();
	}


	FVector End1 = FVector(300.f, 0.f, 20.f);
	FVector End2 = FVector(-300.f, 0.f, 20.f);

	AudioComponentRight = UGameplayStatics::SpawnSoundAttached(SoundRight, BoatMesh, FName(NAME_None), End1, EAttachLocation::KeepRelativeOffset);
	AudioComponentLeft = UGameplayStatics::SpawnSoundAttached(SoundLeft, BoatMesh, FName(NAME_None), End2, EAttachLocation::KeepRelativeOffset);
}

void ABoatPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugSphere(GetWorld(), AudioComponentRight->GetComponentLocation(), 5.f, 8, FColor::Red, false, 0.7f);
	DrawDebugSphere(GetWorld(), AudioComponentLeft->GetComponentLocation(), 5.f, 8, FColor::Red, false, 0.7f);
}

void ABoatPawn::ReceivePaddleDelta(FVector DeltaMove, float DeltaTime)
{
	Velocity = -(DeltaMove*0.5f) / DeltaTime;
}

void ABoatPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("GrabLeft"), IE_Pressed, this, &ABoatPawn::GripLeft);
	PlayerInputComponent->BindAction(TEXT("GrabRight"), IE_Pressed, this, &ABoatPawn::GripRight);
	PlayerInputComponent->BindAction(TEXT("GrabLeft"), IE_Released, this, &ABoatPawn::ReleaseLeft);
	PlayerInputComponent->BindAction(TEXT("GrabRight"), IE_Released, this, &ABoatPawn::ReleaseRight);

	PlayerInputComponent->BindAction(TEXT("BoatExit"), IE_Pressed, this, &ABoatPawn::BoatExit);

	PlayerInputComponent->BindAction(TEXT("PositionReset"), IE_Pressed, this, &ABoatPawn::PositionReset);
}

void ABoatPawn::BoatExit()
{
	if (AVRBoatGameModeBase * GameMode = Cast<AVRBoatGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		ABoatExitPoint * Point = nullptr;
		for (auto ExitPoint : GameMode->ExitPoints)
		{
			if ((GetActorLocation() - ExitPoint->GetActorLocation()).SizeSquared() < FMath::Square(300.f))
			{
				Point = ExitPoint;
			}
		}

		if (Point)
		{
			if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(this))
			{
				FNavLocation Result;
				if (NavSys->ProjectPointToNavigation(Point->GetActorLocation(), Result, FVector(50.f, 50.f, 50.f)))
				{
					AWalkingPawn * WalkingPawn = GetWorld()->SpawnActor<AWalkingPawn>(WalkingPawnClass, FTransform(Result.Location));
					GetController()->Possess(WalkingPawn);
				}
			}
		}
	}
}

void ABoatPawn::PositionReset()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}