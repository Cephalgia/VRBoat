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

	UPROPERTY()
	APlayerMotionController * LeftController = nullptr;

	UPROPERTY()
	APlayerMotionController * RightController = nullptr;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ReceivePaddleDelta(FVector DeltaMove, float DeltaTime);

	void OnPossessed();

protected:

	UPROPERTY(EditAnywhere)
	UBoatCollisionComponent * BoatCollision = nullptr;

	UPROPERTY(EditAnywhere)
	USceneComponent * VROrigin = nullptr;

	UPROPERTY(EditAnywhere)
	UCameraComponent * VRCamera = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent * BoatMesh = nullptr;

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

	UPROPERTY()
	UAudioComponent* AudioComponentRight = nullptr;

	UPROPERTY()
	UAudioComponent* AudioComponentLeft = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* SoundRight = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* SoundLeft = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<APlayerMotionController> MotionControllerClass;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<APaddleActor> PaddleClass;

	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "WalkingPawn"))
	UClass* WalkingPawnClass = nullptr;

	FVector Velocity = FVector::ZeroVector;
	FVector LastRawVelocity = FVector::ZeroVector;
	float RotationSpeedCached = 0.f;
	
	float VelocityLerpAlpha = 0.f;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void GripLeft() { LeftController->Grab(Paddle); } //not just paddle!
	void GripRight() { RightController->Grab(Paddle); }
	void ReleaseLeft() { LeftController->Release(Paddle); }
	void ReleaseRight() { RightController->Release(Paddle); }

	void BoatExit();

	void PlayRiverSound(FVector InFlow);

	void PositionReset();
};