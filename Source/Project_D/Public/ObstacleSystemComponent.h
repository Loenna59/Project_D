// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObstacleSystemComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_D_API UObstacleSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObstacleSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// 장애물과의 상호작용을 시도
	void TriggerOverObstacle() const;
	
	// 장애물 감지
	void DetectObstacle(bool &bOutDetect, FVector &OutHitLocation, FRotator &OutReverseNormal, const bool &bVerbose) const;

	/// 장애물을 파악하여 착지 지점 등을 계산
	/// @param DetectLocation 벽을 감지한 위치
	/// @param ReverseNormal
	/// @param bVerbose Trace 결과를 모두 Draw 할 것인지 여부
	void ScanObstacle(const FVector& DetectLocation, const FRotator& ReverseNormal, const bool& bVerbose) const;
};