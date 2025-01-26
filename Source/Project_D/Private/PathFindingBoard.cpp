// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFindingBoard.h"

#include "PathVector.h"
#include "TraceChannelHelper.h"

// Sets default values
APathFindingBoard::APathFindingBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Fields.SetNum(BoardSize.X * BoardSize.Y);

	for (int32 i = 0, y = 0; y < BoardSize.Y; y++)
	{
		for (int32 x = 0; x < BoardSize.X; x++, i++)
		{
			UPathVector* Field = CreateDefaultSubobject<UPathVector>(FName(FString::Printf(TEXT("PathVector %d"), i)));
			Field->Location = (GetActorLocation() + FVector(x * FieldSize, y * FieldSize, 88.f));
			Field->ClearPath();
			Fields[i] = Field;
		}
	}
}

// Called when the game starts or when spawned
void APathFindingBoard::BeginPlay()
{
	Super::BeginPlay();

	DestinationActor = GetWorld()->GetFirstPlayerController()->GetPawn();

	for (int32 i = 0, y = 0; y < BoardSize.Y; y++)
	{
		for (int32 x = 0; x < BoardSize.X; x++, i++)
		{
			// UPathVector* Field = NewObject<UPathVector>();
			UPathVector* Field = Fields[i];

			Field->Location = (GetActorLocation() + FVector(x * FieldSize, y * FieldSize, 0));

			FVector Start = Field->Location;
			FVector End = Field->Location - FVector(0, 0, 10000);

			TraceChannelHelper::LineTraceByChannel(
				GetWorld(),
				this,
				Start,
				End,
				ECC_Visibility,
				true,
				false,
				[Field] (bool bHit, FHitResult HitResult)
				{
					if (bHit)
					{
							
						Field->Height = HitResult.ImpactPoint.Z;
						FVector NewLocation = FVector(Field->Location.X, Field->Location.Y, Field->Height + 88.f);
						Field->Location = NewLocation;

						FVector ImpactNormal = HitResult.ImpactNormal;
						Field->SlopeAngle = FMath::Acos(FVector::DotProduct(ImpactNormal, FVector::UpVector)) * (180.f / PI);

						if (FMath::Abs(ImpactNormal.X) > FMath::Abs((ImpactNormal.Y))) // East, West
						{
							Field->SlopeAngle *= (ImpactNormal.X < 0)? -1 : 1;
						}
						else
						{
							Field->SlopeAngle *= (ImpactNormal.Y < 0)? -1 : 1;
						}
					}
					else
					{
						Field->Height = Field->Location.Z;
					}
				}
			);
				
			Field->ClearPath();
			// Field.SetHeight(this);

			if (x > 0)
			{
				UPathVector::MakeEastWestNeighbors(Field, Fields[i - 1]);
			}

			if (y > 0)
			{
				int32 Index = i - FMath::RoundToInt32(BoardSize.X);
				UPathVector::MakeNorthSouthNeighbors(Field, Fields[Index]);
			}

			Field->SetIsAlternative((x & 1) == 0);
			if ((y & 1) == 0)
			{
				Field->SetIsAlternative(!Field->GetIsAlternative());
			}

			// Fields[i] = Field;
		}
	}

	FindPaths(0);
	
}

void APathFindingBoard::FindPaths(int32 DestIndex)
{
	int32 Length = BoardSize.X * BoardSize.Y; //sizeof(Fields) / sizeof(Fields[0]);
	
	for (int32 i = 0; i < Length; i++)
	{
		Fields[i]->ClearPath();
	}
	
	if (DestIndex < 0 || DestIndex >= Length)
	{
		return;
	}
	
	Fields[DestIndex]->BecomeDestination();
	SearchFrontier.Enqueue(Fields[DestIndex]);

	while (!SearchFrontier.IsEmpty())
	{
		UPathVector* Field;
		if (SearchFrontier.Dequeue(Field))
		{
			if (Field)
			{
				if (Field->GetIsAlternative())
				{
					if (Field->CanMoveTo(Field->North, MovableCost, MovableSlopeAngle))
					{
						SearchFrontier.Enqueue(Field->GrowPathNorth(MovableCost));
					}
					
					if (Field->CanMoveTo(Field->South, MovableCost, MovableSlopeAngle))
					{
						SearchFrontier.Enqueue(Field->GrowPathSouth(MovableCost));
					}

					if (Field->CanMoveTo(Field->East, MovableCost, MovableSlopeAngle))
					{
						SearchFrontier.Enqueue(Field->GrowPathEast(MovableCost));
					}

					if (Field->CanMoveTo(Field->West, MovableCost, MovableSlopeAngle))
					{
						SearchFrontier.Enqueue(Field->GrowPathWest(MovableCost));
					}
				}
				else
				{
					if (Field->CanMoveTo(Field->West, MovableCost, MovableSlopeAngle))
					{
						SearchFrontier.Enqueue(Field->GrowPathWest(MovableCost));
					}
					
					if (Field->CanMoveTo(Field->East, MovableCost, MovableSlopeAngle))
					{
						SearchFrontier.Enqueue(Field->GrowPathEast(MovableCost));
					}
					
					if (Field->CanMoveTo(Field->South, MovableCost, MovableSlopeAngle))
					{
						SearchFrontier.Enqueue(Field->GrowPathSouth(MovableCost));
					}
					
					if (Field->CanMoveTo(Field->North, MovableCost, MovableSlopeAngle))
					{
						SearchFrontier.Enqueue(Field->GrowPathNorth(MovableCost));
					}
					
				}
			}
		}
	}

	for (int32 i = 0; i < Length; i++)
	{
		Fields[i]->ShowPath();
	}
}

int32 APathFindingBoard::GetFieldIndex(FVector WorldLocation)
{
	int32 Col = FMath::FloorToInt((WorldLocation.X - GetActorLocation().X) / FieldSize);
	int32 Row = FMath::FloorToInt((WorldLocation.Y - GetActorLocation().Y) / FieldSize);

	return Row * BoardSize.X + Col;
}

class UPathVector* APathFindingBoard::FindField(FVector WorldLocation)
{
	int32 Index = GetFieldIndex(WorldLocation);

	if (Index < 0 || Index >= BoardSize.X * BoardSize.Y)
	{
		return nullptr;
	}

	return Fields[Index];
}

// Called every frame
void APathFindingBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DestinationActor)
	{
		FVector NewDestination = DestinationActor->GetActorLocation();
		int32 NewFieldIndex = GetFieldIndex(NewDestination);

		if (NewFieldIndex != LastDestIndex)
		{
			LastDestIndex = NewFieldIndex;
			FindPaths(NewFieldIndex);
		}
	}
}

