// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VRBoat.h"

#include "GameFramework/Actor.h"
#include "MotionControllerComponent.h"

#include "PlayerHandActor.h"

#include "PlayerMotionController.generated.h"

UCLASS(config = Game)
class VRBOAT_API APlayerMotionController : public AActor
{
	GENERATED_BODY()

public:
	APlayerMotionController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool HasLocalNetOwner() const override { return true; };

	void SetMotionSource(FName SourceName);

	FVector GetControllerLocation() const;
	float GetControllerYRotation() const;

	void AttachHandTo(USceneComponent * InComponent, FName InSocketName);

protected:
	UPROPERTY(EditAnywhere)
	USceneComponent * VROrigin = nullptr;

	UPROPERTY(EditAnywhere)
	UMotionControllerComponent* MotionControllerComp = nullptr;

	UPROPERTY()
	APlayerHandActor * PlayerHandActor = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<APlayerHandActor> RightHandActorClass;
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<APlayerHandActor> LeftHandActorClass;
};