// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "PlayerInputComponent.generated.h"

class UInputAction;
class UInputMappingContext;

// ── 输入事件委托（多播，供角色订阅） ──
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerInputAxis, float);
DECLARE_MULTICAST_DELEGATE(FOnPlayerInputAction);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEST_GAMEPLAY_API UPlayerInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInputComponent();

	/** 注册到 Enhanced Input 子系统并绑定回调（由 SetupPlayerInputComponent 调用） */
	void SetupEnhancedInput(UEnhancedInputComponent* EnhancedInput, APlayerController* PC);

	// ── 委托实例 ──
	FOnPlayerInputAxis OnMoveXDelegate;
	FOnPlayerInputAxis OnMoveYDelegate;
	FOnPlayerInputAxis OnShiftDelegate;
	FOnPlayerInputAction OnLmbDelegate;
	FOnPlayerInputAction OnRmbDelegate;
	FOnPlayerInputAction OnSpaceDelegate;
	FOnPlayerInputAction OnQDelegate;
	FOnPlayerInputAction OnEDelegate;
	FOnPlayerInputAction OnFDelegate;

protected:
	virtual void BeginPlay() override;
	bool bInputsMapped = false;

	// ── 轴输入（按住持续触发） ──

	/** 横向移动输入（A/D） */
	UPROPERTY()
	TObjectPtr<UInputAction> MoveXAction;

	/** 纵向移动输入（W/S） */
	UPROPERTY()
	TObjectPtr<UInputAction> MoveYAction;

	/** 左 Shift 按住 */
	UPROPERTY()
	TObjectPtr<UInputAction> ShiftAction;

	// ── 动作输入（按下一次触发） ──

	UPROPERTY()
	TObjectPtr<UInputAction> LmbAction;

	UPROPERTY()
	TObjectPtr<UInputAction> RmbAction;

	UPROPERTY()
	TObjectPtr<UInputAction> SpaceAction;

	UPROPERTY()
	TObjectPtr<UInputAction> QAction;

	UPROPERTY()
	TObjectPtr<UInputAction> EAction;

	UPROPERTY()
	TObjectPtr<UInputAction> FAction;

	/** 默认输入映射上下文 */
	UPROPERTY()
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// ── 轴回调 ──
	void OnMoveX(const FInputActionValue& Value);
	void OnMoveY(const FInputActionValue& Value);
	void OnShift(const FInputActionValue& Value);

	// ── 动作回调 ──
	void OnLmb();
	void OnRmb();
	void OnSpace();
	void OnQ();
	void OnE();
	void OnF();
};
