// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TwinStickAIController.generated.h"

class UStateTreeAIComponent;

/**
 *  支持 StateTree 的 AI 控制器。
 *  通过 StateTree 运行 NPC 逻辑。
 */
UCLASS(abstract)
class ATwinStickAIController : public AAIController
{
	GENERATED_BODY()

	/** StateTree 组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStateTreeAIComponent* StateTreeAI;

public:

	/** 构造函数 */
	ATwinStickAIController();
};
