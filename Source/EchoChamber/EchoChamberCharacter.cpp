// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "EchoChamberCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "EchoChamber/Echoes/ECEchoEmitter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Math/UnrealMathUtility.h"

AEchoChamberCharacter::AEchoChamberCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Echo related stuff
	EchoCooldown = 5.0f; // seconds
	// This makes the echo immediately usable
	LastEchoTime = -EchoCooldown;
}

void AEchoChamberCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AEchoChamberCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveUp", this, &AEchoChamberCharacter::MoveUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &AEchoChamberCharacter::MoveRight);
	PlayerInputComponent->BindAction("Ping", IE_Pressed, this, &AEchoChamberCharacter::AttemptEcho);
}


void AEchoChamberCharacter::MoveUp(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AEchoChamberCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AEchoChamberCharacter::AttemptEcho()
{
	float CurTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());

	// Echo is still on cooldown
	if (CurTime < (LastEchoTime + EchoCooldown))
	{
		return;
	}

	LastEchoTime = CurTime;
	MakeEcho();
}

void AEchoChamberCharacter::MakeEcho()
{
#ifdef ENABLE_RAYTRACE_ECHO
	const int NumTraces = 60;
	const float DeltaAngle = 360.0f / NumTraces;
	const float DistanceToTrace = 600.0f;

	FVector ActorLocation = GetActorLocation();
	FVector FacingVec = GetActorForwardVector();
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;		
	FCollisionResponseParams CollisionParams = FCollisionResponseParams::DefaultResponseParam;
	FHitResult hitResult;

	for (int TraceIdx = 0; TraceIdx < NumTraces; TraceIdx++)
	{
		float DistanceRemaining = DistanceToTrace;
		FVector origin = ActorLocation;
		FVector TraceDirection = UKismetMathLibrary::RotateAngleAxis(FacingVec, DeltaAngle * (float)TraceIdx, GetActorUpVector());
		TraceDirection.Normalize();

		bool bTraceEnded = false;
		while (!bTraceEnded)
		{
			bTraceEnded = !GetWorld()->LineTraceSingleByChannel(hitResult, origin, origin + TraceDirection * DistanceRemaining, ECollisionChannel::ECC_Visibility, Params, CollisionParams);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			if (hitResult.bBlockingHit)
			{
				DrawDebugLine(GetWorld(), origin, origin + TraceDirection * hitResult.Distance, FColor::Red, false, 1.0f);
			}
#endif

			if (hitResult.bBlockingHit)
			{
				FRotator rotation = UKismetMathLibrary::MakeRotationFromAxes(GetActorUpVector() * -1.0f, FVector::CrossProduct(GetActorUpVector() * -1.0f, hitResult.ImpactNormal), hitResult.ImpactNormal);
				UParticleSystemComponent* SpawnedComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, (FVector)hitResult.ImpactPoint, rotation);
			}

			TraceDirection = FMath::GetReflectionVector(TraceDirection, hitResult.ImpactNormal);
			origin = hitResult.ImpactPoint;
			
			DistanceRemaining -= hitResult.Distance;
			if (DistanceRemaining <= 0.0f)
			{
				bTraceEnded = true;
			}
		}
	}
#else
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector location = GetActorLocation();
	FRotator rotation = GetActorRotation();

	FTransform transform;
	transform.SetLocation(GetActorLocation());
	transform.SetRotation(GetActorRotation().Quaternion());

	//AActor* actor = GetWorld()->SpawnActor<AECEchoEmitter>(EchoEmitterClass.GetDefaultObject()->GetClass(), transform, params);
	AActor* actor = GetWorld()->SpawnActor<AECEchoEmitter>(EchoEmitterClass.GetDefaultObject()->GetClass(), transform, params);
#endif
}
