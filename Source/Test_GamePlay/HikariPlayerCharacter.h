// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HikariPlayerCharacter.generated.h"

class UInputAction;
class UAnimMontage;
class AAttackAreaBase;
class UHikariSkillComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDeathSignature, AActor*, DeadPlayer);

UENUM(BlueprintType)
enum class EHikariActionState : uint8
{
	Normal		UMETA(DisplayName = "Normal"),
	Attacking	UMETA(DisplayName = "Attacking"),
	Dodging		UMETA(DisplayName = "Dodging"),
	HitStun		UMETA(DisplayName = "HitStun"),
	Dead		UMETA(DisplayName = "Dead")
};

UCLASS()
class TEST_GAMEPLAY_API AHikariPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHikariPlayerCharacter();

	// 当前是否允许移动
	UFUNCTION(BlueprintPure, Category = "State")
	bool CanMove() const;

	// 当前是否允许开始攻击/闪避等动作
	UFUNCTION(BlueprintPure, Category = "State")
	bool CanStartAction() const;

	// 开始攻击
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void BeginAttack();

	// 结束攻击
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Player|Skill")
	void TryCastSkill1();

	UFUNCTION(BlueprintCallable, Category = "Player|Skill")
	void TryCastSkill2();

	UFUNCTION(BlueprintCallable, Category = "Player|Skill")
	void TryCastSkillSlot1();

	UFUNCTION(BlueprintCallable, Category = "Player|Skill")
	void TryCastSkillSlot2();

	UFUNCTION(BlueprintCallable, Category = "Player|Skill")
	void TryCastSkillSlot3();

	UFUNCTION(BlueprintPure, Category = "State")
	bool IsSprinting() const { return bIsSprinting; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Skill")
	TObjectPtr<UHikariSkillComponent> SkillComponent;

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    //用于接收蓝图中的IA_MoveHorizontal和IA_MoveVertical的输入
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Input")
	TObjectPtr<UInputAction> MoveHorizontalAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Input")
	TObjectPtr<UInputAction> MoveVerticalAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> AttackAction;
	// 普通移动速度
    // EditDefaultsOnly 表示可以在 BP_Hikari 类默认值里调整
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
float WalkSpeed = 600.0f;

// 疾跑速度
// 按住 Shift 时，把 MaxWalkSpeed 设置成这个值
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
float SprintSpeed = 900.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;
// 攻击动画蒙太奇
// 之后在 BP_Hikari 类默认值里指定为 AM_Hikari_Attack_01
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
TObjectPtr<UAnimMontage> AttackMontage;

	// 攻击被中断时的动画淡出时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackCancelBlendOutTime = 0.08f;

	// 当前角色动作状态
	// 第一版用于判断：攻击中不能移动，攻击中不能再次攻击
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EHikariActionState CurrentActionState = EHikariActionState::Normal;
private:
    void MoveHorizontal(const FInputActionValue& Value);
	void MoveVertical(const FInputActionValue& Value);
	// 开始疾跑：按下 Shift 时调用
void StartSprint();

// 停止疾跑：松开 Shift 时调用
void StopSprint();

	// 鼠标左键触发 IA_Attack 后调用
	void OnAttackInput();

	// 攻击 Montage 播放结束时调用
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 统一切换角色动作状态
	void SetActionState(EHikariActionState NewState);

	// 取消当前攻击
	void CancelAttack();
public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Stats", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Stats")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
	bool bIsDead = false;

	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnPlayerDeathSignature OnPlayerDeath;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void HandleDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void TestDie();

protected:
	void Die();
	void OnAttack();

	/** 攻击范围蓝图类（在蓝图中赋值） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<AAttackAreaBase> AttackAreaClass;
};
