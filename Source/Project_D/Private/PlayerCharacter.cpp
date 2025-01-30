// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionWarpingComponent.h"
#include "ActionComponent.h"
#include "BaseZombie.h"
#include "BlankTriggerParam.h"
#include "FallSafetyZone.h"
#include "PlayerHUD.h"
#include "ZombieTriggerParam.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/GameOverUI.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// true일 때 마우스 움직임에 따라 Player의 Yaw 축이 따라 움직인다.
	bUseControllerRotationYaw = true;
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset
	(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/Player/Character/character.character'"));
	if (SkeletalMeshAsset.Object)
	{
		Super::GetMesh()->SetSkeletalMeshAsset(SkeletalMeshAsset.Object);
	}
	
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->bUsePawnControlRotation = true;
	PlayerCamera->SetFieldOfView(100.0f);              
	
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

	// HUD
	PlayerHUD = Cast<UPlayerHUD>(CreateWidget(GetWorld(), PlayerHUDFactory));
    check(PlayerHUD);
    if (PlayerHUD)
    {
    	PlayerHUD->AddToViewport();
    }

	// Bind Event
	WeaponMesh->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnWeaponBeginOverlap);

	// Add Input Mapping Context
	if (const auto PlayerController = Cast<APlayerController>(Controller))
	{
		if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ImcFPS, 0);
		}
	}

	// Movement Init
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = 300.0f;
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// 착지 직전의 낙하 속도를 가지고 낙사 데미지를 구한다.
	const float DownwardSpeed = -GetVelocity().Z;
	// 만약, MinHardFallVelocity = 1000 & MaxSurviveFallVelocity = 2000 라면,
	// >= 2000 -> 100
	// >= 1500 -> 50
	// >= 1000 -> 0
	const float Damage = FMath::GetMappedRangeValueClamped(
		FVector2D(MinHardFallVelocity, MaxSurviveFallVelocity),
		FVector2D(0.0f, 100.0f),
		DownwardSpeed
	);

	// 만약 낙사 데미지를 받아야 할 상황이라면
	if (Damage > 0)
	{
#pragma region 떨어진 위치에 SphereOverlapActors
		// 떨어진 위치가 Fall Safety Zone은 아닌지 확인한다.
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		UClass* ActorClassFilter = AFallSafetyZone::StaticClass();
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Init(this, 1);
		TArray<AActor*> OutActors;
		const bool bSafe = UKismetSystemLibrary::SphereOverlapActors(
			this,
			Hit.ImpactPoint,
			30.0f,
			ObjectTypes,
			ActorClassFilter,
			ActorsToIgnore,
			OutActors
		);
#pragma endregion
		
		if (bSafe)
		{
			// 만약, Fall Safety Zone이라면, 낙사 데미지를 면제한다.
			UE_LOG(LogTemp, Warning, TEXT("Fall Safety Zone 진입으로 낙사 데미지 면제"));
			// 안착하는 애니메이션을 재생한다.
			ActionComponent->TriggerLandOnFallSafetyZone();
		}
		else
		{
			// 그렇지 않다면 낙사 데미지를 온전히 받는다.
			UE_LOG(LogTemp, Warning, TEXT("낙사 데미지 : %f"), Damage);
			// 데미지 처리
			OnDamaged(Damage);
		}
	}
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
		PlayerInput->BindAction(IaJump, ETriggerEvent::Started, this, &APlayerCharacter::StartedJump);
		PlayerInput->BindAction(IaSprint, ETriggerEvent::Triggered, this, &APlayerCharacter::TriggeredSprint);
		PlayerInput->BindAction(IaSprint, ETriggerEvent::Completed, this, &APlayerCharacter::CompletedSprint);
		PlayerInput->BindAction(IaAttack, ETriggerEvent::Started, this, &APlayerCharacter::StartedAttack);
		PlayerInput->BindAction(IaKick, ETriggerEvent::Started, this, &APlayerCharacter::StartedKick);
		PlayerInput->BindAction(IaEquipment, ETriggerEvent::Started, this, &APlayerCharacter::StartedEquipment);
	}
}

float APlayerCharacter::GetBottomZ() const
{
	return Super::GetCharacterMovement()->GetFeetLocation().Z;
}

void APlayerCharacter::SetFlyingMode(const EPlayerState InState)
{
	GetCharacterMovement()->StopMovementImmediately(); // 움직임을 즉시 멈춘다
	SetUseControllerRotationYaw(false); // 마우스 움직임이 발생해도 Player를 회전시키지 않는다
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌의 영향을 받지 않도록 한다
	GetCharacterMovement()->SetMovementMode(MOVE_Flying); // MovementMode를 변경한다
	State = InState;
}

void APlayerCharacter::SetWalkingMode()
{
	GetCharacterMovement()->StopMovementImmediately();
	SetUseControllerRotationYaw(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	State = EPlayerState::WalkingOnGround;
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

void APlayerCharacter::MoveOnGround()
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
	if (EPlayerState::Zipping == State)
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

void APlayerCharacter::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 공격 중이 아닐 때는 처리하지 않음
	if (false == bIsAttacking)
	{
		return;
	}
	
	if (ICollisionTrigger* Trigger = Cast<ICollisionTrigger>(OtherActor))
	{
		if (ABaseZombie* Zombie = Cast<ABaseZombie>(OtherActor))
		{
			TArray<AActor*> ActorsToIgnore;
			FHitResult HitResult;
			const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				WeaponMesh->GetComponentLocation(),
				WeaponMesh->GetComponentLocation(),
				30.0f,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true
			);
			
			if (bHit)
			{
				AActor* HitActor = HitResult.GetActor();
				if (HitActor)
				{
					if (ICollisionTrigger* Trigger2 = Cast<ICollisionTrigger>(HitActor))
					{
						if (ABaseZombie* Zombie2 = Cast<ABaseZombie>(HitActor))
						{
							AZombieTriggerParam* Param = NewObject<AZombieTriggerParam>();
							Param->Damage = 50;
							Param->HitBoneName = HitResult.BoneName;
									
							Trigger2->OnTriggerEnter(HitActor, Param);
						}
						else
						{
							ABlankTriggerParam* Param = NewObject<ABlankTriggerParam>();
							Trigger2->OnTriggerEnter(HitActor, Param);
						}
					}
				}
			}
		}
	}
}

