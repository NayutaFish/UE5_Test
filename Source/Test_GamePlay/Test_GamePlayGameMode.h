// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Test_GamePlayGameMode.generated.h"

/**
 * 俯视角游戏的 GameMode（游戏规则）
 *
 * GameMode 在 UE 中负责：
 *   - 玩家出生（选择 PlayerStart、Spawn Pawn）
 *   - 游戏规则（胜负条件）
 *   - 默认的 PlayerController / Pawn / HUD 类
 *
 * 类比 Unity：相当于 GameManager，但 UE 把规则和状态分开了
 *   GameMode = 规则（仅服务器）
 *   GameState = 状态（所有人可见，联网同步）
 *
 * 注意：这个类标了 abstract（抽象类）
 * 实际参数是在它的蓝图子类 BP_TopDownGameMode 里设置的
 */
UCLASS(abstract)
class ATest_GamePlayGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** 构造函数 */
	ATest_GamePlayGameMode();
};
