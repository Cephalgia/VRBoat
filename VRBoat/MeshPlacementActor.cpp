
#include "MeshPlacementActor.h"

#include "DrawDebugHelpers.h"
AMeshPlacementActor::AMeshPlacementActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComp->SetupAttachment(GetRootComponent());

	SplineComp = CreateDefaultSubobject<UMeshPlacementSplineComponent>(TEXT("Spline Component"));
	SplineComp->SetupAttachment(GetRootComponent());
	
}

#if WITH_EDITOR
void AMeshPlacementActor::UpdateMeshComp()
{
	MeshComp->ClearInstances();

	FVector OrientationVector;
	switch (SplineComp->PlacementAxis)
	{
	case 0 :
		OrientationVector = FVector(1.f, 0.f, 0.f);
		break;
	case 1:
		OrientationVector = FVector(0.f, 1.f, 0.f);
		break;
	default:
		OrientationVector = FVector(0.f, 0.f, 1.f);
		break;
	}

	TArray< UMeshPlacementSplineComponent*> Splines;
	GetComponents<UMeshPlacementSplineComponent>(Splines);

	for (auto Spline : Splines)
	{
		for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 1; ++i)
		{
			FVector PointLocation = Spline->GetRelativeTransform().TransformPosition(Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local));

			FVector NextPointLocation = Spline->GetRelativeTransform().TransformPosition(Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local));
			FVector ToNext = NextPointLocation - PointLocation;
			FRotator PointRotation = FRotator::ZeroRotator;
			if (Spline->PlacementAxis == 0)
			{
				PointRotation = ToNext.ToOrientationRotator();
			}
			else if (Spline->PlacementAxis == 1)
			{
				PointRotation = ToNext.ToOrientationRotator().Quaternion().GetRightVector().ToOrientationRotator();
			}

			if (MeshComp->GetStaticMesh())
			{
				MeshComp->AddInstance(FTransform(PointRotation, PointLocation, FVector::OneVector));
			}
		}
	}
}
#endif