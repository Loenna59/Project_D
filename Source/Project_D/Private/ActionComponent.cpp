// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionComponent.h"

#include "PlayerHelper.h"
#include "PlayerInterface.h"
#include "Components/CapsuleComponent.h"
#include "MotionWarpingComponent.h"
#include "PlayerAnimBlueprintInterface.h"
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

	Initialize();
}

void UActionComponent::Initialize()
{
	PlayerInterface = Cast<IPlayerInterface>(GetOwner());
	if (PlayerInterface)
	{
		PlayerMesh = PlayerInterface->GetMesh();
		PlayerCapsule = PlayerInterface->GetCapsule();
		PlayerMovement = PlayerInterface->GetCharacterMovement();
		PlayerMotionWarping = PlayerInterface->GetMotionWarping();
		PlayerAnimInstance = PlayerMesh->GetAnimInstance();
		PlayerAnimInterface = Cast<IPlayerAnimBlueprintInterface>(PlayerAnimInstance);
		if (PlayerAnimInterface)
		{
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerAnimInterface is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInterface is nullptr"));
	}
}


// Called every frame
void UActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TODO: 종속성
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	const FVector PlayerLocation = Player->GetActorLocation();
	
	if (PlayerActionState == EActionState::Zipping)
	{
		bool bNear = true;
		bNear &= UKismetMathLibrary::NearlyEqual_FloatFloat(PlayerLocation.X, ZippingEndPosition.X, 50.0f);
		bNear &= UKismetMathLibrary::NearlyEqual_FloatFloat(PlayerLocation.Y, ZippingEndPosition.Y, 50.0f);
		bNear &= UKismetMathLibrary::NearlyEqual_FloatFloat(PlayerLocation.Z, ZippingEndPosition.Z, 50.0f);
		if (bNear) // 끝 지점에 거의 도달했다면 Player가 짚라인 탑승 상태를 벗어나도록 함
		{
			PlayerInterface->SetUseControllerRotationYaw(true);
			PlayerCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			PlayerMovement->SetMovementMode(MOVE_Walking);
			PlayerMovement->StopMovementImmediately();
			// PlayerAnimInterface->Execute_SetPlayerActionState(EActionState::WalkingOnGround);
			
			PlayerActionState = EActionState::WalkingOnGround;
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
	StartEnd.Z = PlayerInterface->GetBottomZ();
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
		// 상호작용이 가능한 벽인가? 심층분석 (벽의 방향, 벽의 Top, 착륙지점 등 계산)
		if (ScanWall(HitLocation, ReverseNormal))
		{
			MeasureWall(); // 벽의 높이는?
			bInteracted = TryInteractWall(); // 벽의 높이, Player의 상태에 따라 상호작용
		}
	}

	/*
	// ScanWall
	FacedWallTopHitResult.Reset();
	FirstTopHitResult.Reset();
	LastTopHitResult.Reset();
	EndOfObstacleHitResult.Reset();
	VaultLandingHitResult.Reset();
	*/

	return bInteracted;
}

void UActionComponent::DetectWall(bool &bOutDetect, FVector &OutHitLocation, FRotator &OutReverseNormal) const
{
	const APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	check(Player);
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	FHitResult OutHit;

	for (int i = 0; i < 8; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector ActorLocation = Player->GetActorLocation();
		const FRotator ActorRotation = Player->GetActorRotation();
		const FVector TempVector = UPlayerHelper::MoveVectorUpward(
				UPlayerHelper::MoveVectorDownward(ActorLocation, 40.0f),
				i * 20
			);
		const FVector Start = UPlayerHelper::MoveVectorBackward(TempVector, ActorRotation, 30.0f);
		const FVector End = Player->GetActorForwardVector() * 200.0f + TempVector;
		
		const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			10.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerboseDetect ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			OutHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
		);

		if (true == bHit)
		{
			break;
		}
	}
	bOutDetect = OutHit.bBlockingHit;
	OutHitLocation = OutHit.Location;
	OutReverseNormal = UPlayerHelper::ReverseNormal(OutHit.Normal);
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
			bVerboseScan ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			FacedWallTopHitResult,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
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
		UE_LOG(LogTemp, Warning, TEXT("false == FacedWallTopHitResult.bBlockingHit"))
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
	WallHeight = FirstTopHitResult.ImpactPoint.Z - PlayerInterface->GetBottomZ();
		
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

	// TODO: 각 행동에 필요한 높이 값을 에디터에서 수정할 수 있도록 UPROPERTY 세팅
	if (FirstTopHitResult.bBlockingHit)
	{
		if (bIsOnLand)
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
						TryHang();
						return true;
					}
				}
				else
				{
					if (WallHeight <= 150.0f)
					{
						if (false == bIsStanding)
						{
							// Vault 동작 수행
							if (WallHeight > 50.0f)
							{
								// One Hand Vault 동작 수행
								if (bVerboseInteract)
								{
									UE_LOG(LogTemp, Warning, TEXT("One Hand Vault 동작 수행"));
								}
								TryVault(EVaults::OneHandVault);
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
	}

	return false;
}

void UActionComponent::OnVaultMontageStarted(UAnimMontage* Montage)
{
	if (bVerboseMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UActionComponent::OnVaultMontageStarted"));
		UE_LOG(LogTemp, Warning, TEXT("Location : %s"), *PlayerMesh->GetComponentLocation().ToString());
	}
	// 장애물과의 상호작용 액션이 동작하는 중에 충돌 처리를 하지 않도록 함
	PlayerCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayerMovement->SetMovementMode(MOVE_Flying);
	PlayerAnimInstance->OnMontageStarted.RemoveDynamic(this, &UActionComponent::OnVaultMontageStarted);
}

void UActionComponent::OnVaultMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bVerboseMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UActionComponent::OnVaultMontageEnded"));
		UE_LOG(LogTemp, Warning, TEXT("Location : %s"), *PlayerMesh->GetComponentLocation().ToString());
	}
	bCanInteract = true;
	PlayerAnimInstance->OnMontageEnded.RemoveDynamic(this, &UActionComponent::OnVaultMontageEnded);
}

