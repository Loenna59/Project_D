// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"

#include "FileCache.h"

// Sets default values
AZipline::AZipline()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Cable = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	StartStick = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartStick"));
	EndStick = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EndStick"));
	StartCablePosition = CreateDefaultSubobject<USceneComponent>(TEXT("StartCablePosition"));
	EndCablePosition = CreateDefaultSubobject<USceneComponent>(TEXT("EndCablePosition"));
	
	SetRootComponent(Root);
	Cable->SetupAttachment(RootComponent);
	StartStick->SetupAttachment(RootComponent);
	EndStick->SetupAttachment(RootComponent);
	StartCablePosition->SetupAttachment(StartStick);
	EndCablePosition->SetupAttachment(EndStick);
	
	Root->SetMobility(EComponentMobility::Static);
	StartStick->SetMobility(EComponentMobility::Static);
	EndStick->SetMobility(EComponentMobility::Static);
	StartCablePosition->SetMobility(EComponentMobility::Static);
	EndCablePosition->SetMobility(EComponentMobility::Static);
	
	StartStick->SetRelativeScale3D(FVector(0.1f, 0.1f, 3.0f));
	EndStick->SetRelativeScale3D(FVector(0.1f, 0.1f, 3.0f));
	
	StartCablePosition->SetRelativeLocation(FVector(0, 0, 45));
	EndCablePosition->SetRelativeLocation(FVector(0, 0, 45));
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FVector RootVector = Root->GetComponentLocation();
	StartStick->SetWorldLocation(FVector(RootVector.X, RootVector.Y, RootVector.Z + 150.0f));
	Cable->SetWorldLocation(StartCablePosition->GetComponentLocation());
	Cable->SetAttachEndTo(this, TEXT("EndCablePosition"));
}

// Called when the game starts or when spawned
void AZipline::BeginPlay()
{
	Super::BeginPlay();
}