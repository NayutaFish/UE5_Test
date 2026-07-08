// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorBase.generated.h"

/** 角色基类
 *  提供基础的生命值、受伤、死亡功能
 *  可被蓝图继承扩展
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class TEST_GAMEPLAY_API AActorBase : public AActor
{
	GENERATED_BODY()

public:

	AActorBase();

	/** 当前血量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 Health;

	/** 最大血量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxHealth;

	/** 受伤处理
	 *  @param DamageValue 伤害值
	 *  @param Attacker    攻击者 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void TakeDamage(int32 DamageValue, AActorBase* Attacker);

	/** 死亡（蓝图可重写） */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void Die();
	virtual void Die_Implementation();
};
