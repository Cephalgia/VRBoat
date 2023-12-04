
#pragma once

#include "SplineComponentVisualizer.h"

/** SplineComponent visualizer/edit functionality */
class VRBOATEDITOR_API FMeshPlacementSplineComponentVisualizer : public FSplineComponentVisualizer
{
public:
	FMeshPlacementSplineComponentVisualizer();

	virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale) override;

	//bool TransformSelectedKeys(const FVector& DeltaTranslate, const FRotator& DeltaRotate, const FVector& DeltaScale);
	
};