void UActionComponent::OnVaultMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bVerboseMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UActionComponent::OnVaultMontageBlendingOut"));
		UE_LOG(LogTemp, Warning, TEXT("Location : %s"), *PlayerMesh->GetComponentLocation().ToString());
	}
	PlayerCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlayerMovement->SetMovementMode(MOVE_Walking);
	PlayerAnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UActionComponent::OnVaultMontageBlendingOut);
}

void UActionComponent::TryVault(const EVaults VaultType)
{
	bCanInteract = false;

	// TODO: 각 몽타주 별로 자연스러운 애니메이션 실행을 위해 필요한 보정 값을 에디터에서 수정할 수 있도록 UPROPERTY 세팅
	UAnimMontage* AnimMontage = nullptr;
	FVector Start = FVector::ZeroVector, End = FVector::ZeroVector;
	switch (VaultType)
	{
	case EVaults::OneHandVault:
		AnimMontage = OneHandVault;
		Start = FirstTopHitResult.Location;
		End = VaultLandingHitResult.Location;
		break;
	case EVaults::TwoHandVault:
		AnimMontage = TwoHandVault;
		Start = UPlayerHelper::MoveVectorDownward(
			UPlayerHelper::MoveVectorForward(FirstTopHitResult.Location, WallRotation, 47.0f), 49);
		End = VaultLandingHitResult.Location;
		break;
	case EVaults::FrontFlip:
		AnimMontage = FrontFlip;
		Start = UPlayerHelper::MoveVectorBackward(FirstTopHitResult.Location, WallRotation, 100.0f);
		End = VaultLandingHitResult.Location;
		break;
	}

	if (bVerboseMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start : %s, End : %s"), *Start.ToString(), *End.ToString());
	}
	PlayerMotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("VaultStart"), Start, WallRotation);
	PlayerMotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("VaultEnd"), End, WallRotation);
	
	PlayerAnimInstance->OnMontageStarted.AddDynamic(this, &UActionComponent::OnVaultMontageStarted);
	PlayerAnimInstance->OnMontageEnded.AddDynamic(this, &UActionComponent::OnVaultMontageEnded);
	PlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UActionComponent::OnVaultMontageBlendingOut);
	PlayerAnimInstance->Montage_Play(AnimMontage);
}

void UActionComponent::OnStartHangingMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bVerboseMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UActionComponent::OnStartHangingMontageBlendingOut"));
	}

	bCanClimbing = true;
	PlayerActionState = EActionState::Climbing;
	PlayerAnimInterface->SetPlayerActionState(EActionState::Climbing);
	PlayerMovement->StopMovementImmediately();
	PlayerAnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UActionComponent::OnStartHangingMontageBlendingOut);
}

void UActionComponent::TryHang()
{
	bCanInteract = false;
	// Climbing 중에는 마우스 움직임이 발생해도 회전하지 않도록 합니다. (카메라만 회전)
	PlayerInterface->SetUseControllerRotationYaw(false);
	PlayerCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayerMovement->SetMovementMode(MOVE_Flying);
	PlayerMovement->StopMovementImmediately();
	
	// TODO: 자연스러운 Climb 애니메이션 실행을 위해 필요한 보정 값을 에디터에서 수정할 수 있도록 UPROPERTY 세팅
	const FVector TargetLocation = FirstTopHitResult.ImpactPoint;
	PlayerMotionWarping->AddOrUpdateWarpTargetFromLocation(TEXT("HangEnd"), TargetLocation);
	PlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UActionComponent::OnStartHangingMontageBlendingOut);
	PlayerAnimInstance->Montage_Play(Hanging);
}

void UActionComponent::MoveOnWall(const FVector2D& InMovementVector)
{
	MovementVector = InMovementVector;
	if (false == PlayerAnimInstance->IsAnyMontagePlaying())
	{
		PlayerAnimInterface->SetMovementVector(MovementVector);
		TriggerClimbMovement();
	}
	else
	{
		PlayerMovement->StopMovementImmediately();
		ResetMoveValue();
	}
	
}

