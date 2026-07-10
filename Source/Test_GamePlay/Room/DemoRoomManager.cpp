// Copyright Epic Games, Inc. All Rights Reserved.

#include "Room/DemoRoomManager.h"

#include "Components/SceneComponent.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Room/EnemySpawnPoint.h"
#include "Roguelike/RoguelikeRewardManager.h"
#include "Test_GamePlay.h"
#include "TimerManager.h"

ADemoRoomManager::ADemoRoomManager()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
}

void ADemoRoomManager::BeginPlay()
{
	Super::BeginPlay();

	CollectSpawnPoints();

	if (bAutoStart)
	{
		FTimerHandle StartTimerHandle;
		GetWorldTimerManager().SetTimer(
			StartTimerHandle,
			this,
			&ADemoRoomManager::StartRoom,
			StartDelay,
			false
		);
	}
}

void ADemoRoomManager::CollectSpawnPoints()
{
	SpawnPoints.Empty();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, AEnemySpawnPoint::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (AEnemySpawnPoint* SpawnPoint = Cast<AEnemySpawnPoint>(Actor))
		{
			SpawnPoints.Add(SpawnPoint);
		}
	}

	UE_LOG(LogTest_GamePlay, Log, TEXT("RoomManager found %d spawn points."), SpawnPoints.Num());
}

void ADemoRoomManager::StartRoom()
{
	if (bRoomStarted)
	{
		return;
	}

	if (!EnemyClass)
	{
		UE_LOG(LogTest_GamePlay, Error, TEXT("EnemyClass is not set in DemoRoomManager."));
		return;
	}

	if (SpawnPoints.Num() <= 0)
	{
		UE_LOG(LogTest_GamePlay, Error, TEXT("No spawn points found."));
		return;
	}

	bRoomStarted = true;
	bRoomCleared = false;

	UE_LOG(LogTest_GamePlay, Log, TEXT("Room started."));

	SpawnWave();
}

void ADemoRoomManager::SpawnWave()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (!EnemyClass)
	{
		UE_LOG(LogTest_GamePlay, Error, TEXT("EnemyClass is not set in DemoRoomManager."));
		return;
	}

	if (SpawnPoints.Num() <= 0)
	{
		UE_LOG(LogTest_GamePlay, Error, TEXT("No spawn points found."));
		return;
	}

	AliveEnemyCount = 0;
	SpawnedEnemies.Empty();

	for (AEnemySpawnPoint* SpawnPoint : SpawnPoints)
	{
		if (!IsValid(SpawnPoint))
		{
			continue;
		}

		AEnemyBase* SpawnedEnemy = World->SpawnActor<AEnemyBase>(
			EnemyClass,
			SpawnPoint->GetSpawnTransform()
		);

		if (!IsValid(SpawnedEnemy))
		{
			continue;
		}

		SpawnedEnemy->OnEnemyDeath.AddDynamic(this, &ADemoRoomManager::HandleEnemyDeath);

		SpawnedEnemies.Add(SpawnedEnemy);
		++AliveEnemyCount;

		UE_LOG(LogTest_GamePlay, Log, TEXT("Spawned enemy: %s"), *SpawnedEnemy->GetName());
	}

	UE_LOG(LogTest_GamePlay, Log, TEXT("Wave spawned. AliveEnemyCount = %d"), AliveEnemyCount);

	if (AliveEnemyCount <= 0)
	{
		CheckRoomClear();
	}
}

void ADemoRoomManager::HandleEnemyDeath(AActor* DeadEnemy)
{
	AEnemyBase* DeadEnemyBase = Cast<AEnemyBase>(DeadEnemy);
	if (!IsValid(DeadEnemyBase))
	{
		return;
	}

	const int32 RemovedCount = SpawnedEnemies.Remove(DeadEnemyBase);
	if (RemovedCount <= 0)
	{
		return;
	}

	AliveEnemyCount = FMath::Max(0, AliveEnemyCount - RemovedCount);

	UE_LOG(LogTest_GamePlay, Log, TEXT("Enemy died. AliveEnemyCount = %d"), AliveEnemyCount);

	CheckRoomClear();
}

void ADemoRoomManager::CheckRoomClear()
{
	if (bRoomCleared || AliveEnemyCount > 0)
	{
		return;
	}

	bRoomCleared = true;

	UE_LOG(LogTest_GamePlay, Log, TEXT("Room Clear!"));
	if (RewardManager)
	{
		RewardManager->ShowRewardAfterRoomClear();
	}
	OnRoomClear();
}
