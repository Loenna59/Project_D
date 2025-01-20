// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionWarpingComponent.h"
#include "ActionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// true일 때 마우스 움직임에 따라 Player의 Yaw 축이 따라 움직인다.
	bUseControllerRotationYaw = true;

	ActionComponent = CreateDefaultSubobject<UActionComponent>(TEXT("ActionComponent"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance
	(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Player/ABP_Player.ABP_Player_C'"));
	if (AnimInstance.Class)
	{
		Super::GetMesh()->SetAnimInstanceClass(AnimInstance.Class);
	}
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
		PlayerInput->BindAction(IaMove, ETriggerEvent::Completed, this, &APlayerCharacter::CompletedMove);
		PlayerInput->BindAction(IaJump, ETriggerEvent::Triggered, this, &APlayerCharacter::TriggeredJump);
		PlayerInput->BindAction(IaSprint, ETriggerEvent::Triggered, this, &APlayerCharacter::TriggeredSprint);
		PlayerInput->BindAction(IaSprint, ETriggerEvent::Completed, this, &APlayerCharacter::CompletedSprint);
		PlayerInput->BindAction(IaAttack, ETriggerEvent::Triggered, this, &APlayerCharacter::TriggeredAttack);
	}
}

USkeletalMeshComponent* APlayerCharacter::GetMesh()
{
	return Super::GetMesh();
}

UCapsuleComponent* APlayerCharacter::GetCapsule()
{
	return Super::GetCapsuleComponent();
}

UCharacterMovementComponent* APlayerCharacter::GetCharacterMovement()
{
	return Super::GetCharacterMovement();
}

UMotionWarpingComponent* APlayerCharacter::GetMotionWarping()
{
	return MotionWarpingComponent;
}

float APlayerCharacter::GetBottomZ()
{
	return Super::GetCharacterMovement()->GetFeetLocation().Z;
}

void APlayerCharacter::SetUseControllerRotationPitch(const bool& bUse)
{
	bUseControllerRotationPitch = bUse;
}

void APlayerCharacter::SetUseControllerRotationYaw(const bool& bUse)
{
	bUseControllerRotationYaw = bUse;
}

void APlayerCharacter::SetUseControllerRotationRoll(const bool& bUse)
{
	bUseControllerRotationRoll = bUse;
}

void APlayerCharacter::MoveOnGround(const FVector2D& MovementVector)
{
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
	// add movement 
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlayerCharacter::OnZiplineBeginOverlap(AZipline* InZipline)
{
	if (ActionComponent->PlayerActionState == EActionState::Zipping)
	{
		return;
	}
	
	ActionComponent->bCanZipping = true;
	ActionComponent->TargetZipline = InZipline;
}

void APlayerCharacter::OnZiplineEndOverlap(const AZipline* InZipline)
{
	if (ActionComponent->TargetZipline == InZipline)
	{
		ActionComponent->bCanZipping = false;
	}
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
	const FVector2D MovementVector = InputValue.Get<FVector2D>();

	if (Controller)
	{
		switch (ActionComponent->PlayerActionState)
		{
		case EActionState::WalkingOnGround:
			MoveOnGround(MovementVector);
			break;
		case EActionState::Climbing:
			ActionComponent->MoveOnWall(MovementVector);
			break;
		case EActionState::Zipping:
			break;
		}
	}
}

void APlayerCharacter::CompletedMove(const FInputActionValue& InputActionValue)
{
	ActionComponent->ResetMoveValue();
}

void APlayerCharacter::TriggeredJump()
{
	// Zipline 탑승을 시도 해본다.
	if (true == ActionComponent->TriggerRideZipline())
	{
		return;
	}

	// 벽에 매달리거나 넘는 것을 시도 해본다.
	if (true == ActionComponent->bCanInteract)
	{
		if (true == ActionComponent->TriggerInteractWall())
		{
			return;
		}
	}

	if (EActionState::Climbing == ActionComponent->PlayerActionState)
	{
		if (true == ActionComponent->bCanClimbing)
		{
			ActionComponent->TriggerClimb();
		}
		return;
	}
	
	// 아무것도 안 했으면 그냥 점프를 한다.
	Jump();
}

void APlayerCharacter::TriggeredAttack()
{
	ActionComponent->TriggerMeleeAttack();
}

void APlayerCharacter::TriggeredSprint()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = 600.0f;
}

void APlayerCharacter::CompletedSprint()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = 300.0f;
}