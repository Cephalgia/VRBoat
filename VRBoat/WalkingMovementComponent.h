// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VRBoat.h"

#include "GameFramework/PawnMovementComponent.h"

#include "WalkingMovementComponent.generated.h"

UCLASS(config = Game)
class VRBOAT_API UWalkingMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UWalkingMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

};