// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "WalkingPawn.h"

#include "Engine/Engine.h"
#include "Engine/CollisionProfile.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "../../../HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "XRMotionControllerBase.h"
#include "IXRTrackingSystem.h"

#include "BoatPawn.h"
#include "VRBoatGameModeBase.h"
#include "BoatExitPoint.h"

#include "DrawDebugHelpers.h"

AWalkingPawn::AWalkingPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VR Origin"));
	VROrigin->SetupAttachment(Capsule);

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	VRCamera->SetupAttachment(VROrigin);

	TeleportIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Teleport Indicator"));
	TeleportIndicator->SetupAttachment(Capsule);
	TeleportIndicator->SetHiddenInGame(true);
}

void AWalkingPawn::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
}

void AWalkingPawn::OnPossessed()
{
	if (AVRBoatGameModeBase * GameMode = Cast<AVRBoatGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		if (ABoatPawn * BoatPawn = GameMode->BoatPawn)
		{
			LeftController = BoatPawn->LeftController;
			RightController = BoatPawn->RightController;

			LeftController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false));
			RightController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false));
		}
	}
	PositionReset();
	SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, Capsule->GetScaledCapsuleHalfHeight()));
}

void AWalkingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bTeleporting)
	{
		CachedTeleportLocation = FTransform::Identity;
		FHitResult HitResult;
		//DrawDebugLine(GetWorld(), RightController->GetControllerLocation(), RightController->GetControllerLocation() + RightController->GetControllerForwardVector() * 1000.f, FColor::Red, false, 1.f);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, RightController->GetControllerLocation(), RightController->GetControllerLocation() + RightController->GetControllerForwardVector() * 1000.f, ECollisionChannel::ECC_Visibility))
		{
			//DrawDebugSphere(GetWorld(), HitResult.Location, 10.f, 8, FColor::Red, false, 1.f);
			if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(this))
			{
				FNavLocation Result;
				if (NavSys->ProjectPointToNavigation(HitResult.Location, Result, FVector(50.f, 50.f, 50.f)))
				{
					TeleportIndicator->SetHiddenInGame(false);
					TeleportIndicator->SetWorldLocationAndRotation(Result.Location, FRotator(0.f, RightController->GetControllerRotation().Yaw, 0.f).Quaternion());
					DrawDebugLine(GetWorld(), RightController->GetControllerLocation(), Result.Location, FColor::Red);
					DrawDebugSphere(GetWorld(), Result.Location, 10.f, 8, FColor::Red);
					CachedTeleportLocation = TeleportIndicator->GetComponentTransform();
					CachedTeleportLocation.SetLocation(CachedTeleportLocation.GetLocation() + FVector(0.f, 0.f, Capsule->GetScaledCapsuleHalfHeight()));
				}
			}
			else
			{
				TeleportIndicator->SetHiddenInGame(true);
			}
		}
		else
		{
			TeleportIndicator->SetHiddenInGame(true);
		}
	}
}

void AWalkingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Teleport"), IE_Pressed, this, &AWalkingPawn::PreTeleport);
	PlayerInputComponent->BindAction(TEXT("Teleport"), IE_Released, this, &AWalkingPawn::PostTeleport);
	PlayerInputComponent->BindAction(TEXT("CancelTeleport"), IE_Released, this, &AWalkingPawn::CancelTeleport);

	PlayerInputComponent->BindAction(TEXT("BoatExit"), IE_Pressed, this, &AWalkingPawn::BoatEnter);

	PlayerInputComponent->BindAction(TEXT("PositionReset"), IE_Pressed, this, &AWalkingPawn::PositionReset);
}

void AWalkingPawn::PreTeleport()
{
	bTeleporting = true;
}

void AWalkingPawn::PostTeleport()
{
	bTeleporting = false;
	if (!CachedTeleportLocation.Equals(FTransform::Identity))
	{
		TeleportIndicator->SetHiddenInGame(true);
		TeleportIndicator->SetRelativeLocation(FVector::ZeroVector);

		CachedTeleportLocation.SetRotation(FRotator(0.f, CachedTeleportLocation.Rotator().Yaw - VRCamera->GetRelativeRotation().Yaw, 0.f).Quaternion());

		SetActorTransform(CachedTeleportLocation);
		CachedTeleportLocation = FTransform::Identity;
	}
}

void AWalkingPawn::CancelTeleport()
{
	bTeleporting = false;
	if (!CachedTeleportLocation.Equals(FTransform::Identity))
	{
		CachedTeleportLocation = FTransform::Identity;

		TeleportIndicator->SetHiddenInGame(true);
		TeleportIndicator->SetRelativeLocation(FVector::ZeroVector);
	}
}

void AWalkingPawn::BoatEnter()
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
			GetController()->Possess(GameMode->BoatPawn);
			GameMode->BoatPawn->OnPossessed();
		}
	}
}

void AWalkingPawn::PositionReset()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}