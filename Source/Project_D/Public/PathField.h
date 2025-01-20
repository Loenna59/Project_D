// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathField.generated.h"

UENUM(BlueprintType)
enum class FieldContentType : uint8
{
	Empty,
	Destination
};

UCLASS()
class PROJECT_D_API APathField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathField();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsAlternative;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APathField* North;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APathField* East;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APathField* South;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APathField* West;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APathField* NextOnPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Distance;

	void ClearPath();
	void BecomeDestination();

	bool HasPath();

	void ShowPath();

	class APathField* GrowPathTo(APathField* Neighbor);
	
	class APathField* GrowPathNorth();
	class APathField* GrowPathEast();
	class APathField* GrowPathSouth();
	class APathField* GrowPathWest();

	[[nodiscard]] bool GetIsAlternative() const
	{
		return IsAlternative;
	}

	void SetIsAlternative(bool bIsAlternative)
	{
		IsAlternative = bIsAlternative;
	}

	UPROPERTY(EditAnywhere)
	float Height;

	void SetHeight();

	bool CanMoveTo(APathField* Neighbor);

	static void MakeEastWestNeighbors(APathField* East, APathField* West);
	static void MakeNorthSouthNeighbors(APathField* North, APathField* South);

	static FQuat NorthRotation;
	static FQuat EastRotation;
	static FQuat SouthRotation;
	static FQuat WestRotation;
};
