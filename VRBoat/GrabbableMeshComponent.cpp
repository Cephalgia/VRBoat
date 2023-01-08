
#include "GrabbableMeshComponent.h"

#include "Engine/StaticMeshSocket.h"

#include "DrawDebugHelpers.h"


UGrabbableMeshComponent::UGrabbableMeshComponent()
{
	
}

FTransform UGrabbableMeshComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const
{
	// should work normally for marker sockets that define the line along which the sockets can move
	if (InSocketName.ToString().Contains("MS_"))
	{
		UStaticMeshSocket const* const Socket = GetSocketByName(InSocketName);
		if (Socket)
		{
			FTransform SocketWorldTransform;
			if (Socket->GetSocketTransform(SocketWorldTransform, this))
			{
				switch (TransformSpace)
				{
					case RTS_World:
					{
						return SocketWorldTransform;
					}
					case RTS_Actor:
					{
						if (const AActor* Actor = GetOwner())
						{
							return SocketWorldTransform.GetRelativeTransform(GetOwner()->GetTransform());
						}
						break;
					}
					case RTS_Component:
					{
						return SocketWorldTransform.GetRelativeTransform(GetComponentTransform());
					}
				}
			}
		}
	}
	else if (InSocketName != NAME_None)
	{
		const FTransform* SocketTransform = SocketNewTransforms.Find(InSocketName);
		if (SocketTransform)
		{
			switch (TransformSpace)
			{
				case RTS_World:
				{
					return *SocketTransform * GetComponentTransform();
				}
				case RTS_Actor:
				{
					if (const AActor* Actor = GetOwner())
					{
						return (*SocketTransform * GetComponentTransform()).GetRelativeTransform(GetOwner()->GetTransform());
					}
					break;
				}
				case RTS_Component:
				{
					return *SocketTransform;
				}
			}
		}		
	}

	return Super::GetSocketTransform(InSocketName, TransformSpace);
}

void UGrabbableMeshComponent::SetSocketTransform(FName InSocketName, FTransform InWorldTransform)
{
	TArray<FName> Sockets = GetAllSocketNames();
	TArray<FTransform> MarkerSockets;
	int32 Counter = 0;

	for (FName SocketName : Sockets)
	{
		if (SocketName.ToString().Contains("MS_"))
		{
			MarkerSockets.Add(GetSocketTransform(SocketName, RTS_World));
			Counter++;
		}

		if (Counter == 2)
		{
			break;
		}
	}
	
	if (MarkerSockets.Num() == 2)
	{
		FVector TubePoint = FMath::ClosestPointOnSegment(InWorldTransform.GetLocation(), MarkerSockets[0].GetLocation(), MarkerSockets[1].GetLocation());
		//DrawDebugLine(GetWorld(), MarkerSockets[0].GetLocation(), MarkerSockets[1].GetLocation(), FColor::Red, false, 10.f);
		//DrawDebugSphere(GetWorld(), InWorldTransform.GetLocation(), 10.f, 8, FColor::Red, false, 10.f);
		//DrawDebugSphere(GetWorld(), TubePoint, 10.f, 8, FColor::Blue, false, 10.f);
		FTransform SocketTransform = FTransform(MarkerSockets[0].GetRotation(), TubePoint, MarkerSockets[0].GetScale3D());
		SocketNewTransforms.Add(InSocketName, SocketTransform.GetRelativeTransform(GetComponentTransform()));
	}
}

FVector UGrabbableMeshComponent::GetHandsMidPoint() const
{
	FVector LeftHand = GetSocketTransform("Handle_L", RTS_World).GetLocation();
	FVector RightHand = GetSocketTransform("Handle_R", RTS_World).GetLocation();

	FVector MidPoint = (LeftHand + RightHand) / 2.f;

	return MidPoint;
}