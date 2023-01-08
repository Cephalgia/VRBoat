// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BoatMovementComponent.h"

#include "BoatPawn.h"

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

	float OldZ = UpdatedComponent->GetComponentLocation().Z;

	FVector BoatVector = PawnOwner->GetActorForwardVector().GetSafeNormal2D();
	bool bPaddle = false;

	FVector Flow = GetRiverFlow();

	//DrawDebugLine(GetWorld(), PawnOwner->GetActorLocation() + FVector(0.f, 0.f, 50.f), PawnOwner->GetActorLocation() + (Flow * 50.f) + FVector(0.f, 0.f, 50.f), FColor::Red, false, -1.f, 0, 1.5f);
	//movement
	FVector Acceleration = Paddle->GetInputVelocity() * 1.5f;
	if (Acceleration != FVector::ZeroVector)
	{
		bPaddle = true;
	}
	//if (Acceleration != FVector::ZeroVector/* || Velocity != FVector::ZeroVector*/)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("=================="));
	//	UE_LOG(LogTemp, Warning, TEXT("DeltaTime %f"), DeltaTime);
	//	UE_LOG(LogTemp, Warning, TEXT("Acceleration %s"), *Acceleration.ToString());
	//}
	FVector NewVelocity = Velocity + Acceleration;

	//if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
	//	UE_LOG(LogTemp, Warning, TEXT("New Velocity %s, Velocity %s"), *NewVelocity.ToString(), *Velocity.ToString());
	float WaterResistance = 0.f;// 1.1f - FVector::DotProduct(Acceleration.GetSafeNormal2D(), BoatVector.GetSafeNormal2D()); // 0.1f-1.1f range
	float NewSize = NewVelocity.Size2D() - WaterResistance;
	NewSize = NewSize > 0.f ? NewSize : 0.f;
	//if (Acceleration != FVector::ZeroVector/* || Velocity != FVector::ZeroVector*/)
	//	UE_LOG(LogTemp, Warning, TEXT("New Velocity Size with Resistance %f"), NewSize);	
	NewVelocity = NewVelocity/*.GetSafeNormal2D() * NewSize*/ + Flow;
	//if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
	//	UE_LOG(LogTemp, Warning, TEXT("New Velocity Size with Flow %f"), NewVelocity.Size());

	//if (Acceleration != FVector::ZeroVector /*|| Velocity != FVector::ZeroVector*/)
	//	UE_LOG(LogTemp, Warning, TEXT("------------"));
	//rotation

	FVector ToPaddle = bPaddle ? Paddle->GetRelevantPaddleLocation() - UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
	ToPaddle.Z = 0.f;
	//if (Acceleration != FVector::ZeroVector /*|| Velocity != FVector::ZeroVector*/)
	//	UE_LOG(LogTemp, Warning, TEXT("ToPaddle %s"), *ToPaddle.ToString());
	FVector Torque = FVector::CrossProduct(ToPaddle, Acceleration * 0.3f);
	//if (Acceleration != FVector::ZeroVector/* || Velocity != FVector::ZeroVector*/)
	//	UE_LOG(LogTemp, Warning, TEXT("Torque %s"), *Torque.ToString());
	float Inertia = 50.f + Velocity.Size2D() * 0.005f;
	FVector NewAngularVelocity = AngularVelocity + (Torque) / Inertia;
	WaterResistance = 1.f;
	float NewAngSize = FMath::Abs(NewAngularVelocity.Z) - WaterResistance;
	NewAngSize = NewAngSize > 0.f ? NewAngSize : 0.f;
	NewAngularVelocity.Z = NewAngularVelocity.Z > 0.f ? NewAngSize : -NewAngSize;
	//if (Acceleration != FVector::ZeroVector /*|| Velocity != FVector::ZeroVector*/)
	//	UE_LOG(LogTemp, Warning, TEXT("NewAngularVelocity %s, AngularVelocity %s"), *NewAngularVelocity.ToString(), *AngularVelocity.ToString());

	if (NewVelocity.SizeSquared2D() >= Flow.SizeSquared2D())
	{
		float AccelBoatCos = FVector::DotProduct(NewVelocity.GetSafeNormal2D(), BoatVector.GetSafeNormal2D());
		float AccelBoatSin = FMath::Sqrt(1 - AccelBoatCos * AccelBoatCos);
		/*float*/ NewSize = NewVelocity.Size2D() - AccelBoatSin - 0.1f;
		NewSize = NewSize > 0.f ? NewSize : 0.f;
		NewVelocity = NewVelocity.GetSafeNormal2D() * NewSize;
		/*if (Acceleration != FVector::ZeroVector || Velocity != FVector::ZeroVector)
			UE_LOG(LogTemp, Warning, TEXT("New Velocity Size with resistance %f"), NewVelocity.Size());*/
	}

	Velocity = NewVelocity;
	AngularVelocity = NewAngularVelocity;
	//if (Acceleration != FVector::ZeroVector /*|| Velocity != FVector::ZeroVector*/)
	//	UE_LOG(LogTemp, Warning, TEXT("=================="));
	
	FVector DeltaMove = Velocity * DeltaTime;
	float Sign = AngularVelocity.Z > 0.f ? 1.f : -1.f;
	FRotator NewRotation = FRotator(0.f, PawnOwner->GetActorRotation().Yaw + (Sign * AngularVelocity.Size() * DeltaTime), 0.f);

	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(DeltaMove, NewRotation, true, Hit);
	if (Hit.bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 3.f, 8, FColor::Green, false, 1.f);
		//UE_LOG(LogTemp, Warning, TEXT("Hit Actor %s"), *Hit.Actor->GetName());
	}
	SlideAlongSurface(DeltaMove, (1.f - Hit.Time), Hit.Normal, Hit, true);

	Velocity = Velocity - Flow;

	/*if (UpdatedComponent->GetComponentLocation().Z - OldZ != 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Z CHANGED"));
	}*/
}
float UBoatMovementComponent::SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact)
{
	if (!Hit.bBlockingHit)
	{
		return 0.f;
	}

	float PercentTimeApplied = 0.f;
	const FVector OldHitNormal = Normal;

	FVector SlideDelta = ComputeSlideVector(Delta, Time, Normal, Hit);

	if ((SlideDelta | Delta) > 0.f)
	{
		const FQuat Rotation = UpdatedComponent->GetComponentQuat();
		SlideDelta.Z = 0.f;
		SafeMoveUpdatedComponent(SlideDelta, Rotation, true, Hit);

		const float FirstHitPercent = Hit.Time;
		PercentTimeApplied = FirstHitPercent;
		if (Hit.IsValidBlockingHit())
		{
			// Notify first impact
			if (bHandleImpact)
			{
				HandleImpact(Hit, FirstHitPercent * Time, SlideDelta);
			}

			// Compute new slide normal when hitting multiple surfaces.
			TwoWallAdjust(SlideDelta, Hit, OldHitNormal);

			// Only proceed if the new direction is of significant length and not in reverse of original attempted move.
			if (!SlideDelta.IsNearlyZero(1e-3f) && (SlideDelta | Delta) > 0.f)
			{
				// Perform second move
				SlideDelta.Z = 0.f;
				SafeMoveUpdatedComponent(SlideDelta, Rotation, true, Hit);
				const float SecondHitPercent = Hit.Time * (1.f - FirstHitPercent);
				PercentTimeApplied += SecondHitPercent;

				// Notify second impact
				if (bHandleImpact && Hit.bBlockingHit)
				{
					HandleImpact(Hit, SecondHitPercent * Time, SlideDelta);
				}
			}
		}

		return FMath::Clamp(PercentTimeApplied, 0.f, 1.f);
	}

	return 0.f;
}
