#pragma once

#include "VRBoat.h"

#include "GameFramework/Actor.h"
#include "PlayerMotionController.h"
#include "Sound/SoundWave.h"

#include "GrabbableMeshComponent.h"

#include "PaddleActor.generated.h"

UCLASS(config = Game)
class VRBOAT_API APaddleActor : public AActor
{
	GENERATED_BODY()

public:
	APaddleActor();

	virtual void Tick(float DeltaTime) override;

	FVector GetInputVelocity() const 
	{ 
		if (PaddleMesh->GetAttachChildren().Num() == 3)
			return DeltaMove; 
		return FVector::ZeroVector;
	}
	FVector GetRelevantPaddleLocation() const { return PaddleLocationPrev; }

	void AttachPaddleMesh(USceneComponent * InComponent, FName InSocketName);
	void DetachPaddleMesh();
	void AttachToDefaultComp();

	UGrabbableMeshComponent* GetPaddleMesh() const { return PaddleMesh; }

	UPROPERTY()
	APlayerMotionController * LeftController = nullptr;

	UPROPERTY()
	APlayerMotionController * RightController = nullptr;

	UPROPERTY()
	USceneComponent * DefaultParentComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TArray<USoundWave *> PaddleSounds;

	bool bHeld = false;

protected:
	UPROPERTY(EditAnywhere)
	UGrabbableMeshComponent * PaddleMesh = nullptr;

	UPROPERTY(EditAnywhere)
	USceneComponent * RootComp = nullptr;

	FVector PaddleLocationPrev = FVector::ZeroVector;
	bool bPrevPaddleRight = false;
	FVector DeltaMove = FVector::ZeroVector;

	FVector ToPaddle = FVector::ZeroVector;

	float CurrentRotationLerpAlpha = 1.f;
};