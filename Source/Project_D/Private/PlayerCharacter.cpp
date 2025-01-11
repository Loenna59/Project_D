// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ObstacleSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;

	// 1인칭 카메라 컴포넌트 세팅
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCameraComponent"));
	PlayerCameraComponent->SetupAttachment(RootComponent);
	PlayerCameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
	PlayerCameraComponent->bUsePawnControlRotation = true;

	// 1인칭 스켈레탈 메시 컴포넌트 세팅
	PlayerSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerSkeletalMeshComponent"));
	PlayerSkeletalMeshComponent->SetupAttachment(PlayerCameraComponent);
	PlayerSkeletalMeshComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, -160.0f));
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT(
		"/Script/Engine.SkeletalMesh'/Game/StarterContent/FirstPersonPrototypes/FirstPersonArms/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
	if (SkeletalMeshAsset.Succeeded())
	{
		PlayerSkeletalMeshComponent->SetSkeletalMeshAsset(SkeletalMeshAsset.Object);
	}
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimClass(TEXT(
		"/Script/Engine.AnimBlueprint'/Game/StarterContent/FirstPersonPrototypes/FirstPersonArms/Animations/FirstPerson_AnimBP.FirstPerson_AnimBP_C'"));
	if (AnimClass.Succeeded())
	{
		PlayerSkeletalMeshComponent->AnimClass = AnimClass.Class;
	}
	PlayerSkeletalMeshComponent->CastShadow = false;
	
	ObstacleSystemComponent = CreateDefaultSubobject<UObstacleSystemComponent>(TEXT("ObstacleSystemComponent"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const auto PlayerController = Cast<APlayerController>(Controller))
	{
		if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ImcFPS, 0);
		}
	}

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = 300.0f;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlayerMove();
}

void APlayerCharacter::PlayerMove()
{
	// 플레이어 이동 처리 (등속 운동)
	Direction = FTransform(GetControlRotation()).TransformVector(Direction); // 월드 좌표가 아닌 상대 좌표로 방향 설정
	// FVector P0 = GetActorLocation(); // 플레이어의 현재 위치
	// FVector vt = (walkSpeed * direction) * DeltaTime; // (어느 방향으로 어느 정도의 속도로) * 시간
	// FVector P = P0 + vt;
	// SetActorLocation(P);
	AddMovementInput(Direction); // 대신 Character Movement 컴포넌트의 기능 사용
	Direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		PlayerInput->BindAction(IaTurn, ETriggerEvent::Triggered, this, &APlayerCharacter::TriggeredTurn);
		PlayerInput->BindAction(IaLookUp, ETriggerEvent::Triggered, this, &APlayerCharacter::TriggeredLookUp);
		PlayerInput->BindAction(IaMove, ETriggerEvent::Triggered, this, &APlayerCharacter::TriggeredMove);
		PlayerInput->BindAction(IaJump, ETriggerEvent::Triggered, this, &APlayerCharacter::TriggeredJump);
		PlayerInput->BindAction(IaSprint, ETriggerEvent::Triggered, this, &APlayerCharacter::TriggeredSprint);
		PlayerInput->BindAction(IaSprint, ETriggerEvent::Completed, this, &APlayerCharacter::CompletedSprint);
	}
}

USkeletalMeshComponent* APlayerCharacter::GetMesh()
{
	return PlayerSkeletalMeshComponent;
}

UCapsuleComponent* APlayerCharacter::GetCapsule()
{
	return GetCapsuleComponent();
}

float APlayerCharacter::GetBottomZ()
{
	return GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void APlayerCharacter::TriggeredTurn(const FInputActionValue& InputValue)
{
	const float Val = InputValue.Get<float>();
	AddControllerYawInput(Val);
}

void APlayerCharacter::TriggeredLookUp(const FInputActionValue& InputValue)
{
	const float Val = InputValue.Get<float>();
	AddControllerPitchInput(Val);
}

void APlayerCharacter::TriggeredMove(const FInputActionValue& InputValue)
{
	const FVector2D Val = InputValue.Get<FVector2D>();
	Direction.X = Val.X; // 상하 입력 이벤트 처리
	Direction.Y = Val.Y; // 좌우 입력 이벤트 처리
}

void APlayerCharacter::TriggeredJump()
{
	Jump(); // Character Classd의 Jump 기능 호출
}

void APlayerCharacter::TriggeredSprint()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = 600.0f;
	ObstacleSystemComponent->TriggerInteractObstacle();
}

void APlayerCharacter::CompletedSprint()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = 300.0f;
}