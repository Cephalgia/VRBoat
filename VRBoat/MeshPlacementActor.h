#pragma once

#include "VRBoat.h"

#include "GameFramework/Actor.h"
#include "MeshPlacementSplineComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

#include "MeshPlacementActor.generated.h"

UCLASS(config = Game)
class VRBOAT_API AMeshPlacementActor : public AActor
{
	GENERATED_BODY()

public:
	AMeshPlacementActor();

	UPROPERTY(EditAnywhere, Category = Placement)
	UMeshPlacementSplineComponent * SplineComp = nullptr;

	UPROPERTY(EditAnywhere, Category = Placement)
	UHierarchicalInstancedStaticMeshComponent * MeshComp = nullptr;

#if WITH_EDITOR
	void UpdateMeshComp();
#endif
};