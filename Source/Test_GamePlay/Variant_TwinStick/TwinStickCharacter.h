// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "TwinStickCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class APlayerController;
class UInputAction;
class ATwinStickAoEAttack;
class ATwinStickProjectile;

/**
 *  双摇杆射击游戏的玩家角色。
 *  自动旋转面向瞄准方向，可发射子弹和释放范围攻击（AoE）。
 */
UCLASS(abstract)
class ATwinStickCharacter : public ACharacter
{
	GENERATED_BODY()

	/** 相机弹簧臂 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** 玩家相机 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

protected:

	/** 移动输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** 手柄瞄准输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* StickAimAction;

	/** 鼠标瞄准输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseAimAction;

	/** 冲刺输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* DashAction;

	/** 射击输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ShootAction;

	/** AoE 攻击输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AoEAction;

	/** 鼠标瞄准使用的碰撞追踪通道 */
	UPROPERTY(EditAnywhere, Category="Input")
	TEnumAsByte<ETraceTypeQuery> MouseAimTraceChannel;

	/** 冲刺时施加给角色的冲击力 */
	UPROPERTY(EditAnywhere, Category="Dash", meta = (ClampMin = 0, ClampMax = 10000, Units = "cm/s"))
	float DashImpulse = 2500.0f;

	/** 射击时生成的子弹类型 */
	UPROPERTY(EditAnywhere, Category="Projectile")
	TSubclassOf<ATwinStickProjectile> ProjectileClass;

	/** 子弹生成位置在角色前方的偏移距离 */
	UPROPERTY(EditAnywhere, Category="Projectile", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm"))
	float ProjectileOffset = 100.0f;

	/** AoE 攻击生成器的类型 */
	UPROPERTY(EditAnywhere, Category="AoE")
	TSubclassOf<ATwinStickAoEAttack> AoEAttackClass;

	/** 可使用的 AoE 攻击次数 */
	UPROPERTY(EditAnywhere, Category="AoE")
	int32 Items = 1;

	/** 角色受到伤害时的击退力度 */
	UPROPERTY(EditAnywhere, Category="Damage", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm"))
	float KnockbackStrength = 2500.0f;

	/** AoE 攻击后的冷却时间 */
	UPROPERTY(EditAnywhere, Category="AoE", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float AoECooldownTime = 1.0f;

	/** 手柄瞄准时旋转插值速度 */
	UPROPERTY(EditAnywhere, Category="Aim", meta = (ClampMin = 0, ClampMax = 100, Units = "s"))
	float AimRotationInterpSpeed = 10.0f;

	/** 上次 AoE 攻击的游戏时间 */
	float LastAoETime = 0.0f;

	/** 瞄准偏航角（度） */
	float AimAngle = 0.0f;

	/** 指向该角色所属的玩家控制器 */
	TObjectPtr<APlayerController> PlayerController;

	/** 是否正在使用鼠标瞄准 */
	bool bUsingMouse = false;

	/** 上次的移动输入 */
	FVector2D LastMoveInput;

	/** 手柄自动射击是否激活 */
	bool bAutoFireActive = false;

	/** 自动射击间隔时间 */
	UPROPERTY(EditAnywhere, Category="Aim", meta = (ClampMin = 0, ClampMax = 5, Units = "s"))
	float AutoFireDelay = 0.2f;

	/** 手柄自动射击定时器 */
	FTimerHandle AutoFireTimer;

public:

	/** 构造函数 */
	ATwinStickCharacter();

protected:

	/** 游戏初始化 */
	virtual void BeginPlay() override;

	/** 游戏清理 */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	/** 控制器变更时调用 */
	virtual void NotifyControllerChanged() override;

public:

	/** 每帧更新角色旋转以朝向瞄准方向 */
	virtual void Tick(float DeltaTime) override;

	/** 绑定输入动作 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** 处理移动输入 */
	void Move(const FInputActionValue& Value);

	/** 处理手柄瞄准 */
	void StickAim(const FInputActionValue& Value);

	/** 处理鼠标瞄准 */
	void MouseAim(const FInputActionValue& Value);

	/** 执行冲刺 */
	void Dash(const FInputActionValue& Value);

	/** 发射子弹 */
	void Shoot(const FInputActionValue& Value);

	/** 执行 AoE 攻击 */
	void AoEAttack(const FInputActionValue& Value);

public:

	/** 处理来自输入动作和触摸界面的移动输入 */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoMove(float AxisX, float AxisY);

	/** 处理来自输入动作和触摸界面的瞄准输入 */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoAim(float AxisX, float AxisY);

	/** 处理来自输入动作和触摸界面的冲刺输入 */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoDash();

	/** 处理来自输入动作和触摸界面的射击输入 */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoShoot();

	/** 处理来自输入动作和触摸界面的 AoE 攻击输入 */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoAoEAttack();

public:

	/** 处理碰撞伤害，包含击退效果 */
	void HandleDamage(float Damage, const FVector& DamageDirection);

protected:

	/** 让蓝图代码响应受伤事件 */
	UFUNCTION(BlueprintImplementableEvent, Category="Damage", meta = (DisplayName = "Damaged"))
	void BP_Damaged();

public:

	/** 给玩家增加一个拾取物品 */
	void AddPickup();

protected:

	/** 更新 GameMode 中的物品计数 */
	void UpdateItems();

	/** 自动射击冷却结束后重置标志 */
	void ResetAutoFire();
};
