// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_MoveGoal.generated.h"

/**
 * 环境查询上下文：返回策略单位的当前移动目标位置
 * 供 EQS 查询使用，为 AI 决策提供移动目标点作为上下文数据
 */
UCLASS()
class TEST_GAMEPLAY_API UEnvQueryContext_MoveGoal : public UEnvQueryContext
{
	GENERATED_BODY()

	/** 向查询实例提供上下文数据（当前单位的移动目标位置） */
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
