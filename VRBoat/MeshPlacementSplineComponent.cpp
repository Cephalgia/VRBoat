
#include "MeshPlacementSplineComponent.h"

#include "MeshPlacementActor.h"

UMeshPlacementSplineComponent::UMeshPlacementSplineComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

#if WITH_EDITOR
void UMeshPlacementSplineComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (AMeshPlacementActor * MeshPlacementActor = Cast<AMeshPlacementActor>(GetOwner()))
	{
		MeshPlacementActor->UpdateMeshComp();
	}
}
#endif