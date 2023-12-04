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

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UBillboardComponent	* BillboardComponent = nullptr;
	UPROPERTY()
	UBillboardComponent	* BoatBillboardComponent = nullptr;
#endif
	UPROPERTY(EditAnywhere)
	USceneComponent * BoatStayPoint = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent * BoatPole = nullptr;
};