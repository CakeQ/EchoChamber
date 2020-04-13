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
	Radius = 20.0f;
	Speed = 5.0f;
	Acceleration = 2.0f;

	// Visual appearance
	Thickness = 5.0f;
	Color = FColor(255, 255, 255, 255);
	Segments = 360;

	// Decay/fade
	StrengthLossRate = 15.0f;
	Strength = 100.0f;
	StrengthBounceMultiplier = 1.1f;

	// Collider/overlap detector sphere
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->InitSphereRadius(Radius);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapIgnorePawn"));

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEcho::OnOverlapBegin);

	// Trace sweeps on collides/overlaps
	SweepAngle = 60.0f;
	MaxTraceDist = 600.0f;
	TracesPerSweep = 10;
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

	// Update strength decay
	Strength -= (StrengthLossRate * DeltaTime);

	// Check if we should die
	if (Strength <= 0.0f)
	{
		Destroy();
	}

	// Update the circle radius
	Speed += Acceleration;
	Radius += (Speed * DeltaTime);
	CollisionSphere->SetSphereRadius(Radius);

	// Pitch by -90deg before drawing
	FTransform ActorTransform = GetActorTransform();
	FQuat RotationQuat = FQuat(FVector(0.0f, 1.0f, 0.0f), UKismetMathLibrary::DegreesToRadians(-90.0f));
	ActorTransform.SetRotation(RotationQuat);
	FMatrix Transform = ActorTransform.ToMatrixNoScale();

	FColor CircleColor = Color;
	CircleColor.A = 255.0f * (Strength / 100.0f);

	DrawDebugCircle(
		GetWorld(),
		Transform,
		Radius,
		Segments,
		FColor(255, 255, 255, 10),
		false,
		-1,
		(uint8)'\000',
		Thickness,
		false
	);
}

void AEcho::SetSpawnSurfaceActor(AActor* Actor)
{
	SpawnedOn = Actor;
}

void AEcho::SetEchoStrength(float NewStrength)
{
	Strength = NewStrength;
}

// Called when the echo collides with something
void AEcho::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Don't "collide" with and bounce off actors we were spawned on the surface of
	if (OtherActor == SpawnedOn)
	{
		return;
	}

	// Don't collide with other echos
	if (OtherActor->GetClass() == GetClass())
	{
		return;
	}

	FVector ActorLoc = GetActorLocation();
	FVector UpAxis = GetActorUpVector();

	// Perform the traces around the closest point on the bounding box of the overlapped actor
	FVector ClosestBBoxPoint = OtherActor->GetComponentsBoundingBox().GetClosestPointTo(ActorLoc);
	FVector DirToClosestPt = ClosestBBoxPoint - ActorLoc;
	DirToClosestPt.Normalize();

	int MiddleTrace = UKismetMathLibrary::FFloor((float)TracesPerSweep / 2.0f);
	float TraceAngleDelta = SweepAngle / TracesPerSweep;

	// Offset trace direction to where it should start
	FVector TraceDir = UKismetMathLibrary::RotateAngleAxis(DirToClosestPt, -SweepAngle / 2, UpAxis);
	FHitResult HitResult;

	for (int TraceIdx = 0; TraceIdx < TracesPerSweep; TraceIdx++)
	{
		TraceDir = UKismetMathLibrary::RotateAngleAxis(DirToClosestPt, (-SweepAngle / 2) + TraceAngleDelta * (float)TraceIdx, UpAxis);

		GetWorld()->LineTraceSingleByChannel(HitResult, ActorLoc, ActorLoc + TraceDir * MaxTraceDist, ECollisionChannel::ECC_Visibility);

		if (HitResult.bBlockingHit)
		{
			OnEchoBounce(HitResult.ImpactPoint, OtherActor);
		}
	}
}

// Called when the echo should bounce off another actor
void AEcho::OnEchoBounce(FVector HitLocation, AActor* HitActor)
{
	AEcho* Echo = GetWorld()->SpawnActor<AEcho>(GetClass(), HitLocation, FRotator::ZeroRotator);
	Echo->SetSpawnSurfaceActor(HitActor);
	Echo->SetEchoStrength(Strength * StrengthBounceMultiplier);

	// Lose some strength ourself
	SetEchoStrength(Strength / StrengthBounceMultiplier);
}