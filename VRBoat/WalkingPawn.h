#pragma once

#include "VRBoat.h"

#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

#include "PlayerMotionController.h"

#include "WalkingPawn.generated.h"

UCLASS(config = Game)
class VRBOAT_API AWalkingPawn : public APawn
{
	GENERATED_BODY()

public:
	AWalkingPawn();

	UPROPERTY()
	APlayerMotionController * LeftController = nullptr;

	UPROPERTY()
	APlayerMotionController * RightController = nullptr;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void OnPossessed();

protected:

	UPROPERTY(EditAnywhere)
	UCapsuleComponent * Capsule = nullptr;

	UPROPERTY(EditAnywhere)
	USceneComponent * VROrigin = nullptr;

	UPROPERTY(EditAnywhere)
	UCameraComponent * VRCamera = nullptr; 
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent * TeleportIndicator = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* SoundRight = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* SoundLeft = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<APlayerMotionController> MotionControllerClass;

	FTransform CachedTeleportLocation = FTransform::Identity;
	bool bTeleporting = false;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void PreTeleport();
	void PostTeleport();
	void CancelTeleport();

	void BoatEnter();

	void PositionReset();
};