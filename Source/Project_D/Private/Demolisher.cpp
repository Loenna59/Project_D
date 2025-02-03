// Fill out your copyright notice in the Description page of Project Settings.


#include "Demolisher.h"

#include "DemolisherAnimInstance.h"
#include "DemolisherProp.h"
#include "PlayerCharacter.h"
#include "PlayerHUD.h"
#include "TraceChannelHelper.h"
#include "VaultGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "FSM/DemolisherAttackState.h"
#include "FSM/DemolisherFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pathfinding/PathfindingComponent.h"
#include "Pathfinding/ZombieAIController.h"
#include "Project_D/Project_DCharacter.h"

class UDemolisherAttackState;

ADemolisher::ADemolisher()
{
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	AttackPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(GetMesh());
	
	AttackPoint->SetBoxExtent(FVector(50, 50, 50));
	AttackPoint->SetCollisionProfileName(TEXT("EnemyAttack"));
	AttackPoint->SetGenerateOverlapEvents(true);

	AttackPoint2 = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackPoint2"));
	AttackPoint2->SetupAttachment(GetMesh());
	
	AttackPoint2->SetBoxExtent(FVector(50, 50, 50));
	AttackPoint2->SetCollisionProfileName(TEXT("EnemyAttack"));
	AttackPoint2->SetGenerateOverlapEvents(true);
}

void ADemolisher::BeginPlay()
{
	AActor::BeginPlay();

	CurrentHp = MaxHp;

	APawn* PlayPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (PlayPawn)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(PlayPawn);
		if (Player)
		{
			UUserWidget* UI = Player->PlayerHUD->DemolisherHealthUI;
			HealthUI = Cast<UDemolisherHealthUI>(UI);

			if (HealthUI)
			{
				HealthUI->SetVisibility(ESlateVisibility::Visible);
				HealthUI->OnChangeHp(CurrentHp, MaxHp);
			}
		}
	}

	// GetMesh()->SetMassOverrideInKg(NAME_None, Mass);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	AnimationInstance = Cast<UDemolisherAnimInstance>(GetMesh()->GetAnimInstance());
	AI = Cast<AZombieAIController>(GetController());

	FSM = NewObject<UDemolisherFSMComponent>(this);
	
	UDemolisherAttackState* AttackState = NewObject<UDemolisherAttackState>(FSM);
	AttackState->Initialize(AttackInterval, ThrowDuration, ChargeSpeed, ChargeAcceleration);
	FSM->SetupState(this, AttackState);
	
	AddOwnedComponent(FSM);
	FSM->RegisterComponent();

	FSM->ChangeState(EEnemyState::IDLE, this);

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABaseZombie::OnCollisionHit);

	if (AVaultGameModeBase* VaultGameModeBase = Cast<AVaultGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		VaultGameModeBase->IncreaseCount();
	}

	Pathfinding = NewObject<UPathfindingComponent>(this);
	AddOwnedComponent(Pathfinding);
	Pathfinding->RegisterComponent();

	Pathfinding->Initialize(this);

	if (GetMesh()->DoesSocketExist(TEXT("AttackSocket")))
	{
		AttackPoint->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("AttackSocket"));
	}

	if (GetMesh()->DoesSocketExist(TEXT("AttackSocket2")))
	{
		AttackPoint2->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("AttackSocket2"));
	}
}

void ADemolisher::Tick(float DeltaSeconds)
{
	ACharacter::Tick(DeltaSeconds);

	if (CurrentHp <= 0)
	{
		// 죽음상태로 강제
		AI->StopMovement();
		FSM->ChangeState(EEnemyState::DEATH, this);
		return;
	}
	
	if (UWorld* const World = GetWorld())
	{
		if (bIsAttacking || bIsHitting)
		{
			return;
		}
		
		TraceChannelHelper::SphereTraceByChannel(
			World,
			this,
			GetActorLocation(),
			GetActorLocation(),
			FRotator::ZeroRotator,
			ECC_EngineTraceChannel2, // "Player"
			DetectRadius,
			true,
			false,
			[this] (bool bHit, TArray<FHitResult> HitResults)
			{
				bool HitPlayer = false;
				if (bHit)
				{
					for (FHitResult Hit : HitResults)
					{
						   AActor* HitActor = Hit.GetActor();
						   if (HitActor)
						   {
								if (HitActor->IsA<AProject_DCharacter>() || HitActor->IsA<APlayerCharacter>())
								{
									double Distance = FVector::Distance(HitActor->GetActorLocation(), GetActorLocation());
									
									AI->SetTarget(HitActor);
									
									if (FSM->GetCurrentState() == EEnemyState::WALK)
									{
										return;
									}

									int32 Rand = FMath::RandRange(1, 10);
									if (Rand > 3)
									{
										if (Distance <= MidRangeAttackRadius)
										{
											int32 Rand2 = FMath::RandRange(1, 10);
											if (Rand2 > 3)
											{
												if (Distance > AttackRadius)
												{
													FSM->ChangeState(EEnemyState::WALK, this);
												}
												else
												{
													// GameDebug::ShowDisplayLog(GetWorld(), FString::SanitizeFloat(Distance));
													FSM->ChangeState(EEnemyState::ATTACK, this);
												}	
											}
											else
											{
												FSM->ChangeState(EEnemyState::ATTACK, this);
											}
										}
										else
										{
											FSM->ChangeState(EEnemyState::WALK, this);
										}
									}
									else
									{
										FSM->ChangeState(EEnemyState::ATTACK, this);
									}
									HitPlayer = true;
									break;
								}
						   }
					}
				}

				if (!HitPlayer)
				{
					Evaluate();
					
				}
			}
		);
	}
}

