// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionWarpingComponent.h"
#include "ActionComponent.h"
#include "BaseZombie.h"
#include "BlankTriggerParam.h"
#include "FallSafetyZone.h"
#include "GameDebug.h"
#include "PlayerHUD.h"
#include "ZombieTriggerParam.h"
#include "Blueprint/UserWidget.h"
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

	// Weapon Mesh Attach & Bind Event
	WeaponMesh->AttachToComponent(Super::GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "RightHandWeaponSocket");
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

	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::Landed 착지 당시 속도 : %s"), *GetVelocity().ToString());

	// 착지 당시 속도를 선형 보간 하여 낙사 데미지를 구한다.
	// 떨어질 당시의 속도(GetVelocity().Z에 부호 반전) -> 데미지
	// >= 1000 -> 0
	// >= 1500 -> 50
	// >= 2000 -> 100
	const float Damage = FMath::GetMappedRangeValueClamped(
		FVector2D(1000.0f, 2000.0f),
		FVector2D(0.0f, 100.0f),
		-GetVelocity().Z
	);

	// 만약 낙사 데미지를 받아야 할 상황이라면
	if (Damage > 0)
	{
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
		
		if (bSafe)
		{
			// 만약, Fall Safety Zone이라면, 낙사 데미지를 면제하고 안착하는 애니메이션을 재생한다.
			UE_LOG(LogTemp, Warning, TEXT("Fall Safety Zone 진입으로 낙사 데미지 면제"));
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
				EDrawDebugTrace::ForDuration,
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
	//UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::OnDamaged(%d)"), Amount);

	GameDebug::ShowDisplayLog(GetWorld(), FString::Printf(TEXT("APlayerCharacter::OnDamaged(%d)"), Amount), true);
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
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::OnDead"));

	bIsDead = true;
	OnChangePerspective();
	
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
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::StartedKick"));

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