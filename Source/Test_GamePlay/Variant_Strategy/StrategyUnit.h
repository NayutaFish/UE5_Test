// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "StrategyUnit.generated.h"

class USphereComponent;
class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;

/** 单位移动完成时广播的委托 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitMoveCompletedDelegate, AStrategyUnit*, Unit);

/**
 * 策略游戏中的简单单位
 * 不直接响应玩家输入，而是通过策略玩家控制器间接控制
 */
UCLASS(abstract)
class AStrategyUnit : public ACharacter
{
	GENERATED_BODY()

private:

	/** 交互范围球体组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* InteractionRange;

protected:

	/** 控制此单位的 AI 控制器引用 */
	TObjectPtr<AAIController> AIController;

public:

	/** 构造函数 */
	AStrategyUnit();

protected:

	virtual void NotifyControllerChanged() override;

public:

	/** 立即停止单位移动 */
	void StopMoving();

	/** 通知该单位已被选中 */
	void UnitSelected();

	/** 通知该单位已被取消选中 */
	void UnitDeselected();

	/** 通知该单位被其他单位交互 */
	void Interact(AStrategyUnit* Interactor);

	/** 尝试移动单位到指定位置，可选择到达后与其他单位交互 */
	void MoveToLocation(const FVector& Location, bool bInteract, const TArray<AStrategyUnit*> IgnoreList);

	/** 返回最近缓存的移动目标位置 */
	FVector GetMovementGoal() const;

protected:

	/** EQS 移动目标查询完成时调用 */
	UFUNCTION()
	void OnEQSFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	/** AI 控制器通知该单位移动完成时调用 */
	void OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result);

	/** 处理移动完成的收尾逻辑 */
	void HandleMoveFinished();

protected:

	/** 蓝图可覆写：单位被选中时的处理 */
	UFUNCTION(BlueprintImplementableEvent, Category="NPC", meta = (DisplayName="Unit Selected"))
	void BP_UnitSelected();

	/** 蓝图可覆写：单位被取消选中时的处理 */
	UFUNCTION(BlueprintImplementableEvent, Category="NPC", meta = (DisplayName="Unit Deselected"))
	void BP_UnitDeselected();

	/** 蓝图可覆写：停止单位的交互动画 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="NPC", meta = (DisplayName="Stop Animation"))
	void BP_StopAnimation();

	/** 蓝图可覆写：单位交互行为 */
	UFUNCTION(BlueprintImplementableEvent, Category="NPC", meta = (DisplayName="Interaction Behavior"))
	void BP_InteractionBehavior(AStrategyUnit* Interactor);

protected:

	/** 到达后需要交互时使用的 EQS 查询 */
	UPROPERTY(EditAnywhere, Category="NPC")
	TObjectPtr<UEnvQuery> InteractionQuery;

	/** 到达后不需要交互时使用的 EQS 查询 */
	UPROPERTY(EditAnywhere, Category="NPC")
	TObjectPtr<UEnvQuery> NoInteractionQuery;

	/** 距离移动目标多远时认为已到达 */
	UPROPERTY(EditAnywhere, Category="NPC", meta = (ClampMin = 0, ClampMax = 10000, Units = "cm"))
	float MovementAcceptanceRadius = 100.0f;

	/** 交互检测时搜索附近单位的最远距离 */
	UPROPERTY(EditAnywhere, Category="Input", meta = (ClampMin = 0, ClampMax = 10000, Units = "cm"))
	float InteractionRadius = 250.0f;

	/** 该单位正在运行的 EQS 查询实例 */
	TObjectPtr<UEnvQueryInstanceBlueprintWrapper> EnvQueryInstance;

	/** 缓存的移动目标位置 */
	FVector CurrentMovementGoal;

	/** 到达目标后是否尝试与其他单位交互 */
	bool bInteractOnArrival = false;

	/** 交互检测时需要忽略的单位列表 */
	TArray<AStrategyUnit*> InteractIgnoreList;

public:

	/** 移动完成时广播的委托 */
	FOnUnitMoveCompletedDelegate OnMoveCompleted;
};
