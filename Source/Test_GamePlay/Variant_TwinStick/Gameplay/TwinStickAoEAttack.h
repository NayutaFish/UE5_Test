// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TwinStickAoEAttack.generated.h"

class UStaticMeshComponent;
class USphereComponent;

/**
 *  持续性范围攻击（AoE）。
 *  在激活期间持续伤害进入范围的敌人。
 */
UCLASS(abstract)
class ATwinStickAoEAttack : public AActor
{
	GENERATED_BODY()

	/** AoE 视觉效果 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SphereVisual;

	/** AoE 碰撞体积 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;

protected:

	/** 启动 AoE 伤害检查的定时器 */
	FTimerHandle StartAoETimer;

	/** 停止 AoE 伤害检查的定时器 */
	FTimerHandle StopAoETimer;

	/** AoE 伤害检测间隔 */
	UPROPERTY(EditAnywhere, Category="AoE Attack", meta=(ClampMin = 0, ClampMax = 5, Units = "s"))
	float StartAoETime = 0.033f;

	/** AoE 持续时长 */
	UPROPERTY(EditAnywhere, Category="AoE Attack", meta=(ClampMin = 0, ClampMax = 5, Units = "s"))
	float StopAoETime = 0.5f;

	/** 为 true 时 AoE 会对重叠的物体造成伤害 */
	bool bIsAoEActive = false;

public:

	/** 构造函数 */
	ATwinStickAoEAttack();

protected:

	/** 初始化 */
	virtual void BeginPlay() override;

	/** 清理 */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

protected:

	/** 启动 AoE 定时器回调 */
	void StartAoE();

	/** 停止 AoE 定时器回调 */
	void StopAoE();

	/** 蓝图处理 AoE 淡出效果。注意：结束时必须调用 Destroy Actor！ */
	UFUNCTION(BlueprintImplementableEvent, Category="AoE Attack")
	void BP_AoEFinished();

	/** 处理 AoE 激活期间与敌人的碰撞重叠 */
	UFUNCTION()
	void OnAoEOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
