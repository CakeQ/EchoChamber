// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "ECEchoEmitter.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


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
		{
			DrawDebugLine(GetWorld(), Segments[SegmentIdx].Origin, endPoint, FColor::Red, false, 0.3f);
		}
#endif

		//if (hitResult.bBlockingHit)
		//{
		//	FRotator rotation = UKismetMathLibrary::MakeRotationFromAxes(GetActorUpVector() * -1.0f, FVector::CrossProduct(GetActorUpVector() * -1.0f, hitResult.ImpactNormal), hitResult.ImpactNormal);
		//	UParticleSystemComponent* SpawnedComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, (FVector)hitResult.ImpactPoint, rotation);
		//}

		if (hitResult.bBlockingHit)
		{
			Segments[SegmentIdx].Origin = hitResult.ImpactPoint;
			Segments[SegmentIdx].Direction = FMath::GetReflectionVector(Segments[SegmentIdx].Direction, hitResult.ImpactNormal);
			Segments[SegmentIdx].DistanceRemaining -= hitResult.Distance;
			Segments[SegmentIdx].BouncesRemaining--;

			FRotator rotation = UKismetMathLibrary::MakeRotationFromAxes(GetActorUpVector() * -1.0f, FVector::CrossProduct(GetActorUpVector() * -1.0f, hitResult.ImpactNormal), hitResult.ImpactNormal);
			UParticleSystemComponent* SpawnedComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, (FVector)hitResult.ImpactPoint, rotation);
		}
		else
		{
			Segments[SegmentIdx].Origin = endPoint;
			Segments[SegmentIdx].DistanceRemaining -= fDistanceToTravel;
		}

		if (Segments[SegmentIdx].BouncesRemaining < 0
			|| Segments[SegmentIdx].DistanceRemaining <= 0.0f)
		{
			Segments[SegmentIdx].Done = true;
		}
	}

	if (!bTraceRemaining)
	{
		Destroy();
	}
}