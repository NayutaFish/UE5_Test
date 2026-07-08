// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StrategyTouchControls.generated.h"

class AStrategyPlayerController;

/**
 * 策略游戏的触控控件基类
 * 提供触屏操作按钮，向游戏下达命令
 */
UCLASS(abstract)
class TEST_GAMEPLAY_API UStrategyTouchControls : public UUserWidget
{
	GENERATED_BODY()

protected:

	/** 所属的策略玩家控制器指针 */
	TObjectPtr<AStrategyPlayerController> PlayerController;

public:

	/** 设置所属的策略玩家控制器指针 */
	void SetPlayerController(AStrategyPlayerController* PC);

	/** 同步摄像机缩放百分比到 UI（蓝图可覆写） */
	UFUNCTION(BlueprintImplementableEvent, Category="UI", meta=(DisplayName="Set Zoom Percentage"))
	void BP_SetZoomPercentage(float Percentage);

protected:

	/** 重置摄像机缩放（蓝图可调用） */
	UFUNCTION(BlueprintCallable, Category="UI")
	void ResetZoom();

	/** 在全选和取消全选之间切换（蓝图可调用） */
	UFUNCTION(BlueprintCallable, Category="UI")
	void ToggleSelectAllUnits();

	/** 设置摄像机缩放百分比（蓝图可调用） */
	UFUNCTION(BlueprintCallable, Category="UI")
	void SetZoomPercentage(float Percentage);
};
