// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VRBoatGameModeBase.generated.h"

class ABoatExitPoint;
/**
 * 
 */
UCLASS()
class VRBOAT_API AVRBoatGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<ABoatExitPoint*> ExitPoints;
};
