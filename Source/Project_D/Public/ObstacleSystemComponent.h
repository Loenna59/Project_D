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
	// 현재 마주보고 있는 장애물의 가장 높은 지점
	FHitResult FacedObstacleTopHitResult;
	// 플레이어가 벽을 넘을 방향 (장애물의 해당 부분 normal vector를 180도 회전시킨 결과)
	FRotator ObstacleRotation;
	// 장애물의 Top에 있는 SphereTrace 중 Player 방향에서 가장 가까운 곳에 있는 것
	FHitResult FirstTopHitResult;
	// 장애물의 Top에 있는 SphereTrace 중 Player 방향에서 가장 먼 곳에 있는 것
	FHitResult LastTopHitResult;
	// 장애물의 정확한 끝 지점
	FHitResult EndOfObstacleHitResult;
	// 뛰어넘을 때(Vaulting) Player의 착지 지점
	FHitResult VaultLandingHitResult;
	
	// 장애물과의 상호작용을 시도
	void TriggerOverObstacle();

	/// 플레이어 앞에 장애물이 있는지 탐색
	/// @param bOutDetect 
	/// @param OutHitLocation 
	/// @param OutReverseNormal 
	/// @param bVerbose Trace 결과를 모두 DebugDraw 할 것인지 여부
	void DetectObstacle(bool &bOutDetect, FVector &OutHitLocation, FRotator &OutReverseNormal, const bool &bVerbose) const;

	/// 장애물을 파악하여 착지 지점 등을 계산
	/// @param DetectLocation 벽을 감지한 위치
	/// @param ReverseNormal
	/// @param bVerbose Trace 결과를 모두 DebugDraw 할 것인지 여부
	void ScanObstacle(const FVector& DetectLocation, const FRotator& ReverseNormal, const bool& bVerbose);
};