void UActionComponent::ResetMoveValue()
{
	MovementVector = FVector2D::ZeroVector;
	PlayerAnimInterface->SetMovementVector(MovementVector);
}

void UActionComponent::TriggerClimbMovement()
{
	const TArray<AActor*> ActorsToIgnore;
	;
	for (int i = 0; i < 3; i++)
	{
		FVector Start = UPlayerHelper::MoveVectorDownward(
			UPlayerHelper::MoveVectorRight(
				UPlayerHelper::MoveVectorUpward(PlayerCapsule->GetComponentLocation(), 120.0f),
				PlayerCapsule->GetComponentRotation(),
				MovementVector.X * 10.0f
			),
			i * 10
		);

		const FVector End = UPlayerHelper::MoveVectorForward(
			Start,
			PlayerCapsule->GetComponentRotation(),
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
			WallHitResultForClimbMove,
			true
		);

		if (true == bHit)
		{
			WallRotation = UPlayerHelper::ReverseNormal(WallHitResultForClimbMove.ImpactNormal);
			break;
		}
		PlayerMovement->StopMovementImmediately();
	}
	
	const FVector Start = UPlayerHelper::MoveVectorUpward(
		UPlayerHelper::MoveVectorForward(WallHitResultForClimbMove.ImpactPoint, WallRotation, 2.0f),
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
		WallTopHitResultForClimbMove,
		true
	);
	
	if (true == WallTopHitResultForClimbMove.bStartPenetrating)
	{
		// 시작 지점이 이미 다른 Mesh에 의해 Overlap 되어 있는 경우 즉시 Player의 움직임을 멈춘다.
		PlayerMovement->StopMovementImmediately();
	}
	else if (true == WallTopHitResultForClimbMove.bBlockingHit)
	{
		PlayerMovement->StopMovementImmediately();

		const FVector ImpactPoint = WallTopHitResultForClimbMove.ImpactPoint;
		const FVector Current = PlayerCapsule->GetComponentLocation();
		const FVector Target = UPlayerHelper::MoveVectorBackward(ImpactPoint, WallRotation, 35.0f);
		const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
		const float X = UKismetMathLibrary::FInterpTo(Current.X, Target.X, DeltaSeconds, 5.0f);
		const float Y = UKismetMathLibrary::FInterpTo(Current.Y, Target.Y, DeltaSeconds, 5.0f);
		const FVector NewLocation = FVector(X, Y, ImpactPoint.Z);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *NewLocation.ToString());
		PlayerCapsule->SetWorldLocationAndRotation(NewLocation, WallRotation);
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
	PlayerInterface->SetUseControllerRotationYaw(false);
	PlayerCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayerMovement->SetMovementMode(MOVE_Flying);
	PlayerMovement->StopMovementImmediately();
	// PlayerAnimInterface->Execute_SetPlayerActionState(PlayerAnimInstance, EActionState::Zipping);
	
	PlayerActionState = EActionState::Zipping;
	
	ZippingStartPosition = UPlayerHelper::MoveVectorDownward(TargetZipline->StartCablePosition->GetComponentLocation(), 100.0f);
	ZippingEndPosition = UPlayerHelper::MoveVectorDownward(TargetZipline->EndCablePosition->GetComponentLocation(), 100.0f);
	
	// TODO: 종속성
    APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
    Player->SetActorLocation(ZippingStartPosition);

	return true;
}

void UActionComponent::OnClimbingMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bVerboseMontage)
    {
    	UE_LOG(LogTemp, Warning, TEXT("UActionComponent::OnClimbingMontageBlendingOut"));
    	UE_LOG(LogTemp, Warning, TEXT("Location : %s"), *PlayerMesh->GetComponentLocation().ToString());
    }
	
	bCanInteract = true;
	PlayerInterface->SetUseControllerRotationYaw(true);
	PlayerCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlayerMovement->SetMovementMode(MOVE_Walking);
	PlayerActionState = EActionState::WalkingOnGround;
	PlayerAnimInterface->SetPlayerActionState(EActionState::WalkingOnGround);
	PlayerAnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UActionComponent::OnClimbingMontageBlendingOut);

	PlayerCapsule->SetWorldLocation(UPlayerHelper::MoveVectorUpward(
		UPlayerHelper::MoveVectorForward(FirstTopHitResult.ImpactPoint, WallRotation, 50.0f),
		50.0f));
	PlayerCapsule->SetWorldRotation(FRotator(0, PlayerCapsule->GetComponentRotation().Yaw, 0));
	// PlayerAnimInstance->Montage_Play(Stand);
}

void UActionComponent::TriggerClimb()
{
	UE_LOG(LogTemp, Warning, TEXT("TriggerClimb"));
	const FVector End = UPlayerHelper::MoveVectorUpward(
		UPlayerHelper::MoveVectorForward(FirstTopHitResult.ImpactPoint, WallRotation, 50.0f),
		50.0f
	);
	// PlayerMotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Climbing"), End, WallRotation);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *End.ToString());
	
	bCanClimbing = false;
	PlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UActionComponent::OnClimbingMontageBlendingOut);
	PlayerAnimInstance->Montage_Play(Climbing);
}