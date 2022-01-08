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

	/*BoatMesh->SetCollisionProfileName(UCollisionProfile::Vehicle_ProfileName);
	BoatMesh->BodyInstance.bSimulatePhysics = true;
	BoatMesh->SetCanEverAffectNavigation(false);*/
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
			LeftController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
			LeftController->AttachHandTo(Paddle->GetPaddleMesh(), "Handle_L");
		}

		RightController = Cast<APlayerMotionController>(GetWorld()->SpawnActor(MCClass, &SpawnTransform));
		if (RightController)
		{
			RightController->SetMotionSource(FXRMotionControllerBase::RightHandSourceId);
			RightController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
			RightController->AttachHandTo(Paddle->GetPaddleMesh(), "Handle_R");
		}

		//Paddle->AttachToComponent(BoatMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false), "Paddle");
		Paddle->LeftController = LeftController;
		Paddle->RightController = RightController;

		BoatMovementComp->Paddle = Paddle;
	}

}

void ABoatPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (DynamicMaterial && SpeedBlinkerCurve)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("BlinkerRadius"), SpeedBlinkerCurve->GetFloatValue(BoatMovementComp->Velocity.Size()));
	}*/

	//residual velocity
	//VelocityLerpAlpha += DeltaTime;
	//float TimeRemaining = Velocity.Size() / 66.67f; // 66.67 - interpolation speed
	//VelocityLerpAlpha = DeltaTime / TimeRemaining;
	//FVector ResidualVelocity = FMath::Lerp(Velocity, FVector::ZeroVector, VelocityLerpAlpha);

	//float SpeedDecrease = 66.67 * DeltaTime;
	//FVector ResidualVelocity = Velocity.GetSafeNormal2D() * (Velocity.Size() - SpeedDecrease);

	//FVector BoatVector = GetActorForwardVector().GetSafeNormal2D();

	//// paddle velocity
	//bool bPaddle = false;
	//FVector InputVelocity = Paddle->GetInputVelocity();
	//if (InputVelocity.SizeSquared() > Velocity.SizeSquared())
	//{		
	//	/*if (InputVelocity.SizeSquared2D() > FMath::Square(200.f))
	//	{
	//		InputVelocity = InputVelocity.GetSafeNormal2D() * 200.f;
	//	}*/
	//	Velocity = InputVelocity;
	//	bPaddle = true;
	//	//VelocityLerpAlpha = 0.f;
	//}
	//else
	//{
	//	Velocity = ResidualVelocity;
	//	/*if (Velocity == FVector::ZeroVector)
	//	{
	//		VelocityLerpAlpha = 0.f;
	//	}*/
	//}

	//if (bPaddle)
	//{
	//	LastRawVelocity = Velocity;
	//	Velocity = Velocity.ProjectOnToNormal(BoatVector);
	//	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
	//}
	//else
	//{
	//	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
	//}	

	//float VelocitySize = Velocity.Size2D();


	//float Diff = FMath::FindDeltaAngleDegrees(GetActorRotation().Yaw, LastRawVelocity.ToOrientationRotator().Yaw);
	//if (FMath::Abs(Diff) > 0.5f)
	//{
	//	/*float AngleFadeSpeed = FMath::Abs(Diff) * 66.67f / Velocity.Size();
	//	float AngleSpeedDecrease = AngleFadeSpeed * DeltaTime;*/

	//	float RotationSpeed = 4.f; // 4 degrees per second
	//	if (!bPaddle)
	//	{
	//		RotationSpeed = RotationSpeedCached - DeltaTime*2.f;
	//		RotationSpeed = FMath::Clamp(RotationSpeed, 0.f, FLT_MAX);
	//	}
	//	RotationSpeedCached = RotationSpeed;
	//	float TotalTime = Diff / RotationSpeed;
	//	float Speed = FMath::Abs(TotalTime / DeltaTime);
	//	float CurrentAngleDelta = Diff / Speed;
	//	UE_LOG(LogTemp, Warning, TEXT("actor %f, velocity %f, diff %f, Delta %f, VelSize %f"), GetActorRotation().Yaw, LastRawVelocity.ToOrientationRotator().Yaw, Diff, CurrentAngleDelta, VelocitySize);
	//	//DrawDebugSphere(GetWorld(), GetActorLocation() + GetActorForwardVector() * 50.f + FVector(0.f, 0.f, 20.f), 30.f, 8, FColor::Magenta);

	//	SetActorRotation(FRotator(0.f, GetActorRotation().Yaw + CurrentAngleDelta, 0.f));
	//}
}

void ABoatPawn::ReceivePaddleDelta(FVector DeltaMove, float DeltaTime)
{
	//SetActorLocation(GetActorLocation() - DeltaMove * 0.5f/*, DeltaMove.Rotation().Quaternion()*/);
	Velocity = -(DeltaMove*0.5f) / DeltaTime;
}
