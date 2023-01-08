// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "RiverSplineActor.h"

#include "DrawDebugHelpers.h"
ARiverSplineActor::ARiverSplineActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARiverSplineActor::BeginPlay()
{
	Super::BeginPlay();

	auto IsPointClose = [](FVector P1, FVector P2) { return (P1 - P2).SizeSquared2D() < FMath::Square(20.f); };

	TArray<USplineComponent*> Splines;
	GetComponents<USplineComponent>(Splines);
	for (auto Spline : Splines)
	{
		FVector StartPoint = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector EndPoint = Spline->GetLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
		TArray<USplineComponent*> StartSplines;
		TArray<USplineComponent*> EndSplines;
		for (auto OtherSpline : Splines)
		{
			if (OtherSpline != Spline)
			{
				FVector OtherStartPoint = OtherSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
				FVector OtherEndPoint = OtherSpline->GetLocationAtSplinePoint(OtherSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
				
				if (IsPointClose(StartPoint, OtherStartPoint) || IsPointClose(StartPoint, OtherEndPoint))
				{
					StartSplines.Add(OtherSpline);
				}
				if (IsPointClose(EndPoint, OtherStartPoint) || IsPointClose(EndPoint, OtherEndPoint))
				{
					EndSplines.Add(OtherSpline);
				}
			}
		}
		int32 Index = RiverSplines.Add(FSplineSection(Spline));
		RiverSplines[Index].StartAdjacent = StartSplines;
		RiverSplines[Index].EndAdjacent = EndSplines;
	}
}

FVector ARiverSplineActor::GetRiverFlow(FVector InLocation) const
{
	float Distance = 0.f;
	USplineComponent * ClosestSpline = GetClosestSpline(InLocation, Distance);
	
	if (ClosestSpline)
	{
		return ClosestSpline->FindDirectionClosestToWorldLocation(InLocation, ESplineCoordinateSpace::World) * DistanceToFlowCurve->GetFloatValue(Distance);;
	}

	return FVector::ZeroVector;
}

FVector ARiverSplineActor::GetClosestPoint(FVector InLocation) const
{
	float Distance = 0.f;
	USplineComponent * ClosestSpline = GetClosestSpline(InLocation, Distance);

	if (ClosestSpline)
	{
		return ClosestSpline->FindLocationClosestToWorldLocation(InLocation, ESplineCoordinateSpace::World);
	}

	return FVector::ZeroVector;
}

USplineComponent* ARiverSplineActor::GetClosestSpline(FVector InLocation, float& OutDistance) const
{
	USplineComponent * ClosestSpline = nullptr;
	OutDistance = 0.f;
	FVector Point;
	if (CurrentSplineIndex == -1)
	{
		float MinCurveDistance = FLT_MAX;
		for (auto Spline : RiverSplines)
		{
			FVector ClosestPoint = Spline.Spline->FindLocationClosestToWorldLocation(InLocation, ESplineCoordinateSpace::World);
			float CurrentDistance = (ClosestPoint - InLocation).SizeSquared2D();
			if (CurrentDistance < MinCurveDistance)
			{
				MinCurveDistance = CurrentDistance;
				ClosestSpline = Spline.Spline;
				OutDistance = CurrentDistance;
				Point = ClosestPoint;
			}
		}
	}
	else
	{
		TArray<USplineComponent*> NearbySplines;
		NearbySplines.Add(RiverSplines[CurrentSplineIndex].Spline);
		NearbySplines.Append(RiverSplines[CurrentSplineIndex].StartAdjacent);
		NearbySplines.Append(RiverSplines[CurrentSplineIndex].EndAdjacent);
		float MinCurveDistance = FLT_MAX;
		for (auto Spline : NearbySplines)
		{
			FVector ClosestPoint = Spline->FindLocationClosestToWorldLocation(InLocation, ESplineCoordinateSpace::World);
			float CurrentDistance = (ClosestPoint - InLocation).SizeSquared2D();
			if (CurrentDistance < MinCurveDistance)
			{
				MinCurveDistance = CurrentDistance;
				ClosestSpline = Spline;
				OutDistance = CurrentDistance;
				Point = ClosestPoint;
			}
		}

	}

	return ClosestSpline;
}