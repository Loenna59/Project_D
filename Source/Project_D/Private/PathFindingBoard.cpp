// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFindingBoard.h"

#include "GameDebug.h"

// Sets default values
APathFindingBoard::APathFindingBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APathFindingBoard::BeginPlay()
{
	Super::BeginPlay();

	DestinationActor = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (FieldFactory)
	{
		Fields = new APathField*[BoardSize.X * BoardSize.Y];
		
		for (int32 i = 0, y = 0; y < BoardSize.Y; y++)
		{
			for (int32 x = 0; x < BoardSize.X; x++, i++)
			{
				APathField* Field = GetWorld()->SpawnActor<APathField>(FieldFactory, GetActorLocation() + FVector(x * 100, y * 100, 0), FRotator::ZeroRotator);
				Field->SetActorScale3D(FVector::OneVector);
				Field->ClearPath();
				Field->SetHeight();

				if (x > 0)
				{
					APathField::MakeEastWestNeighbors(Field, Fields[i - 1]);
				}

				if (y > 0)
				{
					int32 Index = i - FMath::RoundToInt32(BoardSize.X);
					APathField::MakeNorthSouthNeighbors(Field, Fields[Index]);
				}

				Field->SetIsAlternative((x & 1) == 0);
				if ((y & 1) == 0)
				{
					Field->SetIsAlternative(!Field->GetIsAlternative());
				}
				
				Fields[i] = Field;
			}
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
		APathField* Field;
		if (SearchFrontier.Dequeue(Field))
		{
			if (Field)
			{
				if (Field->GetIsAlternative())
				{
					if (Field->CanMoveTo(Field->North))
					{
						SearchFrontier.Enqueue(Field->GrowPathNorth());
					}

					if (Field->CanMoveTo(Field->South))
					{
						SearchFrontier.Enqueue(Field->GrowPathSouth());
					}

					if (Field->CanMoveTo(Field->East))
					{
						SearchFrontier.Enqueue(Field->GrowPathEast());
					}

					if (Field->CanMoveTo(Field->West))
					{
						SearchFrontier.Enqueue(Field->GrowPathWest());
					}
				}
				else
				{
					if (Field->CanMoveTo(Field->West))
					{
						SearchFrontier.Enqueue(Field->GrowPathWest());
					}
					
					if (Field->CanMoveTo(Field->East))
					{
						SearchFrontier.Enqueue(Field->GrowPathEast());
					}
					
					if (Field->CanMoveTo(Field->South))
					{
						SearchFrontier.Enqueue(Field->GrowPathSouth());
					}
					
					if (Field->CanMoveTo(Field->North))
					{
						SearchFrontier.Enqueue(Field->GrowPathNorth());
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
	int32 Col = FMath::FloorToInt((WorldLocation.X - GetActorLocation().X) / 100);
	int32 Row = FMath::FloorToInt((WorldLocation.Y - GetActorLocation().Y) / 100);

	return Row * BoardSize.X + Col;
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

