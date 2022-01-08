#pragma once

#include "VRBoat.h"

#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

#include "PlayerMotionController.h"

#include "PaddleActor.generated.h"

UCLASS(config = Game)
class VRBOAT_API APaddleActor : public AActor
{
	GENERATED_BODY()

public:
	APaddleActor();

	virtual void Tick(float DeltaTime) override;

	FVector GetInputVelocity() const { return DeltaMove; }
	FVector GetRelevantPaddleLocation() const { return PaddleLocationPrev; }

	UStaticMeshComponent* GetPaddleMesh() const;

	UPROPERTY()
	APlayerMotionController * LeftController = nullptr;

	UPROPERTY()
	APlayerMotionController * RightController = nullptr;

protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent * PaddleMesh = nullptr;

	UPROPERTY(EditAnywhere)
	USceneComponent * RootComp = nullptr;

	FVector PaddleLocationPrev = FVector::ZeroVector;
	bool bPrevPaddleRight = false;
	FVector DeltaMove = FVector::ZeroVector;
};