void APlayerCharacter::OnDamaged(const int Amount)
{
	//UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::OnDamaged(%d)"), Amount);
	Hp -= Amount;
	PlayerHUD->OnChangeHp(Hp, MaxHp);
	if (Hp <= 0)
	{
		OnDead();
	}

	// 피격 애니메이션 재생
}

void APlayerCharacter::OnDead()
{
	UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::OnDead"));

	bIsDead = true;
	
	// 흑백 화면 처리
	APostProcessVolume* PostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass());
	if (PostProcessVolume)
	{
		PostProcessVolume->bUnbound = true;
		PostProcessVolume->Settings.bOverride_ColorSaturation = true;
		PostProcessVolume->Settings.ColorSaturation = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

		// GameOverUI 생성 (아직 표시 X)
		UGameOverUI* GameOverUI = Cast<UGameOverUI>(CreateWidget(GetWorld(), GameOverUIFactory));

		// 3초 뒤에...
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[PostProcessVolume, GameOverUI]()
			{
				// 흑백 효과 삭제
				if (PostProcessVolume && PostProcessVolume->IsValidLowLevel())
				{
					PostProcessVolume->Destroy();
				}

				if (GameOverUI)
				{
					GameOverUI->AddToViewport();
				}
			},
			3.0f,
			false
		);
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
	MovementVector = InputValue.Get<FVector2D>();
	if (true == bIsHardLanding || true == bIsDead)
	{
		return;
	}
	
	if (Controller)
	{
		switch (State)
		{
		case EPlayerState::WalkingOnGround:
			MoveOnGround();
			break;
		case EPlayerState::Hanging:
			ActionComponent->MoveOnWall();
			break;
		case EPlayerState::Zipping:
			break;
		}
	}
}

void APlayerCharacter::CompletedMove()
{
	MovementVector = FVector2D::ZeroVector;
}

void APlayerCharacter::StartedJump()
{
	// Zipline 탑승을 시도 해본다.
	if (true == ActionComponent->TriggerRideZipline())
	{
		return;
	}

	// 벽에 매달리거나 넘는 것을 시도 해본다.
	if (true == ActionComponent->bCanAction)
	{
		if (true == ActionComponent->TriggerInteractWall())
		{
			return;
		}
	}
	
	// 아무것도 안 했으면 그냥 점프를 한다.
	Jump();
}

void APlayerCharacter::StartedAttack()
{
	if (true == bIsAttacking)
	{
		return;
	}
	
	ActionComponent->TriggerMeleeAttack();
}

void APlayerCharacter::StartedKick()
{
	UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::StartedKick"));

	if (true == bIsKicking)
	{
		return;
	}
	
	if (true == ActionComponent->bIsOnLand)
	{
		ActionComponent->TriggerStandingKick();
		return;
	}
	
	if (false == ActionComponent->bIsOnLand && GetVelocity().Z > -350.0f)
	{
		ActionComponent->TriggerDropkick();
		return;
	}
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

void APlayerCharacter::StartedEquipment()
{
	UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::StartedEquipment"));

	// 우선 Equipment는 Grappling Hook 밖에 없다고 가정한다.
	// 1. 화면 중앙(크로스헤어 위치)의 방향 벡터를 알고 싶다.
	// 2. 해당 방향으로 특정 거리 만큼 LineTrace를 하고 싶다.

	const APlayerController* PlayerController = GetLocalViewingPlayerController();
	
	// 화면 크기를 구한다.
	int32 ScreenWidth, ScreenHeight;
	PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);
	FVector WorldLocation, WorldDirection;

	// 현재 활성화된 ViewTarget(Player에 부착된 카메라)의 정중앙 위치가 월드좌표상에서 어떤 방향인지 구한다.
	// WorldLocation : ViewTarget의 월드 좌표 (PlayerCamara의 월드 좌표)
	// WorldDirection : ViewTarget 중앙이 가리키는 방향
	if (PlayerController->DeprojectScreenPositionToWorld(ScreenWidth * 0.5f, ScreenHeight * 0.5f, WorldLocation, WorldDirection))
	{
		const FVector Start = WorldLocation;  // 카메라 위치가 시작지점
		const FVector End = Start + (WorldDirection * 1000.0f);  // 화면 중앙 방향으로 1000 만큼

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);  // 자신의 충돌 무시 (필요시)

		// Object Type이 ECC_GameTraceChannel6(Wall)인 오브젝트들만 대상으로 LineTrace
		if (const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, ECC_GameTraceChannel6, QueryParams))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
			SetFlyingMode(EPlayerState::Zipping);

			ActionComponent->TargetLocationForFlying = HitResult.ImpactPoint;
			ActionComponent->FlyingSpeed = 1000.0f;
		}
		if (true == bVerboseEquipment)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	}
}