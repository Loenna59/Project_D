// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "PlayerInterface.h"
#include "Zipline.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UPlayerHUD;
class UMotionWarpingComponent;
class UCameraComponent;
class UActionComponent;
class UInputAction;
class UInputMappingContext;

UENUM()
enum class EPlayerState : uint8
{
	WalkingOnGround,
	Hanging,
	Zipping
};

UCLASS()
class PROJECT_D_API APlayerCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
// Implement IPlayerInterface
public:
	virtual USkeletalMeshComponent* GetMesh() override;
	virtual UCapsuleComponent* GetCapsule() override;
	virtual UCharacterMovementComponent* GetCharacterMovement() override;
	virtual UMotionWarpingComponent* GetMotionWarping() override;
	virtual float GetBottomZ() override;

	virtual void SetUseControllerRotationPitch(const bool& bUse) override;
	virtual void SetUseControllerRotationYaw(const bool& bUse) override;
	virtual void SetUseControllerRotationRoll(const bool& bUse) override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerHUD> PlayerHUDFactory = nullptr;

	UPROPERTY()
	UPlayerHUD* PlayerHUD = nullptr;
	int Hp = 100;
	int MaxHp = 100;
	
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputMappingContext* ImcFPS = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IaLookUp = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IaTurn = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IaMove = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IaJump = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IaSprint = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IaAttack = nullptr;

	UPROPERTY()
	EPlayerState State = EPlayerState::WalkingOnGround;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* WeaponMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UActionComponent* ActionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMotionWarpingComponent* MotionWarpingComponent = nullptr;
	
	// 이동 방향
	FVector Direction = FVector::ZeroVector;
	FVector2D MovementVector = FVector2d::ZeroVector;

	// 좌우 회전 입력 처리
	void TriggeredTurn(const FInputActionValue& InputValue);

	// 상하 회전 입력 처리
	void TriggeredLookUp(const FInputActionValue& InputValue);

	// 상하좌우 이동 입력 처리
	void TriggeredMove(const FInputActionValue& InputValue);
	void CompletedMove();

	// 점프 입력 처리
	void StartedJump();

	// 공격 입력 처리
	void StartedAttack();
	
	void TriggeredSprint();
	void CompletedSprint();
	
	// 서 있는 상태에서 플레이어 이동 처리
	void MoveOnGround();
	
	void OnZiplineBeginOverlap(AZipline* InZipline);
	void OnZiplineEndOverlap(const AZipline* InZipline);

	UFUNCTION()
	void OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void OnDamaged(int Amount);
	void OnDead();
};
