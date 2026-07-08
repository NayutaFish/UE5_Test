// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StrategyGameMode.generated.h"

/**
 * 俯视角策略游戏的简单游戏模式基类
 * 抽象类，需要子类实现具体的游戏规则逻辑
 */
UCLASS(abstract)
class AStrategyGameMode : public AGameModeBase
{
	GENERATED_BODY()

};