void ADemolisher::OnStartAttack()
{
	bIsAttacking = true;
	
	if (AttackPoint && AttackPoint2)
	{
		if (AttackTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(AttackTimerHandle);
			AttackTimerHandle.Invalidate();
		}

		if (AttackTimerHandle2.IsValid())
		{
			GetWorldTimerManager().ClearTimer(AttackTimerHandle2);
			AttackTimerHandle2.Invalidate();
		}

		TWeakObjectPtr<ADemolisher> WeakThis = Cast<ADemolisher>(this);
		
		GetWorldTimerManager().SetTimer(
			AttackTimerHandle,
			[WeakThis]
			{
				if (WeakThis.IsValid())
				{
					TraceChannelHelper::SphereTraceByChannel(
						WeakThis->GetWorld(),
						WeakThis.Get(),
						WeakThis->AttackPoint->GetComponentLocation(),
						WeakThis->AttackPoint->GetComponentLocation(),
						FRotator::ZeroRotator,
						ECC_Visibility,
						200,
						true,
						true,
						[] (bool bHit, TArray<FHitResult> HitResults)
						{
							for (FHitResult HitResult : HitResults)
							{
								if (AActor* Actor = HitResult.GetActor())
								{
									if (APlayerCharacter* P = Cast<APlayerCharacter>(Actor))
									{
										P->OnDamaged(20);
									}
								}
							}
						}
					);
				}
			},
			AttackTiming,
			false
		);

		GetWorldTimerManager().SetTimer(
			AttackTimerHandle2,
			[WeakThis]
			{
				if (WeakThis.IsValid())
				{
					TraceChannelHelper::SphereTraceByChannel(
						WeakThis->GetWorld(),
						WeakThis.Get(),
						WeakThis->AttackPoint2->GetComponentLocation(),
						WeakThis->AttackPoint2->GetComponentLocation(),
						FRotator::ZeroRotator,
						ECC_Visibility,
						200,
						true,
						true,
						[] (bool bHit, TArray<FHitResult> HitResults)
						{
							for (FHitResult HitResult : HitResults)
							{
								if (AActor* Actor = HitResult.GetActor())
								{
									if (APlayerCharacter* P = Cast<APlayerCharacter>(Actor))
									{
										P->OnDamaged(20);
									}
								}
							}
						}
					);
				}
			},
			AttackTiming2,
			false
		);

		AnimationInstance->PlayMontage(AI, AnimState::Attack, [](float _) {});
	}
}

void ADemolisher::Throw()
{
	AI->StopMovement();
	bIsAttacking = true;

	FVector TargetLocation = AI->TargetActor->GetActorLocation();

	TWeakObjectPtr<ADemolisher> WeakThis = this;
	
	AnimationInstance->PlayMontage(AI,
		AnimState::Throw,
		[WeakThis, TargetLocation](float PlayLength)
		{
			if (!WeakThis.IsValid())
			{
				return;
			}
			FActorSpawnParameters SpawnParams;
			ADemolisherProp* Prop = WeakThis->GetWorld()->
				SpawnActor<ADemolisherProp>(WeakThis->PropFactory, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			Prop->GetRootComponent()->SetVisibility(false);
			
			if (WeakThis->GetMesh()->DoesSocketExist(TEXT("RightHand")))
			{
				Prop->AttachToComponent(WeakThis->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));
			}

			TWeakObjectPtr<ADemolisherProp> WeakProp = Prop;
			
			FTimerHandle PropTimerHandle;
			WeakThis->GetWorldTimerManager().SetTimer(
				PropTimerHandle,
				[WeakProp]()
				{
					if (WeakProp.IsValid())
					{
						WeakProp->GetRootComponent()->SetVisibility(true);
					}
				},
				1.f,
				false
			);
			
			FTimerHandle PropTimerHandle2;
			WeakThis->GetWorldTimerManager().SetTimer(
				PropTimerHandle2,
				[WeakThis, WeakProp, TargetLocation]()
				{
					if (WeakProp.IsValid())
					{
						WeakProp->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
						WeakProp->Fire(WeakProp->GetActorLocation(), TargetLocation);
					}
				},
				PlayLength - 1.4f,
				false
			);
		}
	);
}

