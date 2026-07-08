// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TwinStickNPC.generated.h"

class ATwinStickPickup;
class ATwinStickNPCDestruction;

/**
 *  双摇杆射击游戏的敌方 NPC。
 *  由 AI Controller 驱动，销毁时奖励分数并随机生成拾取物。
 */
UCLASS(abstract)
class ATwinStickNPC : public ACharacter
{
	GENERATED_BODY()

protected:

	/** 销毁时奖励的分数 */
	UPROPERTY(EditAnywhere, Category="Score", meta=(ClampMin = 0, ClampMax = 100))
	int32 Score = 1;

	/** 生成拾取物的概率百分比 */
	UPROPERTY(EditAnywhere, Category="Pickup", meta=(ClampMin = 0, ClampMax = 100))
	int32 PickupSpawnChance = 10;

	/** 死亡时生成的拾取物类型 */
	UPROPERTY(EditAnywhere, Category="Pickup")
	TSubclassOf<ATwinStickPickup> PickupClass;

	/** 死亡时生成的销毁代理类型 */
	UPROPERTY(EditAnywhere, Category="Destruction")
	TSubclassOf<ATwinStickNPCDestruction> DestructionProxyClass;

	/** 被击中后延迟销毁的时间 */
	UPROPERTY(EditAnywhere, Category="Pickup", meta=(ClampMin = 0, ClampMax = 5, Units = "s"))
	float DeferredDestructionTime = 0.1f;

	/** 延迟销毁定时器 */
	FTimerHandle DestructionTimer;

public:

	/** 是否已被子弹击中并正在销毁，暴露给蓝图供 StateTree 读取 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="NPC")
	bool bHit = false;

public:

	/** 构造函数 */
	ATwinStickNPC();

protected:

	/** 游戏初始化 */
	virtual void BeginPlay() override;

	/** 游戏清理 */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	/** 销毁时处理 */
	virtual void Destroyed() override;

	/** 碰撞处理 */
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:

	/** 处理子弹撞击 */
	void ProjectileImpact(const FVector& ForwardVector);

protected:

	/** 定时器回调，完成 NPC 销毁流程 */
	void DeferredDestroy();
};
