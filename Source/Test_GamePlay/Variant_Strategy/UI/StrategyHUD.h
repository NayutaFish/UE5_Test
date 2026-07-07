// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StrategyHUD.generated.h"

class UStrategyUI;

/**
 * 策略游戏的简单 HUD
 * 负责绘制选择框和选中单位的叠加显示
 */
UCLASS(abstract)
class AStrategyHUD : public AHUD
{
	GENERATED_BODY()

protected:

	/** UI 用户控件指针 */
	UPROPERTY()
	TObjectPtr<UStrategyUI> UIWidget;

	/** UI 控件类（用于生成实例） */
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UStrategyUI> UIWidgetClass;

	/** 是否绘制选择框 */
	bool bDrawBox = false;

	/** 选择框的起始坐标 */
	FVector2D BoxStart;

	/** 选择框的宽高 */
	FVector2D BoxSize;

	/** 选择框的当前位置 */
	FVector2D BoxCurrentPosition;

	/** 选择框的颜色 */
	UPROPERTY(EditAnywhere, Category="UI")
	FLinearColor SelectionBoxColor;

public:

	/** 初始化 */
	virtual void BeginPlay() override;

	/** 更新拖拽选择框的状态 */
	void DragSelectUpdate(FVector2D Start, FVector2D WidthAndHeight, FVector2D CurrentPosition, bool bDraw);

protected:

	/** 绘制 HUD */
	virtual void DrawHUD() override;
};
