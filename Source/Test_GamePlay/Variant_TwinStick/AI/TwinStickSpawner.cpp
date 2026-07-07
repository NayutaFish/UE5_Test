// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickSpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Kismet/GameplayStatics.h"
#include "TwinStickNPC.h"
#include "TwinStickGameMode.h"

ATwinStickSpawner::ATwinStickSpawner()
{
 	PrimaryActorTick.bCanEverTick = true;

}

// 初始化：获取 NavMesh 引用，设置批次生成定时器并立即生成第一批
void ATwinStickSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 查找场景中的 Recast NavMesh
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARecastNavMesh::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		NavData = Cast<ARecastNavMesh>(ActorList[0]);
	} else {

		UE_LOG(LogTemp, Log, TEXT("Could not find recast navmesh"));

	}

	// 设置批次生成定时器
	GetWorld()->GetTimerManager().SetTimer(SpawnGroupTimer, this, &ATwinStickSpawner::SpawnNPCGroup, SpawnGroupDelay, true);

	// 立即生成第一批 NPC
	SpawnNPCGroup();
}

void ATwinStickSpawner::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 清除生成定时器
	GetWorld()->GetTimerManager().ClearTimer(SpawnGroupTimer);
	GetWorld()->GetTimerManager().ClearTimer(SpawnNPCTimer);
}

// 检查 NPC 上限后开始生成新一批
void ATwinStickSpawner::SpawnNPCGroup()
{
	// 重置批次计数器
	SpawnCount = 0;

	// 检查是否低于 NPC 上限
	if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (GM->CanSpawnNPCs())
		{
			SpawnNPC();
		}
	}
}

// 在导航网格上的随机可达点生成单个 NPC，并安排下一个 NPC 的生成
void ATwinStickSpawner::SpawnNPC()
{
	FTransform SpawnTransform;

	// 在生成器周围寻找随机可达点
	FVector SpawnLoc;
	if (UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(), GetActorLocation(), SpawnLoc, SpawnRadius, NavData))
	{
		SpawnTransform.SetLocation(SpawnLoc);

		// 生成 NPC
		ATwinStickNPC* NPC = GetWorld()->SpawnActor<ATwinStickNPC>(NPCClass, SpawnTransform);
	}

	// 增加生成计数
	++SpawnCount;

	// 若本批还有剩余 NPC，设置延迟后继续生成
	if (SpawnCount < SpawnGroupSize)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimer, this, &ATwinStickSpawner::SpawnNPC, FMath::RandRange(MinSpawnDelay, MaxSpawnDelay), false);
	}

}
