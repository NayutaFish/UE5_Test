// Copyright Epic Games, Inc. All Rights Reserved.

#include "Enemy/EnemyBase.h"

#include "Common/AttackAreaBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Test_GamePlay.h"
#include "TimerManager.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// 胶囊体同时作为根组件
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	RootComponent = CapsuleCollision;
	CapsuleCollision->SetCapsuleHalfHeight(50.0f);
	CapsuleCollision->SetCapsuleRadius(40.0f);
	CapsuleCollision->SetCollisionObjectType(ECC_GameTraceChannel2);    // EnemyHitbox
	CapsuleCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap); // 响应伤害
	CapsuleCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	// 网格（仅显示用，碰撞走胶囊体）
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CapsuleCollision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bIsDead = false;

	StartAttackTimer();
}

void AEnemyBase::StartAttackTimer()
{
	if (!AttackAreaClass) return;

	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&AEnemyBase::OnAttackTimer,
		AttackInterval,
		true,
		AttackInterval
	);
}

void AEnemyBase::OnAttackTimer()
{
	if (bIsDead || !AttackAreaClass) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AAttackAreaBase* AttackArea = GetWorld()->SpawnActor<AAttackAreaBase>(
			AttackAreaClass,
			GetActorLocation() + GetActorForwardVector() * 80.0f,
			GetActorRotation(),
			Params))
	{
		AttackArea->Initialize(3.0f, 500.0f, 60.0f, true, true, false);
	}
}

void AEnemyBase::ApplyDamageToEnemy(float DamageAmount)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	UE_LOG(
		LogTest_GamePlay,
		Log,
		TEXT("Enemy %s took %.1f damage. CurrentHealth = %.1f"),
		*GetName(),
		DamageAmount,
		CurrentHealth
	);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void AEnemyBase::TestDie()
{
	Die();
}

void AEnemyBase::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	CurrentHealth = 0.0f;

	UE_LOG(LogTest_GamePlay, Log, TEXT("Enemy %s died."), *GetName());

	OnEnemyDeath.Broadcast(this);

	Destroy();
}
