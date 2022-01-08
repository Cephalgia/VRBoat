#pragma once

#include "VRBoat.h"

#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

#include "PlayerHandActor.generated.h"

UCLASS(config = Game)
class VRBOAT_API APlayerHandActor : public AActor
{
	GENERATED_BODY()

public:
	APlayerHandActor();

	UPROPERTY(BlueprintReadOnly, Category = "HandSettings")
	bool bIsLeft = false;

	void MakeLeftHand();

protected:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent * HandMesh = nullptr;
	UPROPERTY()
	USceneComponent * Origin = nullptr;

};