// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathField.generated.h"

UENUM(BlueprintType)
enum class EPathDirection : uint8
{
	North,
	East,
	South,
	West
};

class EPathDirectionExtensions
{
public:
	// 방향에 따른 회전 값 반환
	static FQuat GetRotation(EPathDirection Direction)
	{
		static const FQuat Rotations[] = {
			FQuat(FRotator(0.f, 180.f, 0.f)),
			FQuat(FRotator(0.0f, 90.0f, 0.0f)),
			FQuat::Identity,
			FQuat(FRotator(0.0f, 270.0f, 0.0f))
		};

		return Rotations[static_cast<uint8>(Direction)];
	}
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
	class UTextRenderComponent* TextRender;

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

	class APathField* GetNextOnPath() const
	{
		return NextOnPath;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Distance;

	void ClearPath();
	void BecomeDestination();
	bool HasPath();
	void ShowPath();

	class APathField* GrowPathTo(APathField* Neighbor, EPathDirection Direction, float Weight);
	
	class APathField* GrowPathNorth(float Weight);
	class APathField* GrowPathEast(float Weight);
	class APathField* GrowPathSouth(float Weight);
	class APathField* GrowPathWest(float Weight);

	bool GetIsAlternative() const
	{
		return IsAlternative;
	}

	void SetIsAlternative(bool bIsAlternative)
	{
		IsAlternative = bIsAlternative;
	}

	UPROPERTY(EditAnywhere)
	float Height;

	UPROPERTY(EditAnywhere)
	float SlopeAngle = 0;

	UPROPERTY()
	FVector ExitPoint;

	UPROPERTY()
	EPathDirection PathDirection;

	bool CanMoveTo(APathField* Neighbor, float Weight, float Angle) const;
	void SetHeight();

	static void MakeEastWestNeighbors(APathField* East, APathField* West);
	static void MakeNorthSouthNeighbors(APathField* North, APathField* South);

	static FQuat NorthRotation;
	static FQuat EastRotation;
	static FQuat SouthRotation;
	static FQuat WestRotation;
};
