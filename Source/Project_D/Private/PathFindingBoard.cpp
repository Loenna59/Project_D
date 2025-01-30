// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFindingBoard.h"

#include "PathVector.h"
#include "TraceChannelHelper.h"

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

	Fields.SetNum(BoardSize.X * BoardSize.Y);

	for (int32 i = 0, y = 0; y < BoardSize.Y; y++)
	{
		for (int32 x = 0; x < BoardSize.X; x++, i++)
		{
			UPathVector* Field = NewObject<UPathVector>();

			Field->Location = (GetActorLocation() + FVector(x * FieldSize, y * FieldSize, 88.f));

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

			Fields[i] = Field;
		}
	}
}

void APathFindingBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	int32 DestIndex = GetFieldIndex(DestinationActor->GetActorLocation());
	if (LastDestIndex != DestIndex)
	{
		LastDestIndex = DestIndex;
	}
}

TArray<UPathVector*> APathFindingBoard::FindPaths(int32 StartIndex)
{
	TArray<UPathVector*> Path; // 반환할 경로를 저장할 배열

	if (!DestinationActor)
	{
		return Path;
	}
	
	int32 Length = BoardSize.X * BoardSize.Y;

	// 모든 필드 초기화
	for (int32 i = 0; i < Length; i++)
	{
		Fields[i]->ClearPath();
	}

	int32 DestIndex = GetFieldIndex(DestinationActor->GetActorLocation());

	// 유효하지 않은 인덱스 처리
	if (StartIndex < 0 || StartIndex >= Length || DestIndex < 0 || DestIndex >= Length)
	{
		return Path; // 빈 경로 반환
	}

	// 목적지 설정
	Fields[DestIndex]->BecomeDestination();

	TArray<UPathVector*> OpenSet;
	OpenSet.Add(Fields[DestIndex]);

	// A* 알고리즘 실행
	while (OpenSet.Num() > 0)
	{
		OpenSet.Sort([](const UPathVector& A, const UPathVector& B) {
			return A.GetTotalCost() < B.GetTotalCost();
		});

		UPathVector* Current = OpenSet[0];
		OpenSet.RemoveAt(0);

		// 시작점에 도달하면 경로 구성
		if (Current == Fields[StartIndex])
		{
			Path = RetracePath(Fields[StartIndex], Fields[DestIndex]);
			break;
		}

		// 이웃 노드 탐색
		if (Current->GetIsAlternative())
		{
			TryAddToOpenSet(Current, Current->North, EPathDirection::South, OpenSet);
			TryAddToOpenSet(Current, Current->South, EPathDirection::North, OpenSet);
			TryAddToOpenSet(Current, Current->East, EPathDirection::West, OpenSet);
			TryAddToOpenSet(Current, Current->West, EPathDirection::East, OpenSet);
		}
		else
		{
			TryAddToOpenSet(Current, Current->West, EPathDirection::East, OpenSet);
			TryAddToOpenSet(Current, Current->East, EPathDirection::West, OpenSet);
			TryAddToOpenSet(Current, Current->South, EPathDirection::North, OpenSet);
			TryAddToOpenSet(Current, Current->North, EPathDirection::South, OpenSet);
		}
	}

	return Path; // 경로 반환

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

void APathFindingBoard::TryAddToOpenSet(UPathVector* Current, UPathVector* Neighbor, EPathDirection Direction, TArray<UPathVector*>& OpenSet)
{
	if (Neighbor && Current->CanMoveTo(Neighbor, MovableCost, MovableSlopeAngle))
	{
		UPathVector* NewNeighbor = Current->GrowPathTo(Neighbor, Direction, MovableCost);
		if (NewNeighbor && !OpenSet.Contains(NewNeighbor))
		{
			NewNeighbor->HeuristicCost = CalculateHeuristic(NewNeighbor, Current);
			OpenSet.Add(NewNeighbor);
		}
	}
}

float APathFindingBoard::CalculateHeuristic(UPathVector* Start, UPathVector* Goal)
{
	return FVector::Dist(Start->Location, Goal->Location);
}

TArray<UPathVector*> APathFindingBoard::RetracePath(UPathVector* StartNode, UPathVector* EndNode)
{
	TArray<UPathVector*> Path;
	UPathVector* CurrentNode = StartNode;

	// 시작점에서 목적지까지 역추적
	while (CurrentNode != EndNode)
	{
		Path.Add(CurrentNode);
		CurrentNode = CurrentNode->Next;
	}

	// 목적지 추가
	Path.Add(EndNode);

	// 경로를 뒤집어 시작점 -> 목적지 순서로 만듦
	Algo::Reverse(Path);

	return Path;
}

