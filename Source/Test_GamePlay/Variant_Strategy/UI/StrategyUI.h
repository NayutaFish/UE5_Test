// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StrategyUI.generated.h"

/**
 * 策略游戏的简单 UI 控件
 * 追踪当前选中的单位数量
 */
UCLASS(abstract)
class UStrategyUI : public UUserWidget
{
	GENERATED_BODY()

protected:

	/** 当前选中的单位数量 */
	int32 SelectedUnitCount = 0;

public:

	/** 设置选中的单位数量 */
	void SetSelectedUnitsCount(int32 Count);

	/** 蓝图可覆写：更新单位计数的子控件 */
	UFUNCTION(BlueprintImplementableEvent, Category="UI", meta = (DisplayName="Update Units Count"))
	void BP_UpdateUnitsCount();

protected:

	/** 返回当前选中的单位数量 */
	UFUNCTION(BlueprintPure, Category="UI")
	int32 GetSelectedUnitsCount() { return SelectedUnitCount; }
};
