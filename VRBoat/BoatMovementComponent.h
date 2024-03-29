// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VRBoat.h"

#include "GameFramework/PawnMovementComponent.h"

#include "PaddleActor.h"
#include "RiverSplineActor.h"

#include "BoatMovementComponent.generated.h"

UCLASS(config = Game)
class VRBOAT_API UBoatMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UBoatMovementComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual float SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult &Hit, bool bHandleImpact = false) override;

	FVector GetRiverFlow() const;
	ARiverSplineActor* GetRiverSpline() const { return RiverSplineActor; }

	UPROPERTY()
	APaddleActor * Paddle = nullptr;

	FVector AngularVelocity;

	bool bStopMovement = false;

protected:

	FVector LastRawVelocity = FVector::ZeroVector;
	float RotationSpeedCached = 0.f;

	float VelocityLerpAlpha = 0.f;

	UPROPERTY()
	ARiverSplineActor * RiverSplineActor = nullptr;
};