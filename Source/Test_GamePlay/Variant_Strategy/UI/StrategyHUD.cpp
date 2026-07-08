// Copyright Epic Games, Inc. All Rights Reserved.


#include "StrategyHUD.h"
#include "StrategyUnit.h"
#include "StrategyPlayerController.h"
#include "StrategyUI.h"

void AStrategyHUD::BeginPlay()
{
	Super::BeginPlay();

	// 生成 UI 控件
	UIWidget = CreateWidget<UStrategyUI>(GetOwningPlayerController(), UIWidgetClass);
	check(UIWidget);

	// 将 UI 控件添加到视口
	UIWidget->AddToViewport(0);
}

void AStrategyHUD::DragSelectUpdate(FVector2D Start, FVector2D WidthAndHeight, FVector2D CurrentPosition, bool bDraw)
{
	// 更新选择框数据
	bDrawBox = bDraw;
	BoxStart = Start;
	BoxSize = WidthAndHeight;
	BoxCurrentPosition = CurrentPosition;

}

void AStrategyHUD::DrawHUD()
{
	// 绘制所有调试信息等
	Super::DrawHUD();

	// 确保有有效的玩家控制器
	if (AStrategyPlayerController* PC = Cast<AStrategyPlayerController>(GetOwningPlayerController()))
	{
		// 绘制选择框
		if (bDrawBox)
		{
			DrawRect(SelectionBoxColor, BoxStart.X, BoxStart.Y, BoxSize.X, BoxSize.Y);

			// 获取选择框中的所有单位
			TArray<AStrategyUnit*> BoxedUnits;
			GetActorsInSelectionRectangle(BoxStart, BoxCurrentPosition, BoxedUnits, true);

			// 在玩家控制器上更新单位选择
			PC->DragSelectUnits(BoxedUnits);
		}

		// 获取当前选中的单位列表
		TArray<AStrategyUnit*> SelectedUnits = PC->GetSelectedUnits();

		// 更新 UI 上的选中计数
		if (UIWidget)
		{
			UIWidget->SetSelectedUnitsCount(SelectedUnits.Num());
		}

		// 处理每个选中的单位
		for (AStrategyUnit* CurrentUnit : SelectedUnits)
		{
			if (IsValid(CurrentUnit))
			{
				// 将单位的坐标投影到屏幕坐标
				FVector2D ScreenCoords;

				if (PC->ProjectWorldLocationToScreen(CurrentUnit->GetActorLocation(), ScreenCoords, true))
				{
					// 在单位附近绘制选中标识文字
					const FString SelectionString = "Selected";
					DrawText(SelectionString, FColor::White, ScreenCoords.X - 25.0f, ScreenCoords.Y + 25.0f, nullptr, 1.5f);
				}
			}

		}
	}

}
