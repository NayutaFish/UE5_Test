// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HikariPlayerCharacter.generated.h"

class UInputAction;
class AAttackAreaBase;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDeathSignature, AActor*, DeadPlayer);

UCLASS()
class TEST_GAMEPLAY_API AHikariPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHikariPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Stats", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Stats")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
	bool bIsDead = false;

	UPROPERTY(BlueprintAssignable, Category = "Player|Events")
	FOnPlayerDeathSignature OnPlayerDeath;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void HandleDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void TestDie();

protected:
	void Die();
	void OnAttack();
	void OnSkill();

	/** 攻击范围蓝图类（在蓝图中赋值） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<AAttackAreaBase> AttackAreaClass;
};
