// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionComponent.h"

#include "PlayerHelper.h"
#include "Components/CapsuleComponent.h"
#include "MotionWarpingComponent.h"
#include "PlayerAnimInstance.h"
#include "Zipline.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UActionComponent::UActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UActionComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(GetOwner());
	PlayerAnimInstance = Cast<UPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance());
}

// Called every frame
void UActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	const FVector PlayerLocation = Player->GetActorLocation();
	
	if (Player->State == EPlayerState::Zipping)
	{
		bool bNear = true;
		bNear &= UKismetMathLibrary::NearlyEqual_FloatFloat(PlayerLocation.X, ZippingEndPosition.X, 50.0f);
		bNear &= UKismetMathLibrary::NearlyEqual_FloatFloat(PlayerLocation.Y, ZippingEndPosition.Y, 50.0f);
		bNear &= UKismetMathLibrary::NearlyEqual_FloatFloat(PlayerLocation.Z, ZippingEndPosition.Z, 50.0f);
		if (bNear) // 끝 지점에 거의 도달했다면 Player가 짚라인 탑승 상태를 벗어나도록 함
		{
			Player->SetUseControllerRotationYaw(true);
			Player->GetCapsule()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			Player->GetCharacterMovement()->StopMovementImmediately();
			// PlayerAnimInterface->Execute_SetPlayerActionState(EPlayerState::WalkingOnGround);
			
			Player->State = EPlayerState::WalkingOnGround;
			TargetZipline = nullptr;
			ZippingStartPosition = FVector::ZeroVector;
			ZippingEndPosition = FVector::ZeroVector;
		}
		else
		{
			// 짚라인의 끝지점과 플레이어의 위치로 나아가야 할 방향을 구한다.
			const FVector Dir = (ZippingEndPosition - PlayerLocation).GetSafeNormal();
			const FVector P0 = PlayerLocation;
			const FVector VT = 700.0f * DeltaTime * Dir;
			const FVector P = P0 + VT;

			// Player는 Yaw 축으로만 변화시킨다.
			Player->SetActorLocation(P);
			Player->SetActorRotation(FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(P, ZippingEndPosition).Yaw, 0.0f));
		}

		return;
	}

	// 플레이어가 지면에 서 있는지 확인
	FVector StartEnd = PlayerLocation;
	StartEnd.Z = Player->GetBottomZ();
	const TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartEnd,
		StartEnd,
		4.0f,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		bVerboseTick ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);
	bIsOnLand = bHit;
}

bool UActionComponent::TriggerInteractWall()
{
	bool bInteracted = false;
	
	bool bDetect;
	FVector HitLocation;
	FRotator ReverseNormal;
	
	// 정면에 벽이 있는가?
	DetectWall(bDetect, HitLocation, ReverseNormal);
	
	if (bDetect)
	{
		FacedWallTopHitResult.Reset();
		FirstTopHitResult.Reset();
		LastTopHitResult.Reset();
		EndOfObstacleHitResult.Reset();
		VaultLandingHitResult.Reset();

		ScanWall(HitLocation, ReverseNormal);
		if (FirstTopHitResult.bBlockingHit)
		{
			MeasureWall();
			bInteracted = TryInteractWall();
		}
	}

	return bInteracted;
}

void UActionComponent::DetectWall(bool &bOutDetect, FVector &OutHitLocation, FRotator &OutReverseNormal) const
{
	FHitResult OutHit;
	for (int i = 0; i < 8; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector TempVector = UPlayerHelper::MoveVectorUpward(
			UPlayerHelper::MoveVectorDownward(
				Player->GetActorLocation(),
				60.0f
			),
			i * 20.0f
		);
		
		const FVector Start = UPlayerHelper::MoveVectorBackward(
			TempVector,
			Player->GetActorRotation(),
			30.0f
		);
		const FVector End = TempVector + Player->GetActorForwardVector() * 200.0f;
		
		const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			10.0f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			bVerboseDetect ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			OutHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			2.0f
		);

		if (true == bHit)
		{
			bOutDetect = OutHit.bBlockingHit;
			OutHitLocation = OutHit.Location;
			OutReverseNormal = UPlayerHelper::ReverseNormal(OutHit.Normal);
			break;
		}
	}
}

