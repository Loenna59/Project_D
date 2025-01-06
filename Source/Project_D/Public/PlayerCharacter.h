// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputMappingContext* imc_FPS = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_LookUp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Turn = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Move = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Jump = nullptr;

	// 이동 속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 600.0f;

	// 이동 방향
	FVector direction = FVector::ZeroVector;

	// 좌우 회전 입력 처리
	void Turn(const struct FInputActionValue& inputValue);

	// 상하 회전 입력 처리
	void LookUp(const struct FInputActionValue& inputValue);

	// 상하좌우 이동 입력 처리
	void Move(const struct FInputActionValue& inputValue);

	// 점프 입력 처리
	void InputJump(const struct FInputActionValue& inputValue);

	// 플레이어 이동 처리
	void PlayerMove();
};
