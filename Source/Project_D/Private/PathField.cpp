// Fill out your copyright notice in the Description page of Project Settings.


#include "PathField.h"

#include "GameDebug.h"
#include "TraceChannelHelper.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetSystemLibrary.h"

FQuat APathField::NorthRotation = FQuat(FRotator(0.f, 180.f, 0.f));
FQuat APathField::EastRotation = FQuat(FRotator(0.f, 90.f, 0.f));
FQuat APathField::SouthRotation = FQuat(FRotator(0.f, 0.f, 0.f));
FQuat APathField::WestRotation = FQuat(FRotator(0.f, 270.f, 0.f));

// Sets default values
APathField::APathField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	ConstructorHelpers::FObjectFinder<UStaticMesh> Plane(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));

	if (Plane.Succeeded())
	{
		Mesh->SetStaticMesh(Plane.Object);
	}

	ConstructorHelpers::FObjectFinder<UMaterial> Mat(TEXT("/Script/Engine.Material'/Game/Assets/Textures/M_Arrow.M_Arrow'"));

	if (Mat.Succeeded())
	{
		Mesh->SetMaterial(0, Mat.Object);
	}

	TextRender = CreateDefaultSubobject<UTextRenderComponent>("Distance");
	TextRender->SetRelativeLocation(FVector(0, 0, 1.f));
	TextRender->SetRelativeRotation(FRotator(90, 0, 0));
	TextRender->SetTextRenderColor(FColor::Red);
	TextRender->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void APathField::BeginPlay()
{
	Super::BeginPlay();

	TextRender->SetVisibility(false);
	
}

// Called every frame
void APathField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TextRender->SetText(FText::FromString(FString::FromInt(Distance)));
}

void APathField::ClearPath()
{
	Distance = TNumericLimits<int32>::Max();
	NextOnPath = nullptr;
	Mesh->SetVisibility(false);
}

void APathField::BecomeDestination()
{
	Distance = 0;
	NextOnPath = nullptr;
}

bool APathField::HasPath()
{
	return Distance != TNumericLimits<int32>::Max();
}

void APathField::ShowPath()
{
	if (Distance == 0 || !HasPath())
	{
		Mesh->SetVisibility(false);
		return;
	}

	Mesh->SetVisibility(true);

	FQuat Rotation = FQuat::Identity;

	if (!NextOnPath)
	{
		SetActorRotation(Rotation);
		return;
	}

	if (North && NextOnPath->GetName() == North->GetName())
	{
		Rotation = NorthRotation * FQuat(FVector::RightVector, FMath::DegreesToRadians(SlopeAngle));
	}
	else
	if (East && NextOnPath->GetName() == East->GetName())
	{
		Rotation = EastRotation * FQuat(FVector::ForwardVector, FMath::DegreesToRadians(SlopeAngle));;
	}
	else
	if (South && NextOnPath->GetName() == South->GetName())
	{
		Rotation = SouthRotation * FQuat(FVector::RightVector, FMath::DegreesToRadians(SlopeAngle));;
	}
	else
	if (West && NextOnPath->GetName() == West->GetName())
	{
		Rotation = WestRotation * FQuat(FVector::ForwardVector, FMath::DegreesToRadians(SlopeAngle));;
	}
	
	SetActorRotation(Rotation);
}

class APathField* APathField::GrowPathTo(APathField* Neighbor)
{
	if (!HasPath() || !Neighbor || Neighbor->HasPath())
	{
		return nullptr;
	}
	
	Neighbor->Distance = this->Distance + 1;
	Neighbor->NextOnPath = this;

	return Neighbor;
}

class APathField* APathField::GrowPathNorth()
{
	return GrowPathTo(North);
}

class APathField* APathField::GrowPathEast()
{
	return GrowPathTo(East);
}

class APathField* APathField::GrowPathSouth()
{
	return GrowPathTo(South);
}

class APathField* APathField::GrowPathWest()
{
	return GrowPathTo(West);
}


void APathField::SetHeight()
{
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() - FVector(0, 0, 1000);

	TraceChannelHelper::LineTraceByChannel(
		GetWorld(),
		this,
		Start,
		End,
		ECC_Visibility,
		true,
		false,
		[this] (bool bHit, FHitResult HitResult)
		{
			if (bHit)
			{
				Height = HitResult.ImpactPoint.Z;
				FVector NewLocation = FVector(GetActorLocation().X, GetActorLocation().Y, Height);
				SetActorLocation(NewLocation);

				FVector ImpactNormal = HitResult.ImpactNormal;
				SlopeAngle = FMath::Acos(FVector::DotProduct(ImpactNormal, FVector::UpVector)) * (180.f / PI);
			}
			else
			{
				Height = GetActorLocation().Z;
			}
		}
	);
}

bool APathField::CanMoveTo(APathField* Neighbor)
{
	if (!Neighbor)
	{
		return false;
	}

	float HeightDiff = FMath::Abs(Neighbor->Height - Height);

	bool bCan = SlopeAngle <= 45.f && HeightDiff < 200;

	Mesh->SetVisibility(bCan);

	return bCan;
}

void APathField::MakeEastWestNeighbors(APathField* East, APathField* West)
{	
	if (West)
	{
		West->East = East;
	}

	if (East)
	{
		East->West = West;
	}
}

void APathField::MakeNorthSouthNeighbors(APathField* North, APathField* South)
{
	if (North)
	{
		North->South = South;
	}

	if (South)
	{
		South->North = North;
	}
}

