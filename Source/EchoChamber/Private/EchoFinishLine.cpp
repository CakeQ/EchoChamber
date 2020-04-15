// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoFinishLine.h"
#include "Engine.h"

// Sets default values
AEchoFinishLine::AEchoFinishLine()
{
	TriggerArea = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionArea"));
	TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &AEchoFinishLine::BeginOverlap);
	RootComponent = TriggerArea;

	FinishPad = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PadMesh"));
	FinishPad->SetupAttachment(RootComponent);
}

void AEchoFinishLine::BeginPlay()
{
	Super::BeginPlay();
	TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &AEchoFinishLine::BeginOverlap);
}

void AEchoFinishLine::TriggerFinish()
{
	UGameplayStatics::OpenLevel(this, TEXT("UILevel"));
}

void AEchoFinishLine::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TriggerFinish();
}