bool UActionComponent::ScanWall(const FVector& DetectLocation, const FRotator& ReverseNormal)
{
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	FVector Start, End;
	bool bHit;
	
	// (300 - i * 10)만큼 위에서 LineTrace를 수행한다.
	// 널널하게 20만큼 뒤에서 시작하여 80만큼 앞에까지 선을 긋는다.
	for (int i = 0; i < 30; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector TempVector = UPlayerHelper::MoveVectorDownward(UPlayerHelper::MoveVectorUpward(DetectLocation, 300.0f), i * 10);
		Start = UPlayerHelper::MoveVectorBackward(TempVector, ReverseNormal, 20.0f);
		End = UPlayerHelper::MoveVectorForward(TempVector, ReverseNormal, 80.0f);
		bHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			Start,
			End,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerboseScan ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			FacedWallTopHitResult,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			2.0f
		);

		// LineTrace가 벽에 충돌하면
		if (true == bHit)
		{
			WallRotation = UPlayerHelper::ReverseNormal(FacedWallTopHitResult.Normal);
			UE_LOG(LogTemp, Warning, TEXT("WallRotation : %s"), *WallRotation.ToString());
			break;
		}
	}
	if (false == FacedWallTopHitResult.bBlockingHit)
	{
		// UE_LOG(LogTemp, Warning, TEXT("false == FacedWallTopHitResult.bBlockingHit"))
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("FacedWallTopImpactPoint : %s"), *FacedWallTopHitResult.ImpactPoint.ToString());
	
	// FacedWallTopHitResult.Location을 기준으로 20씩 전진시켜가며 널널하게 위아래로 10만큼씩 범위로 하여 SphereTrace
	for (int i = 0; i < 10; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector A = UPlayerHelper::MoveVectorForward(FacedWallTopHitResult.Location, WallRotation, i * 20);
		Start = UPlayerHelper::MoveVectorUpward(A, 10.0f);
		End = UPlayerHelper::MoveVectorDownward(Start, 10.0f);
		FHitResult OutHit;
		bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			5.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerboseScan ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			OutHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			2.0f
		);

		if (0 == i)
		{
			if (true == bHit)
			{
				FirstTopHitResult = OutHit;
			}
		}
		else
		{
			if (true == bHit)
			{
				LastTopHitResult = OutHit;
			}
			else
			{
				break;
			}
		}
	}

	if (false == FirstTopHitResult.bBlockingHit || false ==  LastTopHitResult.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("false == FirstTopHitResult.bBlockingHit || false ==  LastTopHitResult.bBlockingHit"))
		return false;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("FirstTopImpactPoint : %s"), *FirstTopHitResult.ImpactPoint.ToString());
	UE_LOG(LogTemp, Warning, TEXT("LastTopImpactPoint : %s"), *LastTopHitResult.ImpactPoint.ToString());

	// LastTopHitResult는 장애물의 정확한 끝 지점이라고 볼 수 없다. (20씩 전진시켜가며 대강 측정한 것이기 때문)
	// LastTopHitResult 기준에서 20만큼 앞에 있는 위치를 시작으로 LastTopHitResult 까지 SphereTrace 하면 정확한 장애물의 끝 지점을 찾을 수 있다.
	Start = UPlayerHelper::MoveVectorForward(LastTopHitResult.ImpactPoint, WallRotation, 20.0f);
	End = LastTopHitResult.ImpactPoint;
	const TArray<AActor*> ActorsToIgnore;
	bHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start,
		End,
		10.0f,
		TraceChannel,
		false,
		ActorsToIgnore,
		bVerboseScan ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		EndOfObstacleHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		2.0f
	);

	if (false == EndOfObstacleHitResult.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("false == EndOfObstacleHitResult.bBlockingHit"))
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("EndOfObstacleImpactPoint : %s"), *EndOfObstacleHitResult.ImpactPoint.ToString());

	// 장애물의 정확한 끝 지점을 찾았다면 해당 지점부터 60만큼 앞에 있는 지점을 기준으로 시작하여 180만큼 아래에 있는 위치까지 SphereTrace
	if (true == bHit)
	{
		Start = UPlayerHelper::MoveVectorForward(EndOfObstacleHitResult.ImpactPoint, WallRotation, 60.0f);
		End = UPlayerHelper::MoveVectorDownward(Start, 180.0f);
		UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			10.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerboseScan ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			VaultLandingHitResult,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			2.0f
		);

		if (true == bHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("VaultLandingImpactPoint : %s"), *VaultLandingHitResult.ImpactPoint.ToString());
		}
	}

	return true;
}

