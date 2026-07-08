// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TwinStickNPC.h"
#include "TwinStickSpawner.generated.h"

class ARecastNavMesh;

/**
 *  双摇杆射击游戏的 NPC 生成器。
 *  按组定时生成 NPC，每批生成多个。
 */
UCLASS(abstract)
class ATwinStickSpawner : public AActor
{
	GENERATED_BODY()

protected:

	/** 要生成的 NPC 类型 */
	UPROPERTY(EditAnywhere, Category="NPC Spawner")
	TSubclassOf<ATwinStickNPC> NPCClass;

	/** 每批 NPC 的生成间隔 */
	UPROPERTY(EditAnywhere, Category="NPC Spawner", meta = (ClampMin = 0, ClampMax = 20, Units = "s"))
	float SpawnGroupDelay = 5.0f;

	/** 单个 NPC 之间的最小生成延迟 */
	UPROPERTY(EditAnywhere, Category="NPC Spawner", meta = (ClampMin = 0, ClampMax = 2, Units = "s"))
	float MinSpawnDelay = 0.33f;

	/** 单个 NPC 之间的最大生成延迟 */
	UPROPERTY(EditAnywhere, Category="NPC Spawner", meta = (ClampMin = 0, ClampMax = 2, Units = "s"))
	float MaxSpawnDelay = 0.66f;

	/** NPC 在生成器周围的可生成半径 */
	UPROPERTY(EditAnywhere, Category="NPC Spawner", meta = (ClampMin = 0, ClampMax = 20, Units = "cm"))
	float SpawnRadius = 600.0f;

	/** 每批生成的 NPC 数量 */
	UPROPERTY(EditAnywhere, Category="NPC Spawner", meta = (ClampMin = 0, ClampMax = 10))
	int32 SpawnGroupSize = 3;

	/** 当前批次已生成的 NPC 数量 */
	int32 SpawnCount = 0;

	/** 批次生成定时器 */
	FTimerHandle SpawnGroupTimer;

	/** 单个 NPC 生成定时器 */
	FTimerHandle SpawnNPCTimer;

	/** Recast 导航网格引用，用于寻找 NPC 有效生成位置 */
	TObjectPtr<ARecastNavMesh> NavData;

public:

	/** 构造函数 */
	ATwinStickSpawner();

protected:

	/** 游戏初始化 */
	virtual void BeginPlay() override;

	/** 游戏清理 */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

protected:

	/** 开始生成新一批 NPC */
	void SpawnNPCGroup();

	/** 生成单个 NPC */
	void SpawnNPC();

};
