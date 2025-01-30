// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "ActionComponent.generated.h"

class UPlayerAnimInstance;
class USpringArmComponent;
class AZipline;
class IPlayerAnimBlueprintInterface;
class UCharacterMovementComponent;
class UCapsuleComponent;
class UMotionWarpingComponent;
class IPlayerInterface;

UENUM()
enum class EActions : uint8
{
	OneHandVault
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Actions|OneHandVault")
	UAnimMontage* OneHandVaultMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|IdleToHang")
	UAnimMontage* IdleToHangMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|IdleToHang")
	float IdleToHangParam1 = 18.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|IdleToHang")
	float IdleToHangParam2 = 203.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|MeleeAttack")
    UAnimMontage* MeleeAttackMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|StandingKick")
	UAnimMontage* StandingKickMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|StandingKick")
	float StandingKickImpulseStrength = 50000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|StandingKick")
	float StandingKickForwardOffset = 200.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|StandingKick")
	float StandingKickRadius = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|StandingKick")
	bool bVerboseStandingKick = false;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|Dropkick")
    UAnimMontage* DropkickMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|Dropkick")
	float DropkickImpulseStrength = 100000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|Dropkick")
    float DropkickForwardOffset = 200.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|Dropkick")
	float DropkickRadius = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|Dropkick")
	bool bVerboseDropkick = false;
	UPROPERTY(EditDefaultsOnly, Category = "Actions|LandOnFallSafetyZone")
	UAnimMontage* LandOnFallSafetyZoneMontage = nullptr;

	UPROPERTY()
	APlayerCharacter* Player = nullptr;
	
	UPROPERTY()
    UPlayerAnimInstance* PlayerAnimInstance = nullptr;
	
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
	bool bCanAction = true;

	// Climb
	FHitResult WallHitResultForHangingHorizontalMove;

	// 장애물과의 상호작용을 시도
	bool TriggerInteractWall();

	/// 플레이어 앞에 장애물이 있는지 탐색
	/// @param bOutDetect 
	/// @param OutHitLocation
	/// @param OutReverseNormal  
	void DetectWall(bool& bOutDetect, FVector& OutHitLocation, FRotator& OutReverseNormal) const;

	/// 장애물을 파악하여 착지 지점 등을 계산
	/// @param DetectLocation 벽을 감지한 위치
	/// @param ReverseNormal
	bool ScanWall(const FVector& DetectLocation, const FRotator& ReverseNormal);

	/// 장애물의 높이를 계산
	void MeasureWall();

	// 
	bool TryInteractWall();

	UFUNCTION()
	void OnVaultMontageStarted(UAnimMontage* Montage);
	
	UFUNCTION()
	void OnVaultMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnVaultMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	
	//
	void PlayAction(const EActions ActionType);

	UFUNCTION()
	void OnStartHangMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnStartHangMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	//
	void TriggerHang();

	//
	void MoveOnWall();

	//
	void TriggerHangingHorizontalMovement();

public:
	UPROPERTY()
	AZipline* TargetZipline = nullptr;
	// Player가 Zipline을 탈 수 있는지 여부
	bool bCanZipping = false;
	FVector ZippingStartPosition = FVector::ZeroVector;
	FVector ZippingEndPosition = FVector::ZeroVector;

	//
	bool TriggerRideZipline();

	UFUNCTION()
	void OnMeleeAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void TriggerMeleeAttack();
	
	UFUNCTION()
	void OnStandingKickMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void TriggerStandingKick();

	UFUNCTION()
	void OnDropkickMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void TriggerDropkick();
	
	void TriggerLandOnFallSafetyZone();

	FVector TargetLocationForFlying = FVector::ZeroVector;
	float FlyingSpeed = 0.0f;
	void FlyingToTarget(float DeltaTime);
};