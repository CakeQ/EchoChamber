// Fill out your copyright notice in the Description page of Project Settings.


#include "Echo.h"
#include "DrawDebugHelpers.h"
#include "Math/Color.h"

// Sets default values
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

	// Collider
	colliderSphere->SetSphereRadius(radius);
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
	speed += acceleration;
	radius += (speed * DeltaTime);

	// Update the collision sphere
	colliderSphere->SetSphereRadius(radius);

	DrawDebugCircle(
		GetWorld(),
		circleTransform,
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
	DrawDebugSphere(GetWorld(), circleTransform.GetOrigin(), radius, segments, color);
}

