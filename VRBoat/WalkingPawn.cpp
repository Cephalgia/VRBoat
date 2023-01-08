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

#include "BoatPawn.h"

#include "DrawDebugHelpers.h"

AWalkingPawn::AWalkingPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	VROrigin = CreateDefaultSubobject<UCapsuleComponent>(TEXT("VR Origin"));
	VROrigin->SetupAttachment(GetRootComponent());

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	VRCamera->SetupAttachment(VROrigin);

	TeleportIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Teleport Indicator"));
	TeleportIndicator->SetupAttachment(VROrigin);
	TeleportIndicator->SetHiddenInGame(true);
}

void AWalkingPawn::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	FTransform SpawnTransform(FVector(0.f, 0.f, 0.f));

	TArray<AActor*> Boat;
	UGameplayStatics::GetAllActorsOfClass(this, ABoatPawn::StaticClass(), Boat);
	if (Boat.Num() > 0)
	{
		if (ABoatPawn * BoatPawn = Cast<ABoatPawn>(Boat[0]))
		{
			LeftController = BoatPawn->LeftController;
			RightController = BoatPawn->RightController;

			LeftController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false));
			RightController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false));
		}
	}
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
					CachedTeleportLocation = TeleportIndicator->GetComponentTransform();
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