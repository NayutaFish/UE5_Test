// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateBase.generated.h"

/** 状态基类
 *  继承自 UObject，可在蓝图子类中重写生命周期方法
 *
 *  用法：
 *    1. 继承 StateBase 创建具体状态
 *    2. 在状态机中调用 OnEnter → Update → OnExit
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class TEST_GAMEPLAY_API UStateBase : public UObject
{
	GENERATED_BODY()

public:

	/** 进入状态时调用 */
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnEnter();
	virtual void OnEnter_Implementation();

	/** 每帧更新（由状态机驱动） */
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void Update(float DeltaTime);
	virtual void Update_Implementation(float DeltaTime);

	/** 退出状态时调用 */
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnExit();
	virtual void OnExit_Implementation();
};
