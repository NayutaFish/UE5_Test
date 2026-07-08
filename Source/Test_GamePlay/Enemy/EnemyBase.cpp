// Copyright Epic Games, Inc. All Rights Reserved.

#include "Enemy/EnemyBase.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Test_GamePlay.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SceneRoot);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bIsDead = false;

	FTimerHandle DieTimerHandle;

	
	
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
