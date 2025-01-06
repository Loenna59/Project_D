// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_FPS, 0);
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 플레이어 이동 처리 (등속 운동)
	direction = FTransform(GetControlRotation()).TransformVector(direction); // 월드 좌표가 아닌 상대 좌표로 방향 설정
	// FVector P0 = GetActorLocation(); // 플레이어의 현재 위치
	// FVector vt = (walkSpeed * direction) * DeltaTime; // (어느 방향으로 어느 정도의 속도로) * 시간
	// FVector P = P0 + vt;
	// SetActorLocation(P);
	AddMovementInput(direction); // 대신 Character Movement 컴포넌트의 기능 사용
	direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &APlayerCharacter::Turn);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &APlayerCharacter::LookUp);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	}
}

void APlayerCharacter::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value);
}

void APlayerCharacter::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value);
}

void APlayerCharacter::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();

	// 상하 입력 이벤트 처리
	direction.X = value.X;

	// 좌우 입력 이벤트 처리
	direction.Y = value.Y;
}