// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TwinStickNPCDestruction.generated.h"

/**
 *  NPC 销毁代理。
 *  替换被销毁的 NPC，用于播放特效而不影响游戏逻辑。
 */
UCLASS(abstract)
class ATwinStickNPCDestruction : public AActor
{
	GENERATED_BODY()

public:

	/** 构造函数 */
	ATwinStickNPCDestruction();

};
