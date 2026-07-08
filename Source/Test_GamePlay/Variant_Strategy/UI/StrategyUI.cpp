// Copyright Epic Games, Inc. All Rights Reserved.


#include "StrategyUI.h"

void UStrategyUI::SetSelectedUnitsCount(int32 Count)
{
	// 检查计数是否有变化
	bool bChanged = SelectedUnitCount != Count;

	// 更新计数器
	SelectedUnitCount = Count;

	// 如果计数有变化，调用蓝图更新处理
	if (bChanged)
	{
		BP_UpdateUnitsCount();
	}
}
