// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathField.h"
#include "GameFramework/Actor.h"
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
	
	void FindPaths(int32 DestIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetFieldIndex(FVector WorldLocation);

	class APathField* FindField(FVector WorldLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D BoardSize = FVector2D(2, 2);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FieldSize = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class APathField> FieldFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovableCost = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovableSlopeAngle = 45.f;
	
	class APathField** Fields;

	TQueue<class APathField*> SearchFrontier;

	UPROPERTY()
	class AActor* DestinationActor;

	int32 LastDestIndex = -1;
};
