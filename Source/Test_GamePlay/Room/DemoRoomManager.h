// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoRoomManager.generated.h"

class AEnemyBase;
class AEnemySpawnPoint;
class AActor;
class USceneComponent;

/**
 * Demo room manager.
 *
 * Runs the first spawn-flow pass:
 * collect spawn points, spawn enemies, listen for death, and report room clear.
 */
UCLASS(Blueprintable)
class TEST_GAMEPLAY_API ADemoRoomManager : public AActor
{
	GENERATED_BODY()

public:
	ADemoRoomManager();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Spawn")
	TSubclassOf<AEnemyBase> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Spawn")
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Spawn", meta = (ClampMin = "0.0", Units = "s"))
	float StartDelay = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Runtime")
	int32 AliveEnemyCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Runtime")
	TArray<TObjectPtr<AEnemyBase>> SpawnedEnemies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Runtime")
	TArray<TObjectPtr<AEnemySpawnPoint>> SpawnPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Runtime")
	bool bRoomStarted = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Runtime")
	bool bRoomCleared = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Room")
	void CollectSpawnPoints();

	UFUNCTION(BlueprintCallable, Category = "Room")
	void StartRoom();

	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnWave();

	UFUNCTION(BlueprintCallable, Category = "Room")
	void CheckRoomClear();

protected:
	UFUNCTION()
	void HandleEnemyDeath(AActor* DeadEnemy);

	UFUNCTION(BlueprintImplementableEvent, Category = "Room")
	void OnRoomClear();
};
