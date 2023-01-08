#pragma once

#include "VRBoat.h"

#include "GameFramework/Actor.h"

#include "BoatExitPoint.generated.h"

UCLASS(config = Game)
class VRBOAT_API ABoatExitPoint : public AActor
{
	GENERATED_BODY()

public:
	ABoatExitPoint();

	virtual void BeginPlay() override;
};