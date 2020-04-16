// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoFinishLine.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEchoFinishLine::AEchoFinishLine()
{
	TriggerArea = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionArea"));
	TriggerArea->InitSphereRadius(100.0f);
	RootComponent = TriggerArea;

	FinishPad = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PadMesh"));
	FinishPad->SetupAttachment(RootComponent);
}

void AEchoFinishLine::BeginPlay()
{
	TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &AEchoFinishLine::OnOverlapBegin);
}

void AEchoFinishLine::TriggerFinish()
{
	UGameplayStatics::OpenLevel(this, TEXT("UILevel"));
}

void AEchoFinishLine::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TriggerFinish();
}


