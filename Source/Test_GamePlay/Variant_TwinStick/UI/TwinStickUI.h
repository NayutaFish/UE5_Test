// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TwinStickUI.generated.h"

/**
 *  双摇杆射击游戏的 UI Widget。
 *  提供蓝图接口，供 C++ 更新分数、连击和物品数量。
 */
UCLASS(abstract)
class UTwinStickUI : public UUserWidget
{
	GENERATED_BODY()

public:

	/** 蓝图事件：更新物品计数 */
	UFUNCTION(BlueprintImplementableEvent, Category="Score")
	void UpdateItems(int32 Score);

	/** 蓝图事件：更新分数显示 */
	UFUNCTION(BlueprintImplementableEvent, Category="Score")
	void UpdateScore(int32 Score);

	/** 蓝图事件：更新连击倍数显示 */
	UFUNCTION(BlueprintImplementableEvent, Category="Score")
	void UpdateCombo(int32 Combo);
};
