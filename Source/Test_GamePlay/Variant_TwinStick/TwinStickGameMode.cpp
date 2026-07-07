// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickGameMode.h"
#include "TwinStickUI.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void ATwinStickGameMode::BeginPlay()
{
	// 创建 UI Widget
	CreateUI();
}

void ATwinStickGameMode::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 清除连击定时器
	GetWorld()->GetTimerManager().ClearTimer(ComboTimer);
}

// 更新 UI 中的物品数量
void ATwinStickGameMode::ItemUsed(int32 Value)
{
	// 确保 UI Widget 存在
	if (!UIWidget)
	{
		CreateUI();
	}

	// 更新 UI
	if (UIWidget)
	{
		UIWidget->UpdateItems(Value);
	}
}

// 按基础分数 x 连击倍数增加分数，并更新连击
void ATwinStickGameMode::ScoreUpdate(int32 Value)
{
	// 基础分数乘以连击倍数后累加
	Score += Value * Combo;

	// 更新 UI
	if (UIWidget)
	{
		UIWidget->UpdateScore(Score);
	}

	// 更新连击倍数
	ComboUpdate();
}

// 创建 HUD UI Widget 并添加到视口
void ATwinStickGameMode::CreateUI()
{
	// 避免重复创建
	if(UIWidget)
		return;

	// 创建 UI Widget 并添加到视口
	UIWidget = CreateWidget<UTwinStickUI>(UGameplayStatics::GetPlayerController(GetWorld(), 0), UIWidgetClass);

	if (UIWidget)
	{
		UIWidget->AddToViewport(0);
	}
}

// 累加连击计数，达到阈值后提升倍数
void ATwinStickGameMode::ComboUpdate()
{
	// 超过上限则返回
	if (Combo > ComboCap)
	{
		return;
	}

	// 增加连击增量
	++ComboIncrement;

	// 是否该提升倍数了？
	if (ComboIncrement > ComboIncrementMax)
	{
		// 重置连击增量
		ComboIncrement = 0;

		// 提升连击倍数
		++Combo;

		// 更新 UI
		if (UIWidget)
		{
			UIWidget->UpdateCombo(Combo);
		}

	}

	// 重置冷却定时器
	ResetComboCooldown();
}

// 重置连击冷却定时器
void ATwinStickGameMode::ResetComboCooldown()
{
	// 设定连击冷却定时器
	GetWorld()->GetTimerManager().SetTimer(ComboTimer, this, &ATwinStickGameMode::ResetCombo, ComboCooldown, false);
}

// 连击超时后降低倍数
void ATwinStickGameMode::ResetCombo()
{
	// 连击倍数是否大于 1？
	if (Combo > 1)
	{
		// 重置连击增量
		ComboIncrement = 0;

		// 降低倍数
		--Combo;

		// 更新 UI
		if (UIWidget)
		{
			UIWidget->UpdateCombo(Combo);
		}

		// 重置冷却定时器
		ResetComboCooldown();
	}
}

bool ATwinStickGameMode::CanSpawnNPCs()
{
	// NPC 数量是否低于上限？
	return NPCCount < NPCCap;
}

void ATwinStickGameMode::IncreaseNPCs()
{
	// 增加 NPC 计数
	++NPCCount;
}

void ATwinStickGameMode::DecreaseNPCs()
{
	// 减少 NPC 计数
	--NPCCount;
}