void UActionComponent::MeasureWall()
{
	// Player 전방에 있는 벽의 높이 = 장애물 꼭대기 위치의 Z좌표 - Player의 발 위치 Z좌표
	WallHeight = FirstTopHitResult.ImpactPoint.Z - Player->GetBottomZ();
		
	if (bVerboseMeasure)
	{
		UE_LOG(LogTemp, Warning, TEXT("감지된 장애물의 높이 : %f"), WallHeight);
	}
}

bool UActionComponent::TryInteractWall()
{
	const AActor* Owner = GetOwner();
	const FVector Velocity = Owner->GetVelocity();
	const double SizeXY = UKismetMathLibrary::VSizeXY(Velocity);

	// 캐릭터의 수평속도가 5 이하라면 서 있는 상태라고 판단
	const bool bIsStanding = UKismetMathLibrary::NearlyEqual_FloatFloat(SizeXY, 0.0f, 5.0f);

	// bIsHang && 두 손 짚기
	
	// bIsOnLand && bIsStanding

	// bIsOnLand && Running
	
	// TODO: 각 행동에 필요한 높이 값을 에디터에서 수정할 수 있도록 UPROPERTY 세팅
	if (FirstTopHitResult.bBlockingHit)
	{
		if (WallHeight <= 300.0f)
		{
			if (bIsStanding)
			{
				if (WallHeight > 150.0f)
				{
					if (bVerboseInteract)
					{
						UE_LOG(LogTemp, Warning, TEXT("Climb 동작 수행"));
					}
					TriggerHang();
					return true;
				}
			}
			else
			{
				if (WallHeight <= 140.0f)
				{
					if (WallHeight > 50.0f)
					{
						// One Hand Vault 동작 수행
						if (bVerboseInteract)
						{
							UE_LOG(LogTemp, Warning, TEXT("One Hand Vault 동작 수행"));
						}
						PlayAction(EActions::OneHandVault);
					}
					else
					{
						if (bVerboseInteract)
						{
							UE_LOG(LogTemp, Warning, TEXT("너무 낮은 벽"));
						}
						return false;
					}
					
					return true;
				}
			}
		}
		else
		{
			// 너무 높은 벽
			if (bVerboseInteract)
			{
				UE_LOG(LogTemp, Warning, TEXT("너무 높은 벽"));
			}
		}
	}

	return false;
}

void UActionComponent::OnVaultMontageStarted(UAnimMontage* Montage)
{
	UE_LOG(LogTemp, Display, TEXT("UActionComponent::OnVaultMontageStarted"));
	
	if (bVerboseMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Vault Start Location : %s"), *Player->GetMesh()->GetComponentLocation().ToString());
	}
	// 장애물과의 상호작용 액션이 동작하는 중에 충돌 처리를 하지 않도록 함
	Player->GetCapsule()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	PlayerAnimInstance->OnMontageStarted.RemoveDynamic(this, &UActionComponent::OnVaultMontageStarted);
}

void UActionComponent::OnVaultMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Display, TEXT("UActionComponent::OnVaultMontageBlendingOut"));
	
	if (bVerboseMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Vault BlendingOut Location : %s"), *Player->GetMesh()->GetComponentLocation().ToString());
	}
	PlayerAnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UActionComponent::OnVaultMontageBlendingOut);
}

void UActionComponent::OnVaultMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Display, TEXT("UActionComponent::OnVaultMontageEnded"));
	
	if (bVerboseMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Vault Ended Location : %s"), *Player->GetMesh()->GetComponentLocation().ToString());
	}
	bCanAction = true;
	Player->GetCapsule()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	PlayerAnimInstance->OnMontageEnded.RemoveDynamic(this, &UActionComponent::OnVaultMontageEnded);
}

