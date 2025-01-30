// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Zipline.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UGameOverUI;
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
class PROJECT_D_API APlayerCharacter : public ACharacter
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
	

public:
	float GetBottomZ() const;

	void SetUseControllerRotationPitch(const bool& bUse);
	void SetUseControllerRotationYaw(const bool& bUse);
	void SetUseControllerRotationRoll(const bool& bUse);

public:
	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* PlayerCamera = nullptr;

	// Actor Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UActionComponent* ActionComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMotionWarpingComponent* MotionWarpingComponent = nullptr;
	
	// Widget
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerHUD> PlayerHUDFactory = nullptr;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameOverUI> GameOverUIFactory = nullptr;
	UPROPERTY()
	UPlayerHUD* PlayerHUD = nullptr;

	// Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* ImcFPS = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IaLookUp = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IaTurn = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IaMove = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IaJump = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IaSprint = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IaAttack = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IaKick = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Equipment")
	UInputAction* IaEquipment = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Equipment")
	bool bVerboseEquipment = false;
	FVector Direction = FVector::ZeroVector;
	FVector2D MovementVector = FVector2d::ZeroVector;
	void TriggeredTurn(const FInputActionValue& InputValue);
	void TriggeredLookUp(const FInputActionValue& InputValue);
	void TriggeredMove(const FInputActionValue& InputValue);
	void CompletedMove();
	void StartedJump();
	void StartedAttack();
	void StartedKick();
	void TriggeredSprint();
	void CompletedSprint();
	void StartedEquipment();
	void MoveOnGround();

	// States
	UPROPERTY()
	EPlayerState State = EPlayerState::WalkingOnGround;
	int Hp = 100;
	int MaxHp = 100;
	bool bIsHardLanding = false;
	bool bIsDead = false;
	bool bIsAttacking = false;
	bool bIsKicking = false;
	void OnDamaged(int Amount);
	void OnDead();

	// Etc
	UPROPERTY(EditDefaultsOnly)
	float MinHardFallVelocity = 1000.0f;
	UPROPERTY(EditDefaultsOnly)
	float MaxSurviveFallVelocity = 2000.0f;
	
	void OnZiplineBeginOverlap(AZipline* InZipline);
	void OnZiplineEndOverlap(const AZipline* InZipline);
	UFUNCTION()
	void OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
