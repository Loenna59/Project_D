// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "ActionComponent.generated.h"

class AZipline;
class IPlayerAnimBlueprintInterface;
class UCharacterMovementComponent;
class UCapsuleComponent;
class UMotionWarpingComponent;
class IPlayerInterface;

UENUM()
enum class EVaults : uint8
{
	OneHandVault,
	TwoHandVault,
	FrontFlip
};

UENUM()
enum class EActionState : uint8
{
	WalkingOnGround,
	Climbing,
	Zipping
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_D_API UActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void Initialize();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bVerboseTick = false;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bVerboseDetect = false;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bVerboseScan = false;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bVerboseMeasure = false;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bVerboseScanObstacle = false;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bVerboseInteract = false;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
    bool bVerboseMontage = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation Montages")
	UAnimMontage* OneHandVault = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Animation Montages")
	UAnimMontage* TwoHandVault = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Animation Montages")
	UAnimMontage* FrontFlip = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Animation Montages")
	UAnimMontage* ClimbStart = nullptr;
	
	IPlayerInterface* PlayerInterface = nullptr;
	UPROPERTY()
	USkeletalMeshComponent* PlayerMesh = nullptr;
	UPROPERTY()
	UCapsuleComponent* PlayerCapsule = nullptr;
	UPROPERTY()
	UCharacterMovementComponent* PlayerMovement = nullptr;
	UPROPERTY()
	UMotionWarpingComponent* PlayerMotionWarping = nullptr;
	UPROPERTY()
	UAnimInstance* PlayerAnimInstance = nullptr;
	IPlayerAnimBlueprintInterface* PlayerAnimInterface = nullptr;
	UPROPERTY()
	EActionState PlayerActionState = EActionState::WalkingOnGround;
	
	// 현재 마주보고 있는 장애물의 가장 높은 지점
	FHitResult FacedWallTopHitResult;
	// 플레이어가 벽을 넘을 방향 (장애물의 해당 부분 normal vector를 180도 회전시킨 결과)
	FRotator WallRotation = FRotator::ZeroRotator;
	// 장애물의 Top에 있는 SphereTrace 중 Player 방향에서 가장 가까운 곳에 있는 것
	FHitResult FirstTopHitResult;
	// 장애물의 Top에 있는 SphereTrace 중 Player 방향에서 가장 먼 곳에 있는 것
	FHitResult LastTopHitResult;
	// 장애물의 정확한 끝 지점
	FHitResult EndOfObstacleHitResult;
	// 뛰어넘을 때(Vaulting) Player의 착지 지점
	FHitResult VaultLandingHitResult;
	// 장애물의 높이 (장애물의 실제 높이가 아닌 Player의 발 위치부터 장애물 꼭대기까지의 높이)
	float WallHeight = 0.0f;
	// 현재 Player가 지면에 서 있는지 여부
	bool bIsOnLand = false;
	// 현재 Player가 장애물과의 Interact를 새롭게 시작할 수 있는지 여부
	bool bCanInteract = true;

	// Climb
	FVector2D MovementVector = FVector2d::ZeroVector;
	FHitResult WallHitResultForClimbMove;
	FHitResult WallTopHitResultForClimbMove;

	// 장애물과의 상호작용을 시도
	void TriggerInteractWall();

	/// 플레이어 앞에 장애물이 있는지 탐색
	/// @param bOutDetect 
	/// @param OutHitLocation 
	/// @param OutReverseNormal 
	/// @param bVerbose Trace 결과를 모두 DebugDraw 할 것인지 여부
	void DetectWall(bool &bOutDetect, FVector &OutHitLocation, FRotator &OutReverseNormal) const;

	/// 장애물을 파악하여 착지 지점 등을 계산
	/// @param DetectLocation 벽을 감지한 위치
	/// @param ReverseNormal
	/// @param bVerbose Trace 결과를 모두 DebugDraw 할 것인지 여부
	void ScanWall(const FVector& DetectLocation, const FRotator& ReverseNormal);

	/// 장애물의 높이를 계산
	/// @param bVerbose 실시간으로 계산된 장애물의 높이를 Debug 할 것인지 여부
	void MeasureWall();

	// 
	void TryInteractWall();

	UFUNCTION()
	void OnVaultMontageStarted(UAnimMontage* Montage);
	
	UFUNCTION()
	void OnVaultMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnVaultMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	
	//
	void TryVault(const EVaults VaultType);

	UFUNCTION()
	void OnClimbMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	
	//
	void TryClimb();

	//
	void MoveOnWall(const FVector2D& InMovementVector);

	//
	void ResetMoveValue();

	//
	void TriggerClimbMovement();

public:
	UPROPERTY()
	AZipline* TargetZipline = nullptr;
	// Player가 Zipline을 탈 수 있는지 여부
	bool bCanZipping = false;
	FVector ZippingStartPosition = FVector::ZeroVector;
	FVector ZippingEndPosition = FVector::ZeroVector;

	//
	void TryRideZipline();
};