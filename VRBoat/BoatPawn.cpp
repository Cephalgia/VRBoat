// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BoatPawn.h"

#include "Engine/Engine.h"
#include "Engine/CollisionProfile.h"
#include "Components/StaticMeshComponent.h"

#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "../../../HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "XRMotionControllerBase.h"

#include "BoatMovementComponent.h"

#include "DrawDebugHelpers.h"

ABoatPawn::ABoatPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	VROrigin = CreateDefaultSubobject<UBoatCollisionComponent>(TEXT("VR Origin"));
	VROrigin->SetupAttachment(GetRootComponent());

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	VRCamera->SetupAttachment(VROrigin);

	BoatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Boat Mesh"));
	BoatMesh->SetupAttachment(GetRootComponent());

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
	VROrigin->SetRelativeLocation(VROrigin->GetRelativeLocation() + FVector(0.f, 0.f, 75.f));
	BoatMesh->SetRelativeLocation(BoatMesh->GetRelativeLocation() + FVector(0.f, 0.f, -75.f));

	FTransform SpawnTransform(FVector(0.f, 0.f, 0.f));

	UClass* PaddleLoaded = PaddleClass.LoadSynchronous();
	Paddle = Cast<APaddleActor>(GetWorld()->SpawnActor(PaddleLoaded, &SpawnTransform));	

	if (Paddle)
	{
		UClass * MCClass = MotionControllerClass.LoadSynchronous();
		LeftController = Cast<APlayerMotionController>(GetWorld()->SpawnActor(MCClass, &SpawnTransform));
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

}

void ABoatPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
}
