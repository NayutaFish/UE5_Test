// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "AttackAreaBase.generated.h"

/** 攻击范围检测基类
 *  同时用于射弹检测范围 和 近战检测范围
 *
 *  区别：
 *    射弹 → Speed > 0, bDetectObstacle = true, LifeTime 较长
 *    近战 → Speed = 0, bDetectObstacle = false, LifeTime 极短
 */
UCLASS(Blueprintable, Abstract)
class TEST_GAMEPLAY_API AAttackAreaBase : public AActor
{
	GENERATED_BODY()

public:

	AAttackAreaBase();

	// ── 组件 ──

	/** 碰撞检测范围（球形） */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	// ── 攻击参数 ──

	/** 存在时间（秒），到期自动销毁。近战设0.1~0.3，射弹设3~5 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float LifeTime = 1.0f;

	/** 移动速度（cm/s）。近战为0，射弹按需设置 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Speed = 0.0f;

	/** 伤害值 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float DamageValue = 10.0f;

	/** 攻击者（谁打出的这个攻击范围） */
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;

	// ── 碰撞过滤 ──

	/** 只检测敌对目标？
	 *  true  → 只伤害与攻击者为敌对的单位
	 *          （Player的攻击打Enemy，Enemy的攻击打Player）
	 *  false → 不区分阵营，碰到谁伤谁 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Filter")
	bool bDetectEnemyOnly = true;

	/** 检测障碍物？（仅射弹类启用，碰到障碍就销毁） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Filter")
	bool bDetectObstacle = false;

	/** 检测可破坏建筑？
	 *  当攻击方为玩家时，检测敌方可破坏建筑；
	 *  当攻击方为敌人时，检测玩家的可破坏建筑 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Filter")
	bool bDetectDestructible = false;

	// ── 生命周期 ──

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** 初始化攻击范围（生成后调用） */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void Initialize(AActor* InOwner, float InLifeTime, float InSpeed, float InDamage, bool bEnemyOnly);

	/** 碰撞开始 */
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

protected:

	/** 处理伤害逻辑（子类重写具体伤害计算） */
	UFUNCTION(BlueprintNativeEvent, Category = "Attack")
	void ApplyDamage(AActor* Target);
	virtual void ApplyDamage_Implementation(AActor* Target);

	/** 碰撞对象的类型判断（子类重写具体的标签/类判断逻辑） */
	UFUNCTION(BlueprintNativeEvent, Category = "Filter")
	bool IsValidTarget(AActor* Target);
	virtual bool IsValidTarget_Implementation(AActor* Target);

private:

	float ElapsedTime = 0.0f;
};
