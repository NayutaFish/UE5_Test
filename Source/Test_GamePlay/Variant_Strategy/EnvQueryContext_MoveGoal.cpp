// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Strategy/EnvQueryContext_MoveGoal.h"
#include "Variant_Strategy/StrategyUnit.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UEnvQueryContext_MoveGoal::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	// 获取发起查询的单位
	if (AStrategyUnit* QuerierActor = Cast<AStrategyUnit>(QueryInstance.Owner.Get()))
	{
		// 将移动目标位置添加到上下文数据中
		UEnvQueryItemType_Point::SetContextHelper(ContextData, QuerierActor->GetMovementGoal());
	}
}
