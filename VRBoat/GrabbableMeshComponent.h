// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VRBoat.h"

#include "Components/StaticMeshComponent.h"

#include "GrabbableMeshComponent.generated.h"

UCLASS(config = Game)
class VRBOAT_API UGrabbableMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UGrabbableMeshComponent();

	virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) const override;

	void SetSocketTransform(FName InSocketName, FTransform InWorldTransform);

	FVector GetHandsMidPoint() const;

protected:
	TMap<FName, FTransform> SocketNewTransforms;

};