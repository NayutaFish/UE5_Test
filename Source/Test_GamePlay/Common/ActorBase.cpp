// Copyright Epic Games, Inc. All Rights Reserved.

#include "Common/ActorBase.h"
#include "Engine/World.h"

AActorBase::AActorBase()
{
	Health = 100;
	MaxHealth = 100;
}

void AActorBase::TakeDamage(int32 DamageValue, AActorBase* Attacker)
{
	if (DamageValue <= 0) return;

	Health = FMath::Clamp(Health - DamageValue, 0, MaxHealth);

	if (Health <= 0)
	{
		Die();
	}
}

void AActorBase::Die_Implementation()
{
	// 死亡逻辑（蓝图可重写）
	Destroy();
}
