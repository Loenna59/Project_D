// Fill out your copyright notice in the Description page of Project Settings.


#include "ClawingZombieState.h"
#include "Components/CapsuleComponent.h"
#include "BaseZombie.h"

void UClawingZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (!Zombie)
	{
		return;
	}

	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			OnceTimerHandle,
			[this, Zombie]()
			{
				if (USkeletalMeshComponent* const MeshComponent = Zombie->GetMesh())
				{
					if (UCapsuleComponent* const Collision = Zombie->GetCapsuleComponent())
					{
						// FVector HeadLocation = MeshComponent->GetSocketLocation(Zombie->HeadBone);
						// Collision->SetWorldLocation(HeadLocation);
					}
				}
				
				AddForceToBones(Zombie);
			},
			3.f,
			false,
			-1.f
		);
	}
}

void UClawingZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (!Zombie)
	{
		return;
	}

	// 	// GetWorld()->GetTimerManager().SetTimer(
	// 	// 	DelayTimerHandle,
	// 	// 	[this]()
	// 	// 	{
	// 	// 		FHitResult Hit;
	// 	// 		FCollisionQueryParams CollisionParams;
	// 	// 		CollisionParams.AddIgnoredActor(this);
	// 	// 		
	// 	// 		FVector Start = GetMesh()->GetSocketLocation(HitBoneName);
	// 	// 		FVector End = Start + FVector::DownVector * 1000.f;
	// 	// 		
	// 	// 		bool bHit = GetWorld()->LineTraceSingleByChannel(
	// 	// 			Hit,
	// 	// 			Start,
	// 	// 			End,
	// 	// 			ECC_Visibility,
	// 	// 			CollisionParams
	// 	// 		);
	// 	//
	// 	// 		if (bHit)
	// 	// 		{
	// 	// 			FVector Location = Hit.Location;
	// 	// 			FRotator Rotation = UKismetMathLibrary::MakeRotFromX(Hit.Normal);
	// 	// 			FVector DecalSize(-31.5f, -64.f, -64.f);
	// 	//           
	// 	// 			//Decal 찍기
	// 	// 			//UGameplayStatics::SpawnDecalAtLocation()
	// 	// 		}
	// 	// 	},
	// 	// 	1.f,
	// 	// 	false,
	// 	// 	-1.f // 첫 실행까지의 대기 시간 (-1은 DelayTime과 동일)
	// 	// );
}

void UClawingZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Clawing On Exit"));
	}
}

void UClawingZombieState::AddForceToBones(ABaseZombie* const Zombie)
{
	// if (USkeletalMeshComponent* const MeshComponent = Zombie->GetMesh())
	// {
	// 	if (UCapsuleComponent* const Collision = Zombie->GetCapsuleComponent())
	// 	{
	// 		MeshComponent->SetSimulatePhysics(true);
	// 		
	// 		if (Zombie->Attacker)
	// 		{
	// 			MeshComponent->AddForce(
	// 				CalculateCrawlForce(Collision, Zombie->Attacker, 500.f, 3000.f),
	// 				Zombie->HeadBone,
	// 				true
	// 			);
	//
	// 			if (Zombie->ContainsBrokenBones(Zombie->BoneArray_R))
	// 			{
	// 				if (!Zombie->ContainsBrokenBones(Zombie->BoneArray_L))
	// 				{
	// 					MeshComponent->AddForce(
	// 						CalculateCrawlForce(Collision, Zombie->Attacker, 1000.f, 1000.f),
	// 						Zombie->LeftHandBone,
	// 						true
	// 					);
	// 				}
	// 			}
	// 			else
	// 			{
	// 				MeshComponent->AddForce(
	// 					CalculateCrawlForce(Collision, Zombie-> Attacker, 1000.f, 1000.f),
	// 					Zombie->RightHandBone,
	// 					true
	// 				);
	// 			}
	// 		}
	// 	}
	// }
}

FVector UClawingZombieState::CalculateCrawlForce(UCapsuleComponent* Collision, AActor* Attacker, float ForwardForce,
	float UpwardForce)
{
	FVector CollisionWorldLocation = Collision->GetComponentLocation();
	FVector AttackerLocation = Attacker->GetActorLocation();
	FRotator LookAt = FRotationMatrix::MakeFromX(AttackerLocation - CollisionWorldLocation).Rotator();

	return LookAt.Vector() * ForwardForce + FVector(0, 0, UpwardForce);
}
