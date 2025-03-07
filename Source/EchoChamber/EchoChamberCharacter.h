// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EchoChamberCharacter.generated.h"

// Uncomment to enable raytrace echos
//#define ENABLE_RAYTRACE_ECHO

class AECEchoEmitter;

UCLASS(Blueprintable)
class AEchoChamberCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEchoChamberCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Called for upwards input */
	void MoveUp(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void MakeEcho();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Echo)
	TSubclassOf<AECEchoEmitter> EchoEmitterClass;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Echo, meta = (AllowPrivateAccess = "true"))
	float EchoCooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Echo, meta = (AllowPrivateAccess = "true"))
	float LastEchoTime;

	UFUNCTION()
	void AttemptEcho();
};

