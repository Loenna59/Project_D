// Fill out your copyright notice in the Description page of Project Settings.


#include "Demolisher.h"

#include "DemolisherAnimInstance.h"
#include "GameDebug.h"
#include "PlayerCharacter.h"
#include "TraceChannelHelper.h"
#include "VaultGameModeBase.h"
#include "Components/CapsuleComponent.h"
#include "FSM/DemolisherFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pathfinding/PathfindingComponent.h"
#include "Pathfinding/ZombieAIController.h"
#include "Project_D/Project_DCharacter.h"

void ADemolisher::BeginPlay()
{
	AActor::BeginPlay();

	CurrentHp = MaxHp;

	// GetMesh()->SetMassOverrideInKg(NAME_None, Mass);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	AnimationInstance = Cast<UDemolisherAnimInstance>(GetMesh()->GetAnimInstance());
	AI = Cast<AZombieAIController>(GetController());

	FSM = NewObject<UDemolisherFSMComponent>(this);
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
}

void ADemolisher::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
	
	if (CurrentHp <= 0)
	{
		return;
	}
	
	if (UWorld* const World = GetWorld())
	{
		if (bIsAttacking || bIsHitting)
		{
			return;
		}

		// 걷기 상태 중 플레이어와의 거리 체크
		if (bIsWalkingDistance > 0)
		{
			if (AI && AI->TargetActor)
			{
				double Distance = FVector::Distance(AI->TargetActor->GetActorLocation(), GetActorLocation());

				// 플레이어가 공격 범위 내로 들어오면 걷기 상태 종료 및 공격 상태로 전환
				if (FMath::Abs(bIsWalkingDistance - Distance) <= 50.f)
				{
					GameDebug::ShowDisplayLog(GetWorld(), "Walking End");
					bIsWalkingDistance = -1; // 걷기 상태 플래그 해제
					FSM->ChangeState(EEnemyState::ATTACK, this); // 공격 상태로 전환
				}

				return; // 걷기 상태 중에는 다른 로직을 실행하지 않음
			}
			else
			{
				// 타겟이 없으면 걷기 상태 종료
				bIsWalkingDistance = -1;
				FSM->ChangeState(EEnemyState::IDLE, this);
				return;
			}
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
			[this](bool bHit, TArray<FHitResult> HitResults)
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
								AI->SetTarget(HitActor);
                             	
                             	// 플레이어를 향해 걸어가거나
                             	// 플레이어를 향해 돌을 던진다 (30%)
                             	int32 Rand_Attack1 = FMath::RandRange(1, 10);
                             	if (Rand_Attack1 > 3)
                             	{
                             		double Distance = FVector::Distance(HitActor->GetActorLocation(), GetActorLocation());

                             		// 플레이어를 향해 가까이 걸어가거나
                             		// 플레이어를 향해 돌진한다 (30%)

									if (Distance > AttackRadius)
									{
                             			int32 Rand_Attack2 = FMath::RandRange(1, 10);

										if (Rand_Attack2 > 3)
										{
											bIsWalkingDistance = Distance; // 걷기 상태 시작
											FSM->ChangeState(EEnemyState::WALK, this);
										}
										else
										{
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
                 	Evaluate(); // 플레이어가 감지되지 않으면 평가 함수 호출
                 }
			}
		);
	}
	
}

ADemolisher::ADemolisher()
{
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

void ADemolisher::OnTriggerAttack(bool Start)
{
	bIsAttacking = Start;
	// Super::OnTriggerAttack(Start);
}

void ADemolisher::Throw()
{
	// GameDebug::ShowDisplayLog(GetWorld(), "Throw");
	AnimationInstance->PlayMontage(AI, AnimState::Throw, [](float _){});
}

void ADemolisher::Swing()
{
	//GameDebug::ShowDisplayLog(GetWorld(), "Swing");
	AnimationInstance->PlayMontage(AI, AnimState::Swing, [](float _){});
}

void ADemolisher::ChargeTo()
{
	// GameDebug::ShowDisplayLog(GetWorld(), "ChargeTo");
	if (UDemolisherAnimInstance* D_Anim = Cast<UDemolisherAnimInstance>(AnimationInstance))
	{
		D_Anim->SetChargingAttack(true);
	}
}

void ADemolisher::FinishAttack()
{
	Super::FinishAttack();
	bIsWalkingDistance = -1;

	if (UDemolisherAnimInstance* D_Anim = Cast<UDemolisherAnimInstance>(AnimationInstance))
	{
		D_Anim->SetChargingAttack(false);
	}
}


