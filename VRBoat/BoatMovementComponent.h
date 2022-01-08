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

	UPROPERTY()
	APaddleActor * Paddle = nullptr;

	FVector AngularVelocity;

protected:
	//FVector ToPaddle = FVector::ZeroVector;

	FVector LastRawVelocity = FVector::ZeroVector;
	float RotationSpeedCached = 0.f;

	float VelocityLerpAlpha = 0.f;

	UPROPERTY()
	ARiverSplineActor * RiverSplineActor = nullptr;

	FVector GetRiverFlow() const;
};