void ADemolisher::ChargeTo(float Speed, float Acceleration)
{
	AI->StopMovement();
	bIsAttacking = true;

	if (ChargingTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(ChargingTimerHandle);
		ChargingTimerHandle.Invalidate();
	}
	
    if (UDemolisherAnimInstance* D_Anim = Cast<UDemolisherAnimInstance>(AnimationInstance))
    {
        D_Anim->SetChargingAttack(true);
    }

    FVector TargetLocation = AI->TargetActor->GetActorLocation();
    FVector Location = GetActorLocation();
    FVector Direction = (TargetLocation - Location).GetSafeNormal();
    CurrentChargeSpeed = Speed;

	TWeakObjectPtr<ADemolisher> WeakThis = this;
	
    GetWorldTimerManager().SetTimer(
        ChargingTimerHandle,
        [Direction, Acceleration, WeakThis]()
        {
        	if (!WeakThis.IsValid())
        	{
        		return;
        	}
        	
        	if (!WeakThis->bIsAttacking)
        	{
        		if (WeakThis->ChargingTimerHandle.IsValid())
        		{
					WeakThis->GetWorldTimerManager().ClearTimer(WeakThis->ChargingTimerHandle);
					WeakThis->ChargingTimerHandle.Invalidate();
				}
        		return;
        	}
        	
            // GameDebug::ShowDisplayLog(GetWorld(), FString::SanitizeFloat(ChargeSpeed));
            WeakThis->CurrentChargeSpeed += Acceleration;
            FVector Delta = Direction * WeakThis->CurrentChargeSpeed * 0.01f;
            FVector Location = WeakThis->GetActorLocation();
        	
            WeakThis->SetActorLocation(Location + Delta);

        	TraceChannelHelper::SphereTraceByChannel(
        		WeakThis->GetWorld(),
        		WeakThis.Get(),
        		Location,
        		Location + Delta,
        		FRotator::ZeroRotator,
        		ECC_EngineTraceChannel2,
        		100,
        		true,
        		true,
        		[WeakThis, Direction] (bool bHit, TArray<FHitResult> HitResults)
        		{
        			if (!WeakThis.IsValid())
        			{
        				return;
        			}
        			
        			for (FHitResult Hit : HitResults)
                    {
        				UPrimitiveComponent* HitComp = Hit.GetComponent();
                        if (HitComp && HitComp->Mobility == EComponentMobility::Movable)
                        {
                        	// HitComp->SetSimulatePhysics(true);
                            // HitComp->AddImpulse(Direction * ChargeSpeed, NAME_None, true);
                        }
                    }
        		}
        	);
        },
        0.01f,
        true,
        0.25f
    );
}

void ADemolisher::FinishAttack()
{
	if (ChargingTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(ChargingTimerHandle);
		ChargingTimerHandle.Invalidate();
	}

	if (UDemolisherAnimInstance* D_Anim = Cast<UDemolisherAnimInstance>(AnimationInstance))
	{
		D_Anim->SetChargingAttack(false);
	}

	TWeakObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Player.IsValid())
	{
		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), Player->GetActorLocation());
		SetActorRotation(NewRotation);
	}

	FTimerHandle WaitTimerHandle;
	TWeakObjectPtr<ADemolisher> WeakThis = this;
	
	GetWorldTimerManager().SetTimer(
		WaitTimerHandle,
		[WeakThis] ()
		{
			WeakThis->Super::FinishAttack();
		},
		1.f,
		false
	);
}

void ADemolisher::PhysicsAttack(AZombieTriggerParam* ZombieParam, FHitResult HitResult,
	USkeletalMeshComponent* MeshComponent, bool& IsSimulated)
{
	if (!bIsAttacking)
	{
		bIsHitting = true;
		Evaluate();

		if (HitTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(HitTimerHandle);
			HitTimerHandle.Invalidate();
		}

		TWeakObjectPtr<ADemolisher> WeakThis = this;
			
		AnimationInstance->PlayMontage(
			AI,
			AnimState::Hit,
			[WeakThis] (float PlayLength)
			{
				if (WeakThis.IsValid())
				{
					WeakThis->GetWorldTimerManager().SetTimer(
						WeakThis->HitTimerHandle,
						[WeakThis] ()
						{
							if (!WeakThis.IsValid())
							{
								return;
							}
							WeakThis->bIsHitting = false;
						},
						PlayLength,
						false
					);
				}
			}
		);
			
	}
	
}

void ADemolisher::OnTriggerEnter(class AActor* OtherActor, class ACollisionTriggerParam* Param)
{
	Super::OnTriggerEnter(OtherActor, Param);

	if (HealthUI)
	{
		HealthUI->OnChangeHp(CurrentHp, MaxHp);
	}
}
