// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BoatMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UBoatMovementComponent::UBoatMovementComponent()
{
	
}

void UBoatMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(this, ARiverSplineActor::StaticClass());
	RiverSplineActor = Cast<ARiverSplineActor>(FoundActor);
}

FVector UBoatMovementComponent::GetRiverFlow() const
{
	return RiverSplineActor->GetRiverFlow(UpdatedComponent->GetComponentLocation());
}

void UBoatMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector BoatVector = PawnOwner->GetActorForwardVector().GetSafeNormal2D();
	bool bPaddle = false;

	FVector Flow = GetRiverFlow();
	//DrawDebugLine(GetWorld(), PawnOwner->GetActorLocation() + FVector(0.f, 0.f, 50.f), PawnOwner->GetActorLocation() + (Flow * 50.f) + FVector(0.f, 0.f, 50.f), FColor::Red, false, -1.f, 0, 1.5f);
	//movement
	FVector Acceleration = Paddle->GetInputVelocity() * 100.f;
	if (Acceleration != FVector::ZeroVector)
	{
		bPaddle = true;
	}
	if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Warning, TEXT("=================="));
		UE_LOG(LogTemp, Warning, TEXT("DeltaTime %f"), DeltaTime);
		UE_LOG(LogTemp, Warning, TEXT("Acceleration %s"), *Acceleration.ToString());
	}
	FVector NewVelocity = Velocity + Acceleration * DeltaTime;

	if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("New Velocity %s, Velocity %s"), *NewVelocity.ToString(), *Velocity.ToString());
	float WaterResistance = 1.1f - FVector::DotProduct(Acceleration.GetSafeNormal2D(), BoatVector.GetSafeNormal2D()); // 0.1f-1.1f range
	float NewSize = NewVelocity.Size2D() - WaterResistance;
	NewSize = NewSize > 0.f ? NewSize : 0.f;
	if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("New Velocity Size with Resistance %f"), NewSize);	
	NewVelocity = NewVelocity.GetSafeNormal2D() * NewSize + Flow;
	if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("New Velocity Size with Flow %f"), NewVelocity.Size());

	if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("------------"));
	//rotation
	FVector ToPaddle = bPaddle ? Paddle->GetRelevantPaddleLocation() - PawnOwner->GetActorLocation() : FVector::ZeroVector;
	ToPaddle.Z = 0.f;
	if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("ToPaddle %s"), *ToPaddle.ToString());
	FVector Torque = FVector::CrossProduct(ToPaddle, Acceleration * 0.1f);
	if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("Torque %s"), *Torque.ToString());
	float Inertia = 50.f + Velocity.Size2D() * 0.05f;
	FVector NewAngularVelocity = AngularVelocity + Torque / Inertia * DeltaTime;
	WaterResistance = 1.f;
	float NewAngSize = FMath::Abs(NewAngularVelocity.Z) - WaterResistance;
	NewAngSize = NewAngSize > 0.f ? NewAngSize : 0.f;
	NewAngularVelocity.Z = NewAngularVelocity.Z > 0.f ? NewAngSize : -NewAngSize;
	if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("NewAngularVelocity %s, AngularVelocity %s"), *NewAngularVelocity.ToString(), *AngularVelocity.ToString());

	Velocity = NewVelocity;
	AngularVelocity = NewAngularVelocity;
	if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("=================="));
	
	FVector DeltaMove = Velocity * DeltaTime;
	float Sign = AngularVelocity.Z > 0.f ? 1.f : -1.f;
	FRotator NewRotation = FRotator(0.f, PawnOwner->GetActorRotation().Yaw + (Sign * AngularVelocity.Size() * DeltaTime), 0.f);

	FHitResult Hit(1.f);
	MoveUpdatedComponent(DeltaMove, NewRotation, true, &Hit);
	SlideAlongSurface(DeltaMove, (1.f - Hit.Time), Hit.Normal, Hit, true);

	Velocity = Velocity - Flow;

	//float SpeedDecrease = 66.67 * DeltaTime;
	//FVector ResidualVelocity = Velocity.GetSafeNormal2D() * (Velocity.Size() - SpeedDecrease);

	//FVector BoatVector = PawnOwner->GetActorForwardVector().GetSafeNormal2D();

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
	//}
	//FVector DeltaMove = Velocity * DeltaTime;

	//float VelocitySize = Velocity.Size2D();

	//FRotator NewRotation = PawnOwner->GetActorRotation();

	//float Diff = FMath::FindDeltaAngleDegrees(PawnOwner->GetActorRotation().Yaw, LastRawVelocity.ToOrientationRotator().Yaw);
	//if (FMath::Abs(Diff) > 0.5f)
	//{
	//	/*float AngleFadeSpeed = FMath::Abs(Diff) * 66.67f / Velocity.Size();
	//	float AngleSpeedDecrease = AngleFadeSpeed * DeltaTime;*/

	//	float RotationSpeed = 4.f; // 4 degrees per second
	//	if (!bPaddle)
	//	{
	//		RotationSpeed = RotationSpeedCached - DeltaTime * 2.f;
	//		RotationSpeed = FMath::Clamp(RotationSpeed, 0.f, FLT_MAX);
	//	}
	//	RotationSpeedCached = RotationSpeed;
	//	float TotalTime = Diff / RotationSpeed;
	//	float Speed = FMath::Abs(TotalTime / DeltaTime);
	//	float CurrentAngleDelta = Diff / Speed;
	//	UE_LOG(LogTemp, Warning, TEXT("actor %f, velocity %f, diff %f, Delta %f, VelSize %f"), PawnOwner->GetActorRotation().Yaw, LastRawVelocity.ToOrientationRotator().Yaw, Diff, CurrentAngleDelta, VelocitySize);
	//	//DrawDebugSphere(GetWorld(), GetActorLocation() + GetActorForwardVector() * 50.f + FVector(0.f, 0.f, 20.f), 30.f, 8, FColor::Magenta);

	//	NewRotation = FRotator(0.f, PawnOwner->GetActorRotation().Yaw + CurrentAngleDelta, 0.f);
	//}

	//FHitResult Hit(1.f);
	//MoveUpdatedComponent(DeltaMove, NewRotation, true, &Hit);
	//SlideAlongSurface(DeltaMove, (1.f - Hit.Time), Hit.Normal, Hit, true);
}