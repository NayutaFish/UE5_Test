// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "TwinStickStateTreeUtility.generated.h"

class ACharacter;

/**
 *  获取玩家任务的自定义实例数据结构
 */
USTRUCT()
struct FStateTreeGetPlayerInstanceData
{
	GENERATED_BODY()

	/** 拥有此任务的角色 */
	UPROPERTY(EditAnywhere, Category="Context")
	TObjectPtr<ACharacter> Character;

	/** 输出的目标玩家角色 */
	UPROPERTY(VisibleAnywhere, Category="Output")
	TObjectPtr<ACharacter> TargetPlayerCharacter;
};

/**
 *  StateTree 任务：获取当前玩家角色
 */
USTRUCT(meta=(DisplayName="GetPlayer", Category="TwinStick"))
struct FStateTreeGetPlayerTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/* 使用正确的实例数据结构 */
	using FInstanceDataType = FStateTreeGetPlayerInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** 状态激活时每帧执行 */
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif // WITH_EDITOR
};
