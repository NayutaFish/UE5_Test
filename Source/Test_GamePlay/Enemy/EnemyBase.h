// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.generated.h"

class UStaticMeshComponent;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeathSignature, AActor*, DeadEnemy);

/**
 * Minimal enemy base for the first room-flow pass.
 *
 * Owns health, damage, death notification, and debug death entry points.
 * AI, movement, attacks, animation, and drops are intentionally left out.
 */
UCLASS(Blueprintable)
class TEST_GAMEPLAY_API AEnemyBase : public AActor
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	bool bIsDead = false;

public:
	UPROPERTY(BlueprintAssignable, Category = "Enemy|Events")
	FOnEnemyDeathSignature OnEnemyDeath;

public:
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ApplyDamageToEnemy(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void TestDie();

protected:
	void Die();
};
