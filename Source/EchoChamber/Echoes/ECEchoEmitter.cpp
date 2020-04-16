// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "ECEchoEmitter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystemComponent.h"


AECEchoEmitter::AECEchoEmitter()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SphereComponent;

	PrimaryActorTick.bCanEverTick = true;
}

void AECEchoEmitter::BeginPlay()
{
	Super::BeginPlay();

	for (int SegmentIdx = 0; SegmentIdx < NumTraces; SegmentIdx++)
	{
		Segments[SegmentIdx].BouncesRemaining = MaxNumBounces;
		Segments[SegmentIdx].DistanceRemaining = MaxTraceDistance;
		Segments[SegmentIdx].Speed = TraceSpeed;
		Segments[SegmentIdx].Origin = GetActorLocation();
		Segments[SegmentIdx].Direction = UKismetMathLibrary::RotateAngleAxis(GetActorForwardVector(), DeltaAngle * (float)SegmentIdx, GetActorUpVector());
		Segments[SegmentIdx].Direction.Normalize();
		Segments[SegmentIdx].Done = false;

		FRotator rotation = UKismetMathLibrary::MakeRotationFromAxes(Segments[SegmentIdx].Direction, FVector::CrossProduct(Segments[SegmentIdx].Direction, GetActorUpVector()), GetActorUpVector());
		Segments[SegmentIdx].ParticleActor = GetWorld()->SpawnActor(TravellingParticleActor.GetDefaultObject()->GetClass(), &Segments[SegmentIdx].Origin, &rotation);
	}
}

void AECEchoEmitter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	FCollisionResponseParams CollisionParams = FCollisionResponseParams::DefaultResponseParam;
	FHitResult hitResult;
	bool bTraceRemaining = false;

	for (int SegmentIdx = 0; SegmentIdx < NumTraces; SegmentIdx++)
	{		
		if (Segments[SegmentIdx].Done)
		{
			continue;
		}

		bTraceRemaining = true;


		float fDistanceToTravel = Segments[SegmentIdx].Speed * DeltaTime;
		FVector endPoint = Segments[SegmentIdx].Origin + Segments[SegmentIdx].Direction * fDistanceToTravel;
		GetWorld()->LineTraceSingleByChannel(hitResult, Segments[SegmentIdx].Origin, endPoint, ECollisionChannel::ECC_Visibility, Params, CollisionParams);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		//if (hitResult.bBlockingHit)
		//{
		//	DrawDebugLine(GetWorld(), Segments[SegmentIdx].Origin, endPoint, FColor::Red, false, 0.3f);
		//}
#endif

		if (hitResult.bBlockingHit)
		{
			//FRotator rotation = UKismetMathLibrary::MakeRotationFromAxes(GetActorUpVector() * -1.0f, FVector::CrossProduct(GetActorUpVector() * -1.0f, hitResult.ImpactNormal), hitResult.ImpactNormal);
			//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, (FVector)hitResult.ImpactPoint, rotation);

			if (hitResult.Actor.IsValid())
			{
				OnWallActorHit(hitResult.Actor.Get());
				//AActor* HitActor = hitResult.Actor.Get();
				
				/*if (AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(HitActor))
				{
					UMaterialInstanceDynamic* MI = UMaterialInstanceDynamic::Create(StaticMeshActor->GetStaticMeshComponent()->GetMaterial(0), this);
					StaticMeshActor->GetStaticMeshComponent()->SetMaterial(0, MI);
					MI->SetScalarParameterValue(TEXT("TeamColorIndex"), testfloat);
				}*/
			}
		}

		if (hitResult.bBlockingHit)
		{
			Segments[SegmentIdx].Origin = hitResult.ImpactPoint;
			Segments[SegmentIdx].Direction = FMath::GetReflectionVector(Segments[SegmentIdx].Direction, hitResult.ImpactNormal);
			Segments[SegmentIdx].DistanceRemaining -= hitResult.Distance;
			Segments[SegmentIdx].BouncesRemaining--;			
		}
		else
		{
			Segments[SegmentIdx].Origin = endPoint;
			Segments[SegmentIdx].DistanceRemaining -= fDistanceToTravel;
		}

		Segments[SegmentIdx].ParticleActor->SetActorLocation(Segments[SegmentIdx].Origin);

		if (Segments[SegmentIdx].BouncesRemaining < 0
			|| Segments[SegmentIdx].DistanceRemaining <= 0.0f)
		{
			Segments[SegmentIdx].ParticleActor->Destroy();
			Segments[SegmentIdx].Done = true;
		}
	}

	if (!bTraceRemaining)
	{
		Destroy();
	}
}

void AECEchoEmitter::OnWallActorHit_Implementation(AActor* ActorHit)
{

}