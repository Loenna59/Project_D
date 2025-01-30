// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathVector.h"
#include "PathFindingBoard.generated.h"

UCLASS()
class PROJECT_D_API APathFindingBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathFindingBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	TArray<class UPathVector*> RetracePath(UPathVector* StartNode, UPathVector* EndNode);

public:	
	int32 GetFieldIndex(FVector WorldLocation);

	class UPathVector* FindField(FVector WorldLocation);

	TArray<class UPathVector*> FindPaths(int32 StartIndex);
	
	void TryAddToOpenSet(UPathVector* Current, UPathVector* Neighbor, EPathDirection Direction, TArray<UPathVector*>& OpenSet);

	float CalculateHeuristic(UPathVector* Start, UPathVector* Dest);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D BoardSize = FVector2D(80, 80);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FieldSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovableCost = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovableSlopeAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UPathVector*> Fields;

	TQueue<class UPathVector*> SearchFrontier;

	UPROPERTY()
	class AActor* DestinationActor;

	int32 LastDestIndex = -1;
};
