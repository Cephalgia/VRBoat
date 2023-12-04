
#include "MeshPlacementSplineComponentVisualizer.h"

#include "VRBoat/MeshPlacementSplineComponent.h"
#include "Engine/StaticMesh.h"

FMeshPlacementSplineComponentVisualizer::FMeshPlacementSplineComponentVisualizer()
	: FSplineComponentVisualizer()
{	
}

bool FMeshPlacementSplineComponentVisualizer::HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale)
{
	ResetTempModes();

	UMeshPlacementSplineComponent* SplineComp = Cast<UMeshPlacementSplineComponent>(GetEditedSplineComponent());
	if (SplineComp != nullptr)
	{
		if (IsAnySelectedKeyIndexOutOfRange(SplineComp))
		{
			// Something external has changed the number of spline points, meaning that the cached selected keys are no longer valid
			EndEditing();
			return false;
		}

		float MeshDimension = 0.f;
		bool bChanged = false;
		switch (SplineComp->PlacementAxis)
		{
		case 0:
			MeshDimension = SplineComp->MeshToPlace->GetBoundingBox().GetExtent().X * 2.f;
			break;
		case 1:
			MeshDimension = SplineComp->MeshToPlace->GetBoundingBox().GetExtent().Y * 2.f;
			break;
		default:
			MeshDimension = SplineComp->MeshToPlace->GetBoundingBox().GetExtent().Z * 2.f;
		}

		DeltaTranslate = DeltaTranslate.GetSafeNormal() * (MeshDimension + SplineComp->Spacing);

		check(SelectionState);
		if (SelectionState->GetSelectedTangentHandle() != INDEX_NONE)
		{
			return TransformSelectedTangent(DeltaTranslate);
		}
		else if (ViewportClient->IsAltPressed())
		{
			if (ViewportClient->GetWidgetMode() == FWidget::WM_Translate && ViewportClient->GetCurrentWidgetAxis() != EAxisList::None && SelectionState->GetSelectedKeys().Num() == 1)
			{
				static const int MaxDuplicationDelay = 3;

				FVector Drag = DeltaTranslate;

				if (bAllowDuplication)
				{
					if (DuplicateDelay < MaxDuplicationDelay)
					{
						DuplicateDelay++;
						DuplicateDelayAccumulatedDrag += DeltaTranslate;
					}
					else
					{
						//Drag += DuplicateDelayAccumulatedDrag;
						DuplicateDelayAccumulatedDrag = FVector::ZeroVector;

						

						bAllowDuplication = false;
						bDuplicatingSplineKey = true;

						DuplicateKeyForAltDrag(Drag);
					}
				}
				else
				{
					UpdateDuplicateKeyForAltDrag(Drag);
				}

				return true;
			}
		}
		else
		{
			return TransformSelectedKeys(DeltaTranslate, DeltaRotate, DeltaScale);
		}
	}

	return false;
}
//
//bool FMeshPlacementSplineComponentVisualizer::TransformSelectedKeys(const FVector& DeltaTranslate, const FRotator& DeltaRotate, const FVector& DeltaScale)
//{
//	UMeshPlacementSplineComponent* SplineComp = Cast<UMeshPlacementSplineComponent>(GetEditedSplineComponent());
//	if (SplineComp != nullptr)
//	{
//		FInterpCurveVector& SplinePosition = SplineComp->GetSplinePointsPosition();
//		FInterpCurveQuat& SplineRotation = SplineComp->GetSplinePointsRotation();
//		FInterpCurveVector& SplineScale = SplineComp->GetSplinePointsScale();
//
//		const int32 NumPoints = SplinePosition.Points.Num();
//
//		check(SelectionState);
//		const TSet<int32>& SelectedKeys = SelectionState->GetSelectedKeys();
//		int32 LastKeyIndexSelected = SelectionState->GetVerifiedLastKeyIndexSelected(NumPoints);
//		check(SelectedKeys.Num() > 0);
//		check(SelectedKeys.Contains(LastKeyIndexSelected));
//
//		SplineComp->Modify();
//
//		for (int32 SelectedKeyIndex : SelectedKeys)
//		{
//			check(SelectedKeyIndex >= 0);
//			check(SelectedKeyIndex < NumPoints);
//
//			FInterpCurvePoint<FVector>& EditedPoint = SplinePosition.Points[SelectedKeyIndex];
//			FInterpCurvePoint<FQuat>& EditedRotPoint = SplineRotation.Points[SelectedKeyIndex];
//			FInterpCurvePoint<FVector>& EditedScalePoint = SplineScale.Points[SelectedKeyIndex];
//
//
//			if (!DeltaTranslate.IsZero())
//			{
//				// Find key position in world space
//				const FVector CurrentWorldPos = SplineComp->GetComponentTransform().TransformPosition(EditedPoint.OutVal);
//				// Move in world space
//				const FVector NewWorldPos = CurrentWorldPos + DeltaTranslate;
//
//				// Convert back to local space
//				EditedPoint.OutVal = SplineComp->GetComponentTransform().InverseTransformPosition(NewWorldPos);
//			}
//
//			if (!DeltaRotate.IsZero())
//			{
//				// Set point tangent as user controlled
//				EditedPoint.InterpMode = CIM_CurveUser;
//
//				// Rotate tangent according to delta rotation
//				FVector NewTangent = SplineComp->GetComponentTransform().GetRotation().RotateVector(EditedPoint.LeaveTangent); // convert local-space tangent vector to world-space
//				NewTangent = DeltaRotate.RotateVector(NewTangent); // apply world-space delta rotation to world-space tangent
//				NewTangent = SplineComp->GetComponentTransform().GetRotation().Inverse().RotateVector(NewTangent); // convert world-space tangent vector back into local-space
//				EditedPoint.LeaveTangent = NewTangent;
//				EditedPoint.ArriveTangent = NewTangent;
//
//				// Rotate spline rotation according to delta rotation
//				FQuat NewRot = SplineComp->GetComponentTransform().GetRotation() * EditedRotPoint.OutVal; // convert local-space rotation to world-space
//				NewRot = DeltaRotate.Quaternion() * NewRot; // apply world-space rotation
//				NewRot = SplineComp->GetComponentTransform().GetRotation().Inverse() * NewRot; // convert world-space rotation to local-space
//				EditedRotPoint.OutVal = NewRot;
//			}
//
//			if (DeltaScale.X != 0.0f)
//			{
//				// Set point tangent as user controlled
//				EditedPoint.InterpMode = CIM_CurveUser;
//
//				const FVector NewTangent = EditedPoint.LeaveTangent * (1.0f + DeltaScale.X);
//				EditedPoint.LeaveTangent = NewTangent;
//				EditedPoint.ArriveTangent = NewTangent;
//			}
//
//			if (DeltaScale.Y != 0.0f)
//			{
//				// Scale in Y adjusts the scale spline
//				EditedScalePoint.OutVal.Y *= (1.0f + DeltaScale.Y);
//			}
//
//			if (DeltaScale.Z != 0.0f)
//			{
//				// Scale in Z adjusts the scale spline
//				EditedScalePoint.OutVal.Z *= (1.0f + DeltaScale.Z);
//			}
//		}
//
//		SplineComp->UpdateSpline();
//		SplineComp->bSplineHasBeenEdited = true;
//
//		NotifyPropertyModified(SplineComp, SplineCurvesProperty);
//
//		if (!DeltaRotate.IsZero())
//		{
//			SelectionState->Modify();
//			SelectionState->SetCachedRotation(SplineComp->GetQuaternionAtSplinePoint(LastKeyIndexSelected, ESplineCoordinateSpace::World));
//		}
//
//		GEditor->RedrawLevelEditingViewports(true);
//
//		return true;
//	}
//
//	return false;
//}