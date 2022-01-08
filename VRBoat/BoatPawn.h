#pragma once

#include "VRBoat.h"

#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"

#include "PlayerMotionController.h"
#include "PaddleActor.h"
#include "BoatMovementComponent.h"
#include "BoatCollisionComponent.h"

#include "BoatPawn.generated.h"

UCLASS(config = Game)
class VRBOAT_API ABoatPawn : public APawn
{
	GENERATED_BODY()

public:
	ABoatPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ReceivePaddleDelta(FVector DeltaMove, float DeltaTime);

protected:

	UPROPERTY(EditAnywhere)
	UBoatCollisionComponent * VROrigin = nullptr;

	UPROPERTY(EditAnywhere)
	UCameraComponent * VRCamera = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent * BoatMesh = nullptr;

	UPROPERTY()
	APlayerMotionController * LeftController = nullptr;

	UPROPERTY()
	APlayerMotionController * RightController = nullptr;

	UPROPERTY()
	APaddleActor * Paddle = nullptr;

	UPROPERTY()
	UBoatMovementComponent * BoatMovementComp = nullptr;

	UPROPERTY(EditAnywhere)
	UPostProcessComponent * PostProcessComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface * MaterialInterface = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic * DynamicMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	UCurveFloat * SpeedBlinkerCurve = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<APlayerMotionController> MotionControllerClass;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<APaddleActor> PaddleClass;

	FVector Velocity = FVector::ZeroVector;
	FVector LastRawVelocity = FVector::ZeroVector;
	float RotationSpeedCached = 0.f;
	
	float VelocityLerpAlpha = 0.f;
};