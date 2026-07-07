// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TwinStickGameMode.generated.h"

class UTwinStickUI;

/**
 *  双摇杆射击游戏的 GameMode。
 *  管理分数、连击倍数和 UI。
 */
UCLASS(abstract)
class ATwinStickGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	/** 要生成的 UI Widget 类型 */
	UPROPERTY(EditAnywhere, Category="Twin Stick")
	TSubclassOf<UTwinStickUI> UIWidgetClass;

	/** 指向生成的 UI Widget */
	TObjectPtr<UTwinStickUI> UIWidget;

	/** 当前得分 */
	int32 Score = 0;

	/** 当前连击倍数 */
	int32 Combo = 1;

	/** 连击增量计数 */
	int32 ComboIncrement = 0;

	/** 连击增量达到此值后提升倍数 */
	UPROPERTY(EditAnywhere, Category="Twin Stick", meta=(ClampMin = 0, ClampMax = 10))
	int32 ComboIncrementMax = 5;

	/** 最大连击倍数 */
	UPROPERTY(EditAnywhere, Category="Twin Stick", meta=(ClampMin = 0, ClampMax = 10))
	int32 ComboCap = 4;

	/** 连击超时重置时间 */
	UPROPERTY(EditAnywhere, Category="Twin Stick", meta=(ClampMin = 0, ClampMax = 10, Units = "s"))
	float ComboCooldown = 3.0f;

	/** 上次连击击杀的游戏时间 */
	float LastComboTime = 0.0f;

	FTimerHandle ComboTimer;

	/** 场景中允许的最大 NPC 数量 */
	UPROPERTY(EditAnywhere, Category="Twin Stick", meta=(ClampMin = 0, ClampMax = 100))
	int32 NPCCap = 20;

	/** 场景中当前的 NPC 数量 */
	int32 NPCCount = 0;

public:

	/** 游戏初始化 */
	virtual void BeginPlay() override;

	/** 清理 */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:

	/** 当物品被使用时调用 */
	void ItemUsed(int32 Value);

	/** 按给定值增加分数 */
	void ScoreUpdate(int32 Value);

protected:

	/** 创建 UI Widget（如果尚未创建） */
	void CreateUI();

	/** 更新连击倍数 */
	void ComboUpdate();

	/** 重置连击冷却定时器 */
	void ResetComboCooldown();

	/** 连击超时后重置倍数 */
	void ResetCombo();

public:

	/** 返回当前 NPC 数量是否低于上限 */
	bool CanSpawnNPCs();

	/** 增加 NPC 计数 */
	void IncreaseNPCs();

	/** 减少 NPC 计数 */
	void DecreaseNPCs();
};
