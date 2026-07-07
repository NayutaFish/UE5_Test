// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickAIController.h"
#include "Components/StateTreeAIComponent.h"

// 构造函数：创建 StateTree 组件并配置附身行为
ATwinStickAIController::ATwinStickAIController()
{
	// 创建 StateTree AI 组件
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAI"));
	check(StateTreeAI);

	// 附身 Pawn 时自动启动 StateTree
	bStartAILogicOnPossess = true;

	// 附加到附身的角色上（EnvQuery 正常工作需要）
	bAttachToPawn = true;
}
