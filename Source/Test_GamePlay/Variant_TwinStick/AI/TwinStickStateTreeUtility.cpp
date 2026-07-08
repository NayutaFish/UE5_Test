// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickStateTreeUtility.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "TopDownTemplate"

// Tick：获取第一个本地玩家的 Pawn 作为目标角色
EStateTreeRunStatus FStateTreeGetPlayerTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// 获取实例数据
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// 获取第一个本地玩家控制的 Pawn
	InstanceData.TargetPlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(InstanceData.Character, 0));

	// 保持任务运行
	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FStateTreeGetPlayerTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	return LOCTEXT("StateTreeTaskGetPlayerDescription", "<b>Get Player</b>");
}
#endif // WITH_EDITOR