void UActionComponent::PlayAction(const EActions ActionType)
{
	UE_LOG(LogTemp, Display, TEXT("UActionComponent::PlayAction"));

	UMotionWarpingComponent* MotionWarping = Player->MotionWarpingComponent;
	
	bCanAction = false;
	
	UAnimMontage* AnimMontage = nullptr;
	FVector Start = FVector::ZeroVector, End = FVector::ZeroVector;
	
	switch (ActionType)
	{
	case EActions::OneHandVault:
		// Set Montage

		AnimMontage = OneHandVault;
		// Set Montage Delegate
		PlayerAnimInstance->OnMontageStarted.AddDynamic(this, &UActionComponent::OnVaultMontageStarted);
		PlayerAnimInstance->OnMontageEnded.AddDynamic(this, &UActionComponent::OnVaultMontageEnded);
		PlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UActionComponent::OnVaultMontageBlendingOut);

		// Set Motion Warping
		Start = FirstTopHitResult.Location;
		End = VaultLandingHitResult.Location;
		MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("ActionStart"), Start, WallRotation);
		MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("ActionEnd"), End, WallRotation);
		break;
	}

	if (bVerboseMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start : %s, End : %s"), *Start.ToString(), *End.ToString());
	}
	
	PlayerAnimInstance->Montage_Play(AnimMontage);
}

void UActionComponent::OnStartHangMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Display, TEXT("UActionComponent::OnStartHangMontageBlendingOut"));
	PlayerAnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UActionComponent::OnStartHangMontageBlendingOut);
	
	Player->State = EPlayerState::Hanging;
	Player->GetCharacterMovement()->StopMovementImmediately();
	
}

void UActionComponent::OnStartHangMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Display, TEXT("UActionComponent::OnStartHangMontageEnded"));
	PlayerAnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UActionComponent::OnStartHangMontageEnded);

	const FVector TargetLocation = UPlayerHelper::MoveVectorDownward(
		UPlayerHelper::MoveVectorBackward(FirstTopHitResult.ImpactPoint, WallRotation, 18.0f),
		103.0f
	);
	Player->SetActorLocationAndRotation(TargetLocation, WallRotation);
}

void UActionComponent::TriggerHang()
{
	UE_LOG(LogTemp, Display, TEXT("UActionComponent::TriggerHang"));
	bCanAction = false;
	// Hang Idle 중에는 마우스 움직임이 발생해도 회전하지 않도록 합니다. (카메라만 회전)
	Player->SetUseControllerRotationYaw(false);
	Player->GetCapsule()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	Player->GetCharacterMovement()->StopMovementImmediately();

	const FVector TargetLocation = UPlayerHelper::MoveVectorDownward(
		UPlayerHelper::MoveVectorBackward(FirstTopHitResult.ImpactPoint, WallRotation, 18.0f),
		103.0f
	);
	Player->MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("HangEnd"), TargetLocation, WallRotation);
	
	PlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UActionComponent::OnStartHangMontageBlendingOut);
	PlayerAnimInstance->OnMontageEnded.AddDynamic(this, &UActionComponent::OnStartHangMontageEnded);
	PlayerAnimInstance->Montage_Play(IdleToHang);
}

void UActionComponent::MoveOnWall()
{
	if (false == PlayerAnimInstance->IsAnyMontagePlaying())
	{
		TriggerHangingHorizontalMovement();
	}
	else
	{
		Player->GetCharacterMovement()->StopMovementImmediately();
	}
}

