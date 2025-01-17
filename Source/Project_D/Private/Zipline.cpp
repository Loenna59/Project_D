// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"

#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AZipline::AZipline()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Cable = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	StartStick = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartStick"));
	EndStick = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EndStick"));
	StartCablePosition = CreateDefaultSubobject<USceneComponent>(TEXT("StartCablePosition"));
	EndCablePosition = CreateDefaultSubobject<USceneComponent>(TEXT("EndCablePosition"));
	StartStickColliderBox = CreateDefaultSubobject<UBoxComponent>(TEXT("StartStickColliderBox"));
	
	SetRootComponent(Root);
	Cable->SetupAttachment(RootComponent);
	StartStick->SetupAttachment(RootComponent);
	EndStick->SetupAttachment(RootComponent);
	StartCablePosition->SetupAttachment(StartStick);
	EndCablePosition->SetupAttachment(EndStick);
	StartStickColliderBox->SetupAttachment(RootComponent);
	
	Root->SetMobility(EComponentMobility::Static);
	StartStick->SetMobility(EComponentMobility::Static);
	EndStick->SetMobility(EComponentMobility::Static);
	StartCablePosition->SetMobility(EComponentMobility::Static);
	EndCablePosition->SetMobility(EComponentMobility::Static);
	
	StartStick->SetRelativeScale3D(FVector(0.1f, 0.1f, 3.0f));
	EndStick->SetRelativeScale3D(FVector(0.1f, 0.1f, 3.0f));
	
	StartCablePosition->SetRelativeLocation(FVector(0, 0, 45));
	EndCablePosition->SetRelativeLocation(FVector(0, 0, 45));
	
	StartStickColliderBox->SetBoxExtent(FVector(150.0f, 150.0f, 150.0f));
	StartStickColliderBox->OnComponentBeginOverlap.AddDynamic(this, &AZipline::OnComponentBeginOverlap);
	StartStickColliderBox->OnComponentEndOverlap.AddDynamic(this, &AZipline::OnComponentEndOverlap);
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FVector RootVector = Root->GetComponentLocation();
	StartStick->SetWorldLocation(FVector(RootVector.X, RootVector.Y, RootVector.Z + 150.0f));
	StartStickColliderBox->SetWorldLocation(StartStick->GetComponentLocation());
	Cable->SetWorldLocation(StartCablePosition->GetComponentLocation());
	Cable->SetAttachEndTo(this, EndCablePosition->GetFName());
}

// Called when the game starts or when spawned
void AZipline::BeginPlay()
{
	Super::BeginPlay();
}

void AZipline::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 만약, BeginOverlap 이벤트가 발생하였고, OtherActor가 APlayerCharacter라면 Player에게 알린다.
	//		-> Player는 Zipline을 탈 수 있는 상태로 세팅한다.
	
	UE_LOG(LogTemp, Warning, TEXT("AZipline::OnComponentBeginOverlap"));
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->OnZiplineBeginOverlap(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AZipline::OnComponentBeginOverlap : NotPlayer!!!!"));
	}
}

void AZipline::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("AZipline::OnComponentEndOverlap"));
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->OnZiplineEndOverlap(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AZipline::OnComponentEndOverlap : NotPlayer!!!!"));
	}
}
