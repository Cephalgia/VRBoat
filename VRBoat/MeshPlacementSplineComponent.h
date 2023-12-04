
#pragma once

#include "VRBoat.h"

#include "Components/SplineComponent.h"

#include "MeshPlacementSplineComponent.generated.h"

class UStaticMesh;

UCLASS(config = Game, meta = (BlueprintSpawnableComponent))
class VRBOAT_API UMeshPlacementSplineComponent : public USplineComponent
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMesh* MeshToPlace = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	uint8 PlacementAxis;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	float Spacing;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};