void UActionComponent::TriggerHangingHorizontalMovement()
{
	UCapsuleComponent* Capsule = Player->GetCapsule();
	UCharacterMovementComponent* Movement = Player->GetCharacterMovement();
	const FVector WorldLocation = Capsule->GetComponentLocation();
	const FRotator WorldRotation = Capsule->GetComponentRotation();
	
	const TArray<AActor*> ActorsToIgnore;
	for (int i = 0; i < 3; i++)
	{
		const FVector Start = UPlayerHelper::MoveVectorDownward(
			UPlayerHelper::MoveVectorRight(
				UPlayerHelper::MoveVectorUpward(WorldLocation, 120.0f),
				WorldRotation,
				Player->MovementVector.X * 10.0f
			),
			i * 10
		);

		const FVector End = UPlayerHelper::MoveVectorForward(
			Start,
			WorldRotation,
			60.0f
		);

		const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			5.0f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration, // TODO: Verbose
			WallHitResultForHangingHorizontalMove,
			true
		);

		if (true == bHit)
		{
			WallRotation = UPlayerHelper::ReverseNormal(WallHitResultForHangingHorizontalMove.ImpactNormal);
			break;
		}
		Movement->StopMovementImmediately();
	}
	
	const FVector Start = UPlayerHelper::MoveVectorUpward(
		UPlayerHelper::MoveVectorForward(WallHitResultForHangingHorizontalMove.ImpactPoint, WallRotation, 2.0f),
		20.0f
	);
	const FVector End = UPlayerHelper::MoveVectorDownward(Start, 100.0f);
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start,
		End,
		5.0f,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration, // TODO: Verbose
		WallHitResultForHangingHorizontalMove,
		true,
		FLinearColor::Black,
		FLinearColor::Blue,
		5.0f
	);
	
	if (true == WallHitResultForHangingHorizontalMove.bStartPenetrating)
	{
		UE_LOG(LogTemp, Warning, TEXT("1"));
		// 시작 지점이 이미 다른 Mesh에 의해 Overlap 되어 있는 경우 즉시 Player의 움직임을 멈춘다.
		Movement->StopMovementImmediately();
	}
	else if (true == WallHitResultForHangingHorizontalMove.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("2"));
		Movement->StopMovementImmediately();

		const FVector ImpactPoint = WallHitResultForHangingHorizontalMove.ImpactPoint;
		const FVector Current = Capsule->GetComponentLocation();
		const FVector Target = UPlayerHelper::MoveVectorBackward(ImpactPoint, WallRotation, 35.0f);
		const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
		const float X = UKismetMathLibrary::FInterpTo(Current.X, Target.X, DeltaSeconds, 5.0f);
		const float Y = UKismetMathLibrary::FInterpTo(Current.Y, Target.Y, DeltaSeconds, 5.0f);
		const FVector NewLocation = FVector(X, Y, ImpactPoint.Z - 107.0f);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *NewLocation.ToString());
		Capsule->SetWorldLocationAndRotation(NewLocation, WallRotation);
	}
}

bool UActionComponent::TriggerRideZipline()
{
	// 근처에 Zipline이 없다면
	if (false == bCanZipping || nullptr == TargetZipline)
	{
		return false;
	}
	
	bCanZipping = false;
	// Zipline을 타는 중에는 마우스 움직임이 발생해도 회전하지 않도록 합니다. (카메라만 회전)
	Player->SetUseControllerRotationYaw(false);
	Player->GetCapsule()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	Player->GetCharacterMovement()->StopMovementImmediately();
	// PlayerAnimInterface->Execute_SetPlayerActionState(PlayerAnimInstance, EPlayerState::Zipping);
	
	Player->State = EPlayerState::Zipping;
	
	ZippingStartPosition = UPlayerHelper::MoveVectorDownward(TargetZipline->StartCablePosition->GetComponentLocation(), 100.0f);
	ZippingEndPosition = UPlayerHelper::MoveVectorDownward(TargetZipline->EndCablePosition->GetComponentLocation(), 100.0f);
	
    Player->SetActorLocation(ZippingStartPosition);

	return true;
}

void UActionComponent::OnMeleeAttackMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	Player->bIsAttacking = false;
	PlayerAnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UActionComponent::OnMeleeAttackMontageBlendingOut);
}

void UActionComponent::TriggerMeleeAttack() 
{
	PlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UActionComponent::OnMeleeAttackMontageBlendingOut);
	PlayerAnimInstance->Montage_Play(MeleeAttack);
}

void UActionComponent::TriggerStandingKick()
{
	PlayerAnimInstance->Montage_Play(StandingKick);
}

void UActionComponent::TriggerDropkick()
{
	PlayerAnimInstance->Montage_Play(Dropkick);
}

void UActionComponent::TriggerLandOnFallSafetyZone()
{
	PlayerAnimInstance->Montage_Play(LandOnFallSafetyZone);
}
