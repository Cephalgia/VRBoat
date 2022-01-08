// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VRBoat.h"

#include "Components/ShapeComponent.h"

#include "BoatCollisionComponent.generated.h"

USTRUCT(Blueprintable)
struct FPointArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (MakeEditWidget = true))
	TArray<FVector> Points;
};

UCLASS(config = Game)
class VRBOAT_API UBoatCollisionComponent : public UShapeComponent
{
	GENERATED_BODY()

public:
	UBoatCollisionComponent();

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface
	
	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End USceneComponent Interface

	virtual void UpdateBodySetup() override;

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	//~ End UObject Interface.

protected:
	void UpdateConvexHull();
	void CreateConvexShapeBodySetupIfNeeded();

	UPROPERTY(EditAnywhere, Category = "Settings")
	TArray<FPointArray> Vertices;

};