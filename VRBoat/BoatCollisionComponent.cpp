
#include "BoatCollisionComponent.h"


UBoatCollisionComponent::UBoatCollisionComponent()
{
	
}

void UBoatCollisionComponent::UpdateConvexHull()
{
	FKConvexElem * ConvexElement = ShapeBodySetup->AggGeom.ConvexElems.GetData();
	ConvexElement->VertexData.Empty();

	float CrossSectionVertexNum = Vertices.Num() > 0 ? Vertices[0].Points.Num() : 0;

	//Check vertex array for validity
	bool VerticesValid = true;
	for (int i = 0; i < Vertices.Num(); ++i)
	{
		if (Vertices[i].Points.Num() != CrossSectionVertexNum)
		{
			VerticesValid = false;
		}
	}

	if (Vertices.Num() > 1 && CrossSectionVertexNum > 3 && VerticesValid)
	{
		for (int i = 0; i < CrossSectionVertexNum; ++i)
		{
			TArray<TArray<FVector>> VertexColumns;
			for (int j = 0; j < Vertices.Num(); ++j)
			{
				TArray<FVector> LevelVertices;
				LevelVertices.Add(Vertices[j].Points[i]);
				LevelVertices.Add(i == CrossSectionVertexNum - 1 ? Vertices[j].Points[0] : Vertices[j].Points[i + 1]);

				ConvexElement->VertexData.Add(LevelVertices[0]);
				ConvexElement->VertexData.Add(LevelVertices[1]);

				VertexColumns.Add(LevelVertices);
			}

			for (int k = 0; k < VertexColumns.Num() - 1; ++k)
			{
				ConvexElement->VertexData.Add(VertexColumns[k][0]);
				ConvexElement->VertexData.Add(VertexColumns[k + 1][0]);
			}
		}
	}
}

FPrimitiveSceneProxy* UBoatCollisionComponent::CreateSceneProxy()
{
	class FDrawСonvexSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FDrawСonvexSceneProxy(UBoatCollisionComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
		{
			bWillEverBeLit = false;

			if (InComponent->GetBodySetup() != nullptr)
			{
				FKConvexElem * ConvexElement = InComponent->GetBodySetup()->AggGeom.ConvexElems.GetData();
				if (ConvexElement)
				{
					Vertices = ConvexElement->VertexData;
				}
			}
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_GetDynamicMeshElements_DrawDynamicElements);


			const FTransform LocalToWorld(GetLocalToWorld());

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					const FLinearColor DrawColor(FColor::Red);

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
					for (int i = 0; i < Vertices.Num(); i += 2)
					{
						FVector Start = LocalToWorld.GetLocation() + LocalToWorld.TransformVector(Vertices[i]);
						FVector End = LocalToWorld.GetLocation() + LocalToWorld.TransformVector(Vertices[i + 1]);
						PDI->DrawLine(Start, End, DrawColor, GetDepthPriorityGroup(View));
					}
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bProxyVisible = IsSelected();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = IsShown(View) && bProxyVisible;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}
		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:
		TArray<FVector> Vertices;
	};

	return new FDrawСonvexSceneProxy(this);
}

FBoxSphereBounds UBoatCollisionComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	if (ShapeBodySetup)
	{
		if (FKConvexElem * ConvexElement = ShapeBodySetup->AggGeom.ConvexElems.GetData())
		{
			return FBoxSphereBounds(ConvexElement->CalcAABB(GetComponentToWorld(), GetComponentToWorld().GetScale3D()));
		}
	}

	return FBoxSphereBounds();
}

void UBoatCollisionComponent::UpdateBodySetup()
{
	CreateConvexShapeBodySetupIfNeeded();
	UpdateConvexHull();
}

void UBoatCollisionComponent::CreateConvexShapeBodySetupIfNeeded()
{
	if (ShapeBodySetup == nullptr || ShapeBodySetup->IsPendingKill())
	{
		ShapeBodySetup = NewObject<UBodySetup>(this, NAME_None, RF_Transient);
		if (GUObjectArray.IsDisregardForGC(this))
		{
			ShapeBodySetup->AddToRoot();
		}

		// If this component is in GC cluster, make sure we add the body setup to it to
		ShapeBodySetup->AddToCluster(this);
		// if we got created outside of game thread, but got added to a cluster, 
		// we no longer need the Async flag
		if (ShapeBodySetup->HasAnyInternalFlags(EInternalObjectFlags::Async) && GUObjectClusters.GetObjectCluster(ShapeBodySetup))
		{
			ShapeBodySetup->ClearInternalFlags(EInternalObjectFlags::Async);
		}

		ShapeBodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
		ShapeBodySetup->AggGeom.ConvexElems.Add(FKConvexElem());
		ShapeBodySetup->bNeverNeedsCookedCollisionData = false;
		bUseArchetypeBodySetup = false;	//We're making our own body setup, so don't use the archetype's.

		//Update bodyinstance and shapes
		BodyInstance.BodySetup = ShapeBodySetup;
		{
			if (BodyInstance.IsValidBodyInstance())
			{
				FPhysicsCommand::ExecuteWrite(BodyInstance.GetActorReferenceWithWelding(), [this](const FPhysicsActorHandle& Actor)
				{
					TArray<FPhysicsShapeHandle> Shapes;
					BodyInstance.GetAllShapes_AssumesLocked(Shapes);

					for (FPhysicsShapeHandle& Shape : Shapes)	//The reason we iterate is we may have multiple scenes and thus multiple shapes, but they are all pointing to the same geometry
					{
						//Update shape with the new body setup. Make sure to only update shapes owned by this body instance
						if (BodyInstance.IsShapeBoundToBody(Shape))
						{
							FPhysicsInterface::SetUserData(Shape, (void*)ShapeBodySetup->AggGeom.ConvexElems[0].GetUserData());
						}
					}
				});
			}
		}
	}
}

#if WITH_EDITOR
void UBoatCollisionComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateBodySetup();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR