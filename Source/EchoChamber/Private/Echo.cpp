// Fill out your copyright notice in the Description page of Project Settings.


#include "Echo.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "Math/Color.h"
#include "Kismet/KismetMathLibrary.h"

AEcho::AEcho()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Propagation properties
	radius = 20.0f;
	speed = 5.0f;
	acceleration = 2.0f;

	// Visual appearance
	thickness = 5.0f;
	color = FColor(255, 255, 255, 255);
	segments = 360;

	bGoing = true;

	// Collider/overlap detector sphere
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->InitSphereRadius(radius);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapIgnorePawn"));
	CollisionSphere->bHiddenInGame = false;

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEcho::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AEcho::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEcho::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update the circle radius
	if (bGoing)
	{
		speed += acceleration;
		radius += (speed * DeltaTime);
		CollisionSphere->SetSphereRadius(radius);
	}

	DrawDebugCircle(
		GetWorld(),
		GetTransform().ToMatrixNoScale(),
		radius,
		segments,
		color,
		false,
		-1,
		(uint8)'\000',
		thickness,
		false
	);

	// DEBUG
	for (FVector pt : Collisions)
	{
		DrawDebugSphere(GetWorld(), pt, 20.0f, 20, FColor::Red);
	}
	//DrawDebugSphere(GetWorld(), GetTransform().GetLocation(), radius, 20, color);
}

// Called when the echo collides with something
void AEcho::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<FHitResult> HitResults;
	const FVector ActorLoc = GetActorLocation();

	if (!OtherActor->GetName().Equals("Cube21_9"))
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("%s"), *OtherActor->GetName()));
	bGoing = false;

	FCollisionShape RaySphere;
	RaySphere.ShapeType = ECollisionShape::Sphere;
	RaySphere.SetSphere(radius);

	//DrawDebugSphere(GetWorld(), ActorLoc, radius, 20, FColor::Red, true);

	GetWorld()->SweepMultiByChannel(HitResults, ActorLoc, ActorLoc, FQuat::FQuat(), ECollisionChannel::ECC_WorldDynamic, RaySphere);
	DrawDebugSphere(GetWorld(), GetTransform().GetLocation(), radius, 20, FColor::Red, true);

	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("%f hits"), HitResults.Num()));

	for (FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		// piss on nothing but the man himself
		if (HitActor != OtherActor)
		{
			continue;
		}

		// Not in the plane of the echo
		if (UKismetMathLibrary::Abs((Hit.ImpactPoint.Z - ActorLoc.Z)) > 0.01f)
		{
			continue;
		}

		Collisions.Add(Hit.ImpactPoint);
	}
}