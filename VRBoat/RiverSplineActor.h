#pragma once

#include "VRBoat.h"

#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"

#include "RiverSplineActor.generated.h"

USTRUCT()
struct FSplineSection
{
	GENERATED_BODY()

	FSplineSection() {}
	FSplineSection(USplineComponent* InSpline) { Spline = InSpline; }

	UPROPERTY()
	USplineComponent* Spline = nullptr;

	UPROPERTY()
	TArray<USplineComponent*> StartAdjacent;
	UPROPERTY()
	TArray<USplineComponent*> EndAdjacent;
};

UCLASS(config = Game)
class VRBOAT_API ARiverSplineActor : public AActor
{
	GENERATED_BODY()

public:
	ARiverSplineActor();

	virtual void BeginPlay() override;

	FVector GetRiverFlow(FVector InLocation) const;

	FVector GetClosestPoint(FVector InLocation) const;

protected:
	UPROPERTY(Transient)
	TArray<FSplineSection> RiverSplines;

	UPROPERTY(EditAnywhere)
	UCurveFloat * DistanceToFlowCurve = nullptr;

	int32 CurrentSplineIndex = -1;

	USplineComponent* GetClosestSpline(FVector InLocation, float& OutDistance) const;
};